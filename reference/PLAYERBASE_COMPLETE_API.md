# PlayerBase Complete API

**Source:** raw_data/PlayerBase.c (268 KB vanilla code)
**Analysis:** Complete method signatures for player

---

## 🔍 Overview

**PlayerBase** is the main player controller class.

**Inheritance:** `Entity → EntityAI → Man → ManBase → DayZPlayer → DayZPlayerInstantiate → PlayerBase`

---

## 📋 Connection Events

```c
// Called when player connects
override void OnConnect()

// Called when player disconnects
override void OnDisconnect()
```

---

## 📋 Stats (Health, Blood, etc.)

```c
// Get stats
PlayerStats GetPlayerStats()

// Specific stats
Stat GetStat(string statName)
Stat<float> GetStatStamina()
Stat<float> GetStatEnergy()
Stat<float> GetStatWater()
Stat<float> GetStatHealth()
Stat<float> GetStatBlood()

// Modify stats
void AddHealth(string zone, string healthType, float value)
void AddBlood(float value)
void AddStamina(float value)
```

---

## 📋 Modifiers & Symptoms

```c
// Modifiers manager
ModifiersManager GetModifiersManager()

// Add modifier
void InsertModifier(Modifier m)
void RemoveModifier(Modifier m)

// Symptom manager
SymptomManager GetSymptomManager()

// Add symptom
void AddSymptom(int symptomID)
void RemoveSymptom(int symptomID)
```

---

## 📋 Health State

```c
// Check state
bool IsAlive()
bool IsUnconscious()
bool IsBleeding()

// Health level
int GetHealth01(string zone, string healthType)
void SetHealth01(float value)

// Broken legs
eBrokenLegs GetBrokenLegs()
void SetBrokenLegs(eBrokenLegs state)
```

---

## 📋 Position & Movement

```c
// Position (inherited from Entity)
vector GetPosition()
void SetPosition(vector pos)

// Movement
vector GetOrientation()
void SetOrientation(vector orientation)

// Velocity
vector GetVelocity()
vector GetVelocityVector()
```

---

## 📋 Inventory

```c
// Get inventory
PlayerInventory GetInventory()

// Hands
EntityAI GetHumanInventory().GetEntityInHands()
void TakeEntityToHands(EntityAI item)

// Quick bar
QuickBarBase GetQuickBar()
void OnQuickBarUsed(int index)
```

---

## 📋 Actions

```c
// Action manager
ActionManagerBase GetActionManager()

// Perform action
void PerformAction(int actionID, ActionData actionData)

// Interrupt action
void InterruptAction()
```

---

## 📋 Network

```c
// RPC
override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)

// Get identity
PlayerIdentity GetIdentity()

// Get ID
string GetID()
```

---

## 📋 Misc

```c
// Display name
string GetDisplayName()

// Check if player
bool IsPlayer()

// Get camera
DayZPlayerCameraBase GetCamera()

// Emote manager
EmoteManager GetEmoteManager()

// Weapon manager
WeaponManager GetWeaponManager()
```

---

## 🎯 Common Usage Patterns

### Player Join/Leave

```c
modded class PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Load player data
        Print("Player connected: " + GetIdentity().GetName());
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save player data
        Print("Player disconnected: " + GetIdentity().GetName());
    }
}
```

### Health Monitoring

```c
modded class PlayerBase
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Check health
        float health = GetHealth01("GlobalHealth", "Health");

        if (health < 0.2)
        {
            Print("Player is critically injured!");
        }
    }
}
```

### Modifier System

```c
modded class PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Add custom modifier
       ModifiersManager mm = GetModifiersManager();
        mm.InsertModifier(ModifierConst.CUSTOM_MODIFIER);
    }
}
```

### Custom Stat Effect

```c
class StaminaBoostPlayer extends PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Boost stamina on connect
        GetStatStamina().Add(100);
    }
}
```

---

## 🔗 Related

→ [EntityAI API](ENTITYAI_COMPLETE_API.md) - Base methods
→ [ItemBase API](ITEMBASE_COMPLETE_API.md) - Item methods

---

*Last updated: 2025-01-18*
