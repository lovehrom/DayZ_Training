# ItemBase Complete API

**Source:** raw_data/ItemBase.c (140 KB vanilla code)
**Analysis:** Complete method signatures for items

---

## 🔍 Overview

**ItemBase** is the base class for all items (weapons, food, clothing, etc.).

**Inheritance:** `Entity → EntityAI → InventoryItem → ItemBase`

---

## 📋 Quantity Methods

```c
// Get quantity
float GetQuantity()
int GetQuantityInit()
int GetQuantityMax()
int GetQuantityMin()

// Set quantity
void SetQuantity(float qty)
void AddQuantity(float delta)

// Check
bool HasQuantity()
bool CanBeSplit()
bool IsFullQuantity()

// Split
ItemBase SplitItem(float qty)

// Combine
bool CanBeCombined(EntityAI other)
bool CombineItems(EntityAI other, bool stack)
```

---

## 📋 Wetness Methods

```c
// Get wetness
float GetWet()
float GetWetMax()
float GetWetMin()

// Set wetness
void SetWet(float wet)
void AddWet(float delta)

// Levels
EWetnessLevel GetWetnessLevel() // DRY, DAMP, WET, SOAKING, DRENCHED
bool IsDry()
bool IsWet()
bool IsSoaking()
bool IsDrenched()
```

---

## 📋 Temperature Methods

```c
// Get
float GetTemperature()
float GetTemperatureMax()
float GetTemperatureMin()

// Set
void SetTemperature(float temp)
void AddTemperature(float delta)

// Food temperature
float GetFoodTemperature()
void SetFoodTemperature(float temp)
```

---

## 📋 Configuration Methods

```c
// Config get (various types)
int ConfigGetInt(string path)
float ConfigGetFloat(string path)
bool ConfigGetBool(string path)
string ConfigGetString(string path)

// Config arrays
void ConfigGetTextArray(string path, out TStringArray output)

// Check exists
bool ConfigIsExisting(string path)
```

---

## 📋 Inventory Methods

```c
// Can be...
bool CanPutAsAttachment(EntityAI parent)
bool CanPutInCargo(EntityAI parent)
bool CanPutIntoHands(EntityAI parent)

// Get location
InventoryLocation GetInventoryLocation()

// Movement
void OnItemLocationChanged(InventoryLocation old, InventoryLocation new)
```

---

## 📋 Actions

```c
// Single-use actions (eat, drink, etc.)
void SetSingleUseAction(int action)
void GetSingleUseActions(out TIntArray actions)

// Continuous actions
void SetContinuousAction(int action)
void GetContinuousActions(out TIntArray actions)

// Interact actions
void SetInteractAction(int action)
void GetInteractActions(out TIntArray actions)
```

---

## 📋 Condition/Health

```c
// Health override
override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source)

// Damage
bool IsDamaged()
bool IsRuined()
float GetHealth01()
```

---

## 📋 Consumables

```c
// Eat/Drink
void Consume(float amount, PlayerBase consumer)

// Modifiers transfer
void TransferModifiers(PlayerBase receiver)
```

---

## 📋 Sound

```c
// Play sound
void PlaySound(string sound, bool playEx = true)
void StopSound(string sound)
```

---

## 📋 Misc

```c
// Display name
string GetDisplayName()

// Type
string GetType()

// Class name
string ClassName()

// View index
int GetViewIndex()
void SetViewIndex(int index)

// Can be taken
bool IsTakeable()
bool IsBeingPlaced()
```

---

## 🎯 Common Usage Patterns

### Custom Consumable

```c
class CustomFood extends Edible_Base
{
    override void Consume(float amount, PlayerBase consumer)
    {
        super.Consume(amount, consumer);

        // Custom effect
        consumer.GetStatEnergy().Add(500);
    }
}
```

### Stackable Item

```c
class CustomAmmo extends Ammunition_Base
{
    override bool CanBeSplit()
    {
        return true;
    }

    override void SetQuantity(float qty)
    {
        super.SetQuantity(qty);

        // Update ammo count
        if (IsAmmoPile())
        {
            Ammunition_Base ammo = Ammunition_Base.Cast(this);
            ammo.ServerSetAmmoCount((int)qty);
        }
    }
}
```

### Temperature Item

```c
class WarmItem extends ItemBase
{
    override void OnCEUpdate()
    {
        super.OnCEUpdate();

        // Slowly cool down
        if (GetTemperature() > GetTemperatureMin())
        {
            AddTemperature(-0.1);
        }
    }
}
```

---

## 🔗 Related

→ [EntityAI API](ENTITYAI_COMPLETE_API.md) - Base methods
→ [PlayerBase API](PLAYERBASE_COMPLETE_API.md) - Player methods

---

*Last updated: 2025-01-18*
