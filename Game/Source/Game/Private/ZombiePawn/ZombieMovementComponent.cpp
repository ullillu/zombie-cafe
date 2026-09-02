#include "ZombiePawn/ZombieMovementComponent.h"

UZombieMovementComponent::UZombieMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    NavAgentProps.bCanWalk = true;
    NavAgentProps.bCanFly = false;
    NavAgentProps.bCanSwim = false;

    NavAgentProps.AgentRadius = 34.f;
    NavAgentProps.AgentHeight = 176.f;
}

float UZombieMovementComponent::GetMaxSpeed() const
{
    return MaxSpeed;
}

void UZombieMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("RequestDirectMove: %s"),
        *MoveVelocity.ToString()
    );

    if (MoveVelocity.IsNearlyZero())
    {
        RequestedVelocity = FVector::ZeroVector;
        return;
    }

    const FVector Direction = MoveVelocity.GetSafeNormal();
    const float Speed = bForceMaxSpeed ? MaxSpeed : FMath::Min(MoveVelocity.Size(), MaxSpeed);

    RequestedVelocity = Direction * Speed;
}

void UZombieMovementComponent::RequestPathMove(const FVector& MoveInput)
{
    if (MoveInput.IsNearlyZero())
    {
        RequestedVelocity = FVector::ZeroVector;
        return;
    }

    RequestedVelocity = MoveInput.GetSafeNormal() * MaxSpeed;
}

void UZombieMovementComponent::StopActiveMovement()
{
    RequestedVelocity = FVector::ZeroVector;
    Velocity = FVector::ZeroVector;

    Super::StopActiveMovement();
}

void UZombieMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    if (RequestedVelocity.IsNearlyZero())
    {
        Velocity = FVector::ZeroVector;
        return;
    }

    Velocity = RequestedVelocity;

    const FVector Delta = Velocity * DeltaTime;

    FHitResult Hit;

    SafeMoveUpdatedComponent(
        Delta,
        UpdatedComponent->GetComponentQuat(),
        true,
        Hit
    );

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(
            Delta,
            1.f - Hit.Time,
            Hit.Normal,
            Hit
        );
    }

    const FVector MoveDirection = Velocity.GetSafeNormal2D();

    if (!MoveDirection.IsNearlyZero())
    {
        const FRotator TargetRotation =
            MoveDirection.Rotation();

        const FRotator NewRotation =
            FMath::RInterpConstantTo(
                UpdatedComponent->GetComponentRotation(),
                TargetRotation,
                DeltaTime,
                RotationSpeed
            );

        UpdatedComponent->SetWorldRotation(NewRotation);
    }
}

bool UZombieMovementComponent::CanStartPathFollowing() const
{
    return PawnOwner != nullptr &&
        UpdatedComponent != nullptr;
}

bool UZombieMovementComponent::CanStopPathFollowing() const
{
    return true;
}