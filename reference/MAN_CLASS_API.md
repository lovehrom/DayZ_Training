# Man Class API

**Source:** raw_data/Man.c (36 KB vanilla code)
**Analysis:** Man class methods for animations and controls

---

## 🔍 Overview

**Man** is the base human class. PlayerBase inherits from Man.

**Inheritance:** `Entity → EntityAI → Man`

---

## 📋 Animation

```c
// Start animation
void StartCommandAnim(int animID)
void StopCommandAnim()

// Animation state
bool IsAnimationPlaying()
int GetAnimInstance()
```

---

## 📋 Movement

```c
// Movement state
bool IsRunning()
bool IsWalking()
bool IsCrouching()
bool IsProne()
bool IsSwimming()

// Get stance
HumanMovementState GetMovementState()
```

---

## 📋 Hands

```c
// Raised hands
bool IsRaised()
void Raise(bool state)

// In hands
EntityAI GetEntityInHands()
void SetEntityInHands(EntityAI item)
```

---

## 📋 Camera

```c
// Get camera
DayZPlayerCameraBase GetCamera()

// Camera state
bool IsCamera3rdPerson()
```

---

## 🔗 Related

→ [PlayerBase API](PLAYERBASE_COMPLETE_API.md) - Player-specific methods

---

*Last updated: 2025-01-18*
