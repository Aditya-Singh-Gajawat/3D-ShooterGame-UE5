# AMyCharacter Class - README

This `AMyCharacter` class represents the main character in the UE5 shooter project, responsible for character movement, actions like firing a pistol, and executing an ultimate ability. The class includes various components such as camera control, movement logic, animations, sound effects, and visual effects. Below is a detailed explanation of the code, focusing on the **trace logic** and its purpose.

## Overview of Key Components

### Components:
1. **Camera and Spring Arm**: 
   - The camera follows the character from behind via a spring arm (`CameraBoom`), providing a dynamic view of the character.
   - The `FollowCamera` component is attached to the spring arm to follow the character's movement and orientation.

2. **Movement**: 
   - Movement is implemented using the `MoveForward` and `MoveRight` functions, where the character’s movement direction is based on the control rotation.

3. **Ultimate Ability**: 
   - The character has an **ultimate ability** that applies force and effects when triggered. It also involves disabling player input for the duration of the ability animation.

4. **Combat**: 
   - The character can fire a pistol, with associated sound effects, animations, and particle effects (e.g., muzzle flash, beam, and hit effects).

## Trace Logic Explanation

### Trace in `SpawnFX` Function

One of the notable features of this class is the **trace functionality** used to simulate the firing of a weapon (pistol) and the detection of hits. This logic is executed in the `SpawnFX` function.

```cpp
FVector2D ViewportSize;
if (GEngine && GEngine->GameViewport)
{
    GEngine->GameViewport->GetViewportSize(ViewportSize);
}

FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f); // Coordinates of the crosshair
FVector CrosshairWorldPosition;
FVector CrosshairWorldDirection;

// Converts screen space (crosshair position) to world space (raycast direction)
bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
(
    UGameplayStatics::GetPlayerController(this, 0),
    CrosshairLocation,
    CrosshairWorldPosition,
    CrosshairWorldDirection
);

if (bScreenToWorld)
{
    UE_LOG(LogTemp, Warning, TEXT("Deproject Successful"));
    FHitResult ScreenHitResult;
    const FVector Start = CrosshairWorldPosition;
    const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50000.f; // Long distance for raycast

    FVector BeamEndPoint = End; // If no hit, the beam reaches the end of the line

    // Perform the trace (line trace) to check for hits
    GetWorld()->LineTraceSingleByChannel(ScreenHitResult, Start, End, ECollisionChannel::ECC_Visibility);

    if (ScreenHitResult.bBlockingHit) // If there was a hit
    {
        UE_LOG(LogTemp, Warning, TEXT("Trace Successful"));
        BeamEndPoint = ScreenHitResult.Location; // The hit location becomes the end point of the beam

        // Spawn hit effect at the hit location
        if (PistolHitFX)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolHitFX, ScreenHitResult.Location);
        }
    }

    // Spawn the beam effect (the ray) from the start to the end of the line (or the hit point)
    if (PistolBeamFX)
    {
        UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolBeamFX, SocketTransform);
        if (Beam)
        {
            Beam->SetVectorParameter(FName("Target"), BeamEndPoint); // Set the beam's target to the hit location
        }
    }
}
```

### Breakdown of Trace Logic:

1. **Screen-to-World Conversion**:
   - The trace starts with **deprojecting the screen coordinates** of the crosshair (which is at the center of the screen by default) into world space. This converts the screen position (a 2D point) into a direction in the 3D world.
   - `DeprojectScreenToWorld` is used for this conversion. The result is the start position (`CrosshairWorldPosition`) and the direction (`CrosshairWorldDirection`) of the ray in world space.

2. **Line Trace**:
   - A **line trace (raycast)** is performed using the `LineTraceSingleByChannel` function. The trace starts from the crosshair's world position and extends in the direction the crosshair is pointing (using `CrosshairWorldDirection`).
   - The trace has a length of 50,000 units (`End = CrosshairWorldPosition + CrosshairWorldDirection * 50000.f`), meaning it checks a long distance in the game world.

3. **Hit Detection**:
   - If the trace hits an object (i.e., `ScreenHitResult.bBlockingHit` is true), the code will log a successful hit and adjust the end point of the beam to the hit location.
   - A **hit effect (particle system)** is spawned at the hit location if `PistolHitFX` is defined.

4. **Beam Effect**:
   - The beam is then drawn from the character to the hit point (or maximum range if there’s no hit) using the `PistolBeamFX`. This is a visual representation of the raycast, often used in shooting mechanics to indicate the trajectory of a shot.

### Why This Trace Logic Was Implemented:

1. **Realistic Weapon Firing**:
   - This trace simulates the behavior of a firearm, where the bullet travels in a straight line and hits an object at a certain distance. By checking for hits and spawning effects (like a hit marker or impact visual effects), the player gets visual feedback about the interaction with the world.

2. **Crosshair to World Space**:
   - The trace starts at the crosshair's position, which is the target the player is aiming at. The conversion from screen space to world space ensures that the trace is aligned with the player's view, providing accurate firing behavior.

3. **Particle and Sound Effects**:
   - By spawning visual effects (e.g., muzzle flash, hit effects, and beam), the code provides immediate feedback to the player, enhancing the experience of shooting. These effects are not only cosmetic but also help the player understand if they’ve successfully hit an enemy or an object.

4. **Player Interaction with the Environment**:
   - The use of `LineTrace` allows the player to interact with the environment, triggering events like damaging enemies or activating game mechanics based on the trace hit.

## Ultimate Ability Logic

The ultimate ability is a powerful feature that applies a force to the character, propelling them in a certain direction. The logic is split into two functions:

1. **`DelayedUltimateAbility()`**: 
   - After the ultimate animation is played, this function applies the force to the character. The direction and magnitude of the force are calculated and then applied via `AddImpulse()`.

2. **`DelayedUltimateAbilityEmitter()`**: 
   - This function triggers a particle effect and sound cue related to the ultimate ability, such as a "bazooka" firing effect, providing visual and audio feedback.

### Why Delay with Timers?

Delays are used to synchronize animations, sound, and effects with the actual gameplay. For example:
- **Animation timing**: The character's animation (such as an ultimate ability animation) has a specific duration. The `EnablePlayerInput` function re-enables input only after the animation has completed.
- **Ultimate ability**: The character’s force application happens after a delay to give the player time to complete the animation before impacting the game world.

Timers are used to create this coordinated effect, ensuring that the game's actions are synchronized with player input and visual effects.

---

## Conclusion

This class combines character movement, combat, and special abilities with a focus on providing smooth and interactive gameplay. The trace logic plays an important role in combat, providing the player with accurate feedback on their actions. The overall design makes use of Unreal Engine's powerful systems for particle effects, sound cues, and character animation to enhance the player’s experience.
---

## License
This project is open-source and available for personal use.

## Author
**Aditya Singh Gajawat**