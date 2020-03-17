#include "GJCharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

UGJCharacterMovementComponent::UGJCharacterMovementComponent()
{	
	JumpCooldown = 0.4f;
	CurrentJumpCooldown = 0.f;
	LaunchDistance = 2000.f;
	SwordDashDistance = 1000.f;
	JumpsAvailable = 1;
}

void UGJCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentJumpCooldown = FMath::Max(0.f, CurrentJumpCooldown - DeltaTime);
	
	if (CurrentJumpCooldown <= 0.f)
	{
		JumpsAvailable = FMath::Min(1u, JumpsAvailable + 1);
		
		CurrentJumpCooldown += JumpCooldown;
	}
}

bool UGJCharacterMovementComponent::CanAttemptJump() const
{
	const bool bCanJump = Super::CanAttemptJump();
	
	return bCanJump && !IsFalling() && JumpsAvailable > 0;
}

void UGJCharacterMovementComponent::Jump()
{
	if (GetCharacterOwner()->CanJump())
	{	
		--JumpsAvailable;
	}
}

void UGJCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UGJCharacterMovementComponent, JumpsAvailable);
}
