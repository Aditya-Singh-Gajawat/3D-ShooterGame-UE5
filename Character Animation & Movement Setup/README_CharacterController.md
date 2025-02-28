# UE5 Shooter Character

## Overview
This project implements a third-person shooter character in Unreal Engine 5. The character features movement mechanics, animation updates, and shooting mechanics, using C++ classes for precise control over gameplay elements.

## Features
- **Character Movement**: Supports forward, backward, and lateral movement with smooth camera control.
- **Animation System**: Uses an animation instance (`MyAnimInstance`) to update character speed and acceleration.
- **Shooting Mechanics**: Implements a pistol firing system with muzzle flash particle effects and sound cues.
- **Camera System**: Includes a spring arm and follow camera for third-person perspective.

## File Structure
```
/UE5_Shooter
│-- /Source
│   │-- /UE5POINT5_SHOOTER
│   │   │-- MyCharacter.h
│   │   │-- MyCharacter.cpp
│   │   │-- MyAnimInstance.h
│   │   │-- MyAnimInstance.cpp
│-- /Config
│-- /Content
│-- UE5POINT5_SHOOTER.uproject
```

## Class Breakdown

### AMyCharacter
This class represents the player character and includes functionality for movement, camera handling, and shooting.

#### Variables
- `CameraBoom`: Positions the camera behind the character.
- `FollowCamera`: The main camera used for third-person view.
- `BaseTurnRate` & `BaseLookUpRate`: Controls the speed of camera rotation.
- `PistolSoundCue`: Stores the sound effect for the pistol.
- `PistolMuzzleFX`: Handles the muzzle flash effect when firing the pistol.

#### Methods
- `MoveForward(float Value)`: Moves the character forward/backward.
- `MoveRight(float Value)`: Moves the character left/right.
- `TurnAtRate(float Rate)`: Rotates the character horizontally.
- `LookUpAtRate(float Rate)`: Rotates the camera vertically.
- `FirePistol()`: Fires the pistol, triggering visual and audio effects.
- `SetupPlayerInputComponent()`: Binds player inputs to corresponding actions.

### UMyAnimInstance
This class manages animation properties based on character movement.

#### Variables
- `Drongo`: Stores a reference to the character instance.
- `Speed`: Stores the character's movement speed.
- `bIsAccelerating`: Indicates whether the character is accelerating.

#### Methods
- `UpdateAnimationProperties(float DeltaTime)`: Updates movement-related properties for animation.
- `NativeInitializeAnimation()`: Initializes animation instance and references the player character.


## Controls
| Action       | Input |
|-------------|-------|
| Move Forward | W |
| Move Backward | S |
| Move Right | D |
| Move Left | A |
| Look Around | Mouse |
| Fire Pistol | Left Mouse Button |

## Future Improvements
- Implement aiming mechanics.
- Add reload functionality.
- Integrate additional weapon types.
- Expand animations for crouching and jumping.

## License
This project is open-source and available for personal use.

## Author
**Aditya Singh Gajawat**


