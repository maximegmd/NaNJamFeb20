#include "GJCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "GJWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "GJCharacterMovementComponent.h"
#include "GJWeaponComponent.h"
#include "GJPlayerController.h"

AGJCharacter::AGJCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGJCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = false;
	GetCharacterMovement()->bSnapToPlaneAtStart = false;
	GetCharacterMovement()->SetIsReplicated(true);

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 900.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Weapon component
	WeaponComponent = CreateDefaultSubobject<UGJWeaponComponent>(TEXT("Weapon"));
	WeaponComponent->bEditableWhenInherited = true;
	WeaponComponent->SetIsReplicated(true);
	WeaponComponent->SetupAttachment(GetMesh(), FName(TEXT("RightHandEquipSocket")));

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bPerformingAttack = false;
	
	// Settings
	BaseTurnRate = 450.f;
	LastSimpleAttackTimer = 0.f;
	MaxHealth = 100.f;
	Health = GetMaximumHealth();
	AttackActionCount = 1;
}

void AGJCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if(HasAuthority())
		LastSimpleAttackTimer = FMath::Clamp(LastSimpleAttackTimer - DeltaSeconds, 0.f, 100.f);
}

void AGJCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}

float AGJCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		Health -= DamageAmount;

		AGJPlayerController* GJController = Cast<AGJPlayerController>(GetController());
		if(GJController && Health <= 0.001f)
		{
			GJController->NotifyDeath();
			
		}
	}

	return DamageAmount;
}

void AGJCharacter::Jump()
{
	ServerJump();
}

void AGJCharacter::ImmediateSimpleAttack()
{
	ServerImmediateSimpleAttack();
}

bool AGJCharacter::IsWeaponEquipped() const
{
	return WeaponComponent->IsEquipped();
}

bool AGJCharacter::WantsSimpleAttack() const
{
	return LastSimpleAttackTimer > 0.f;
}

void AGJCharacter::DashSword()
{
	ServerDashSword();
}

void AGJCharacter::IncrementAttackActionCount()
{
	AttackActionCount++;
}

float AGJCharacter::GetMaximumHealth() const
{
	return MaxHealth;
}

void AGJCharacter::PickUpWeapon(const TSubclassOf<AGJWeapon>& AWeaponClass)
{
	if(HasAuthority())
	{
		this->WeaponClass = AWeaponClass;

		WeaponComponent->Equip(WeaponClass);
	}
}

void AGJCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void AGJCharacter::SetRotation(FRotator Rotation)
{
	SetActorRotation(Rotation);

	if(!HasAuthority())
		ServerSetActorRotation(Rotation);
}

void AGJCharacter::DrawWeapon()
{
	ServerDrawWeapon();
}

bool AGJCharacter::DoJump(float Distance)
{
	auto MovementComponent = GetCharacterMovementComponent();
	
	if (CanJump())
	{
		auto Direction = GetActorForwardVector() + GetActorUpVector() * 0.02f;
		Direction.Normalize();

		LaunchCharacter(Direction * Distance, true, true);

		MovementComponent->Jump();

		return true;
	}

	return false;
}

void AGJCharacter::DoDrawWeapon()
{
	if (!WeaponComponent->IsEquipped())
	{
		WeaponComponent->Equip(WeaponClass);
	}
	else
	{
		WeaponComponent->Unequip();
	}
}

void AGJCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

void AGJCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGJCharacter, WeaponComponent);
	DOREPLIFETIME(AGJCharacter, LastSimpleAttackTimer);
	DOREPLIFETIME(AGJCharacter, Health);
}

float AGJCharacter::Heal()
{
	if (HasAuthority())
	{
		Health = FMath::Min(Health + 20.f, GetMaximumHealth());
	}

	return Health;
}

void AGJCharacter::ServerSetActorRotation_Implementation(FRotator Rotation)
{
	if (!HasAuthority())
		return;
	
	SetActorRotation(Rotation);
}

void AGJCharacter::ServerJump_Implementation()
{
	if (!HasAuthority())
		return;
	
	DoJump(GetCharacterMovementComponent()->GetLaunchDistance());
}

void AGJCharacter::ServerDrawWeapon_Implementation()
{
	if (!HasAuthority())
		return;
	
	DoDrawWeapon();
}

void AGJCharacter::ServerImmediateSimpleAttack_Implementation()
{
	if (!HasAuthority())
		return;

	if (!IsWeaponEquipped())
		ServerDrawWeapon();

	if (IsWeaponEquipped())
		LastSimpleAttackTimer = 0.4f;
}

void AGJCharacter::ServerDashSword_Implementation()
{
	if (!HasAuthority())
		return;
	
	DoJump(GetCharacterMovementComponent()->GetSwordDashDistance());
}
