# **Ultimate Ability Implementation**  

## **Overview**  
This project implements an Ultimate Ability in Unreal Engine using C++. The ability allows the player to launch a powerful projectile, applying force to hit enemies and interacting with physics objects. The system integrates animations, effects, and input handling.  

---

## **Features**  
- **Ultimate Ability Activation** – Pressing a designated input key triggers the ability.  
- **Projectile-Based Attack** – Spawns an energy blast or similar projectile.  
- **Force Application** – Enemies and objects hit by the projectile receive an impulse.  
- **Character Knockback System** – The player experiences backward and upward force when using the ability.  
- **Animations and Effects** – The ability triggers animation sequences and sound effects.  
- **Cooldown Management** – Prevents spamming the ability.  
- **Physics-Based Interactions** – Realistic object movement based on mass and gravity.  

---

## **Force Calculation in Ultimate Ability**  

The `ApplyForceWhenUltimateIsUsed` function applies a **knockback force** when the ultimate ability is used. The force consists of:  
- **Backward Impulse** (opposite to the character’s forward direction).  
- **Upward Lift** (a vertical push to simulate an enhanced movement effect).  

### **Force Calculation Breakdown**  

#### 1. **Determining the Launch Direction**  
Retrieves and normalizes the character’s **forward vector** to determine force application:  
```cpp
FVector LaunchDirection = GetActorForwardVector();
LaunchDirection.Normalize();
```  

#### 2. **Calculating Adjusted Force Values**  
The force is adjusted based on the character’s **mass and gravity scale** to maintain realistic physics:  
```cpp
float Mass = GetCharacterMovement()->Mass;
float AdjustedForce = ForceMagnitude * GetCharacterMovement()->GravityScale * Mass;
float AdjustedUpward = UpwardForce * GetCharacterMovement()->GravityScale * Mass;
```  

#### 3. **Computing the Final Force Vector**  
Applies force in the opposite direction of movement with an **upward component**:  
```cpp
FVector Force = -LaunchDirection * ForceMagnitude + FVector(0.f, 0.f, UpwardForce);
```  

#### 4. **Applying the Impulse to the Character**  
Uses Unreal Engine’s physics system to apply the force:  
```cpp
GetCharacterMovement()->AddImpulse(Force, true);
```  
- `true` ensures that the force is applied **even when the character is airborne**.  

---

## **Ultimate Ability System Overview**  

The Ultimate Ability system consists of multiple components:  

### **1. Ability Activation**  
- The ability is triggered by an input key and calls `ActivateUltimateAbility()`, which executes `FireUltimate()`.  
- Checks cooldown status before execution.  

### **2. Projectile-Based Attack**  
- The ability spawns a **projectile** that interacts with enemies and objects.  
- Upon impact, it applies an **impulse force** based on physics calculations.  

### **3. Character Knockback System**  
- The character experiences a **recoil effect** when using the ability.  
- The force applied depends on the character’s **mass and gravity scale**.  

### **4. Animations & Effects**  
- A **custom animation montage** plays when the ability is activated.  
- **Sound effects and particle effects** enhance visual feedback.  

### **5. Cooldown Management**  
- The ability is placed on **cooldown** to prevent spamming.  
- A timer resets the ability after a defined period.  

---

## **How to Use**  
1. **Press the Ultimate Ability Key** to activate the ability.  
2. The character will execute an **animation**, launch a **projectile**, and experience a **knockback force**.  
3. The ability will be on **cooldown** before it can be used again.  

---

## **Future Improvements**  
- Add **UI indicators** for cooldown status.  
- Introduce **different projectile effects**.  
- Implement **enemy-specific interactions**.  
- Balance **damage and cooldown** for optimized gameplay.  

## License
This project is open-source and available for personal use.

## Author
**Aditya Singh Gajawat**

