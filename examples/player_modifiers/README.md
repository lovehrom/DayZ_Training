# Player Modifiers Example

**Status:** ✅ Working Example
**Source:** Redbull Energy Drinks (3643119261)
**Complexity:** Simple

---

## ⚡ 30-Second Summary

Temporary player stat modifiers (stamina, energy, hydration) from consumable items with timed effects.

---

## 🎯 Features

- ✅ **Stat Boosts** - Increase stamina/energy/hydration
- ✅ **Timed Effects** - Duration-based modifiers
- ✅ **Stacking** - Multiple effects
- ✅ **Consumable** - Eat/drink to activate

---

## 💡 Key Concepts

### Apply Modifier

```c
modded class PlayerBase
{
    override void OnConsume(float amount, PlayerBase player)
    {
        super.OnConsume(amount, player);

        // Boost stamina
        GetStatStamina().Add(100);

        // Boost energy
        GetStatEnergy().Add(500);
    }
}
```

### Timed Effect

```c
// Add stamina boost for 5 minutes
GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RemoveBoost, 300000, false);
```

---

## 🔗 Related Topics

→ [Player Modifiers](../reference/PLAYER_MODIFIERS.md) - Full docs
→ [Consumables](../topics/06_crafting.md) - Items

---

*Последнее обновление: 2025-01-18*
