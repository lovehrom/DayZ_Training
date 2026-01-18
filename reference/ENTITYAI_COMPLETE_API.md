# EntityAI Complete API

**Source:** raw_data/EntityAI.c (132 KB vanilla code)
**Analysis:** Complete method signatures with descriptions

---

## 🔍 Overview

**EntityAI** is the base class for all interactive items in DayZ (weapons, food, clothing, etc.).

**Inheritance:** `Entity → EntityAI → ItemBase → PlayerBase`

---

## 📋 Core Methods

### Lifecycle

```c
// Called when entity is initialized
void OnCEUpdate()
void OnCECreate()
void OnCEDelete(EntityAI owner)

// Called after config load
void DeferredInit()
void EOnInit(IEntity other, int extra)
```

### Health & Damage

```c
// Get health
float GetHealth(string "zoneName", string "healthType")
float GetHealth01(string "zoneName", string "healthType")

// Set health
void SetHealth(string "zoneName", string "healthType", float value)
void AddHealth(string "zoneName", string "healthType", float value)

// Check damage
bool IsDamaged()
bool IsRuined()

// Take damage
void TakeDamage(float damage, int damageType, EntityAI source)

// Override damage handling
override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source)
```

### Temperature

```c
// Get temperature
float GetTemperature()
float GetTemperatureInit()
float GetTemperatureMax()
float GetTemperatureMin()

// Set temperature
void SetTemperature(float temp)
void AddTemperature(float delta)

// Freezing/Thawing
bool IsFrozen()
float GetFreezeThawProgress()
void SetFrozenState(bool frozen)
```

### Weight

```c
// Get weight
float GetWeight()
float GetWeightEx() // Including attachments/cargo

// Update weight
void RefreshWeight()
void UpdateWeight(float delta)
```

### Inventory Events

```c
// Attachment events (called on parent)
void OnItemAttached(EntityAI item, string slot, EntityAI parent)
void OnItemDetached(EntityAI item, string slot, EntityAI parent)

// Cargo events (called on parent)
void OnItemAddedIntoCargo(EntityAI item, EntityAI parent)
void OnItemRemovedFromCargo(EntityAI item, EntityAI parent)
void OnItemMovedInCargo(EntityAI item, EntityAI parent)
```

### Network

```c
// RPC
void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)

// Synchronization
void SendRPC()
void OnVariablesSynchronized()
```

### Destruction

```c
// Death/Kill
void EEKilled(Object killer)
void SetHealth01(float "health") // If 0, calls EEKilled

// Replace with dead body
void ReplaceWithNew(EntityAI newEntity)
```

---

## 🔥 Event Invokers

**ScriptInvokers** - Subscribe to events:

```c
// Attach/Detach
ref ScriptInvoker m_OnItemAttached
ref ScriptInvoker m_OnItemDetached

// Cargo
ref ScriptInvoker m_OnItemAddedIntoCargo
ref ScriptInvoker m_OnItemRemovedFromCargo
ref ScriptInvoker m_OnItemMovedInCargo

// Damage/Death
ref ScriptInvoker m_OnHitByInvoker
ref ScriptInvoker m_OnKilledInvoker
```

**Usage:**
```c
override void OnItemAttached(EntityAI item, string slot, EntityAI parent)
{
    super.OnItemAttached(item, slot, parent);
    Print("Item " + item + " attached to slot " + slot);
}
```

---

## 🎯 Common Overrides

### Item with Custom Health

```c
class CustomItem extends ItemBase
{
    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source)
    {
        super.EEHitBy(damageResult, damageType, source);

        // Reduce damage by 50%
        float reducedDamage = damageResult.GetDamage(damageType) * 0.5;
        AddHealth("GlobalHealth", "Health", -reducedDamage);
    }
}
```

### Temperature Item

```c
class ColdItem extends ItemBase
{
    override void OnCEUpdate()
    {
        super.OnCEUpdate();

        // Slowly warm up
        if (GetTemperature() < GetTemperatureMax())
        {
            AddTemperature(0.1);
        }
    }
}
```

### Weight Modifier

```c
class HeavyItem extends ItemBase
{
    override float GetWeight()
    {
        return super.GetWeight() * 2.0; // Double weight
    }
}
```

---

## 🔗 Related

→ [ItemBase API](ITEMBASE_COMPLETE_API.md) - Item-specific methods
→ [PlayerBase API](PLAYERBASE_COMPLETE_API.md) - Player methods

---

*Last updated: 2025-01-18*
