# Medical Treatment System - Complete Reference

**Source:** Vanilla DayZ Medical + Common Mods
**Complexity:** Intermediate

---

## ⚡ Summary

Medical Treatment System enables players to heal each other using medical items (blood bags, saline, morphine). Critical for roleplay and survival servers.

---

## 🎯 Key Features

- ✅ **Blood Transfusion** - Restore blood with blood bags
- ✅ **Saline Drip** - Hydration and slow blood recovery
- ✅ **Morphine** - Cure fractures/pain
- ✅ **Epinephrine** - Revive unconscious players
- ✅ **Vitamin Bottles** - Custom nutrition system

---

## 🔧 Implementation

### Blood Transfusion

```c
class ActionBloodTransfusion: ActionSingleUseBase
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        // Target must be player
        PlayerBase targetPlayer = PlayerBase.Cast(target.GetObject());
        if (!targetPlayer)
            return false;

        // Need blood bag in hands
        if (!item.IsKindOf("BloodBag"))
            return false;

        // Target must have low blood
        if (targetPlayer.GetHealth("GlobalHealth", "Blood") > 4000)
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase targetPlayer = PlayerBase.Cast(action_data.m_Target.GetObject());
        ItemBase bloodBag = action_data.m_MainItem;

        // Transfer blood
        float bloodAmount = 1000;  // Full blood bag
        targetPlayer.AddHealth("GlobalHealth", "Blood", bloodAmount);

        // Empty blood bag
        bloodBag.AddHealth(-5000);  // Destroy item
    }
}
```

### Saline Drip

```c
class ActionSalineDrip: ActionContinuousBase
{
    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase player = PlayerBase.Cast(action_data.m_Target.GetObject());

        // Saline provides hydration + slow blood recovery
        player.AddHealth("GlobalHealth", "Water", 500);

        // Continuous blood recovery over time
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SalineEffect, 1000, true, player);
    }

    void SalineEffect(PlayerBase player)
    {
        player.AddHealth("GlobalHealth", "Blood", 50);  // Slow recovery
    }
}
```

### Medical Items Configuration

```cpp
// config.cpp
class BloodBagEmpty: ItemBase
{
    scope = 2;
    displayName = "Blood Bag (Empty)";
    weight = 100;
};

class BloodBagFull: BloodBagEmpty
{
    scope = 2;
    displayName = "Blood Bag (Full+)";
    quantity = 1;  // Single use
};

class SalineBag: ItemBase
{
    scope = 2;
    displayName = "Saline Bag";
    weight = 100;
};

class Morphine: ItemBase
{
    scope = 2;
    displayName = "Morphine Auto-Injector";
    weight = 50;
};
```

---

## 📝 Quick Reference

### Blood Transfusion
- Requires: Blood Bag (Full)
- Restores: 1000 blood
- Time: Instant

### Saline Drip
- Requires: Saline Bag
- Restores: 500 hydration + 50 blood/sec
- Time: 20 seconds

### Morphine
- Requires: Morphine Auto-Injector
- Cures: Fractures, pain
- Time: Instant

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
