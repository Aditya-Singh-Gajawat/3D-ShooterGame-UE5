# README.md - Explanation of Strafe, Aiming, Widget, and Related Systems

This document provides a detailed explanation of how certain gameplay mechanics—such as strafing, aiming, and widget interaction—are calculated and handled in the codebase. The features described here pertain to player movement, camera handling, and interactive widgets, which are critical for the character's behavior in the game.

## 1. **Strafe Calculation**

In this game, strafing refers to the ability of the character to move sideways while also rotating to face a specific direction. The code calculates and updates the `StrafeValue` and `LastStrafeValue` to achieve this behavior.

### How it Works:
- **StrafeValue Calculation:**
  - In `UMyAnimInstance::UpdateAnimationProperties()`, the `StrafeValue` is calculated by comparing the player's movement direction (`PlayerMoveRotation`) to the direction the player is aiming (`PlayerAimRotation`).
  - The difference in yaw angles between the two rotations is used to determine how much the character is strafing. This is done using the `NormalizedDeltaRotator` function, which normalizes the difference between the two rotations to get a relative yaw value.
  - If the player's character is moving (i.e., `Velocity.Size() > 0`), the `LastStrafeValue` is updated to store the previous frame's strafe value.
  
- **Usage of StrafeValue:**
  - The `StrafeValue` can be used to blend animations or modify the character's movement. For example, when the character is strafing left or right, the animation system can use this value to transition smoothly between movement animations.

### Code Snippet:
```cpp
StrafeValue = UKismetMathLibrary::NormalizedDeltaRotator(PlayerMoveRotation, PlayerAimRotation).Yaw;
if (Drongo->GetVelocity().Size() > 0.f)
{
    LastStrafeValue = StrafeValue;
}
```

## 2. **Aiming Mechanism**

The aiming functionality modifies the player's camera and movement behavior when the player enters the aiming state, typically used in first-person or third-person shooters.

### How it Works:
- **Aiming State:**
  - The `bIsAiming` boolean variable determines whether the character is currently aiming. This value is toggled by the `AimingPressed()` and `AimingReleased()` functions in the `AMyCharacter` class.
  - When `bIsAiming` is `true`, it influences the camera's field of view (FOV), zoom levels, and relative camera location.
  - The `CameraInterp()` function in `AMyCharacter` is responsible for smoothly transitioning the camera position and FOV during aiming. It interpolates the camera location to provide a smoother transition between normal and aiming modes.

- **Effects of Aiming:**
  - When aiming, the camera is zoomed in (with a smaller FOV), and its location is adjusted slightly forward and downward for a more tactical view.
  - The aim state also influences the player character's movement and how the character rotates in response to user input.

### Code Snippet:
```cpp
void AMyCharacter::CameraInterp(float DeltaTime)
{
    TargetCamLocation = bIsAiming ? FVector(250.f, 0.f, -50.f) : FVector(0.f, 0.f, 0.f);
    TargetCamRotation = bIsAiming ? FRotator(0.f, 0.f, 0.f) : FRotator(0.f, 0.f, 0.f); 
    float TargetFOV = bIsAiming ? 75.f : 90.f;

    FVector NewCamLocation = FMath::VInterpTo(FollowCamera->GetRelativeLocation(), TargetCamLocation, DeltaTime, ZoomInterpSpeed);
    FRotator NewCamRotation = FMath::RInterpTo(FollowCamera->GetRelativeRotation(), TargetCamRotation, DeltaTime, ZoomInterpSpeed);
    float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

    FollowCamera->SetRelativeLocation(NewCamLocation);
    FollowCamera->SetRelativeRotation(NewCamRotation);
    FollowCamera->FieldOfView = NewFOV;
}
```

## 3. **Widget Interaction (Weapon Widget)**

Widgets in the game are used to display items and their associated UI elements (like a weapon's UI widget). This system allows the player to interact with items in the game world, typically through collision detection.

### How it Works:
- **Widget Display:**
  - The `AMyItem` class contains a `UWidgetComponent` called `WeaponWidget`, which is used to display the UI for the weapon. This widget is initially set to be invisible when the game begins (`WeaponWidget->SetVisibility(false)`).
  - The `Tick()` function in `AMyCharacter` performs a line trace to detect whether the player is looking at an item in the world that has an associated widget.
  - If the player is looking at an item, the widget is made visible.

- **Line Trace for Widget Interaction:**
  - The line trace (or "raycast") checks whether the player's crosshair is hovering over an item. It uses the `TraceForWidget()` function, which calculates the world position of the crosshair and casts a ray from it to detect any interactable items (such as weapons).
  - If a hit is detected with an item, its associated widget is made visible.

### Code Snippet:
```cpp
void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FHitResult WidgetHitResult;
    FVector DummyHitLocation; // Dummy Variable
    TraceForWidget(WidgetHitResult, DummyHitLocation);

    if (WidgetHitResult.bBlockingHit)
    {
        AMyItem* Item = Cast<AMyItem>(WidgetHitResult.GetActor());
        if (Item && Item->ReturnWeaponWidget())
            Item->ReturnWeaponWidget()->SetVisibility(true);
    }
}

bool AMyCharacter::TraceForWidget(FHitResult& HitResult, FVector& HitLocation)
{
    if (!GEngine || !GEngine->GameViewport)
    {
        return false;
    }

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);
    FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
    (
        UGameplayStatics::GetPlayerController(this, 0),
        CrosshairLocation,
        CrosshairWorldPosition,
        CrosshairWorldDirection
    );

    if (!bScreenToWorld)
        return false;

    const FVector Start = CrosshairWorldPosition;
    const FVector End = Start + CrosshairWorldDirection * 50000.f;

    HitLocation = End; // If no hit, return the end location

    GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

    if (HitResult.bBlockingHit)
    {
        HitLocation = HitResult.Location;
        return true;
    }

    return false;
}
```

### Key Concepts:
- **Line Tracing**: Used for detecting interactive objects in the world by casting rays from the camera or crosshair.
- **Widget Visibility**: Based on whether the player is looking at a particular object, a widget will be made visible, aiding interaction.
  
### Notes:
- If the player is looking at an item (such as a weapon or collectible), the widget for that item becomes visible, allowing for UI-based interactions.
- The `TraceForWidget()` method handles this by using a raycast to determine if the player is aiming at something interactable. This process involves deprojecting the screen's crosshair position into world coordinates and then performing a line trace.

---

This document provides an overview of how strafing, aiming, and widget interactions are managed within the game. Each feature is implemented using Unreal Engine's systems such as animation blueprints, character movement, line tracing, and camera interpolation to create a responsive and immersive gameplay experience.
## License
This project is open-source and available for personal use.

## Author
**Aditya Singh Gajawat**