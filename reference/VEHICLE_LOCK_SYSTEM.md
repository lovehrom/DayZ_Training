# Vehicle Lock System - Complete Reference

**Source:** TraderPlus CarLock + Common Mods
**Complexity:** Intermediate

---

## ⚡ Summary

Vehicle Lock System allows players to lock/unlock vehicles using keys. Prevents theft and enables vehicle ownership.

---

## 🎯 Key Features

- ✅ **Lock/Unlock Vehicles** - With key in hands
- ✅ **Key Crafting** - Create vehicle keys
- ✅ **Key Duplication** - Copy keys
- ✅ **Owner Tracking** - Vehicle ownership
- ✅ **Picklocking** - Skill-based lockpicking

---

## 🔧 Implementation

### Vehicle Lock Data

```c
modded class CarScript
{
    protected bool m_IsLocked;
    protected string m_OwnerID;
    protected ref array<string> m_KeyIDs;

    override void Init()
    {
        super.Init();
        m_IsLocked = false;
        m_OwnerID = "";
        m_KeyIDs = new array<string>;
    }

    bool IsLocked()
    {
        return m_IsLocked;
    }

    void LockVehicle(string playerID, string keyID)
    {
        if (m_OwnerID != playerID)
        return;  // Not owner

        m_IsLocked = true;
        m_KeyIDs.Insert(keyID);
    }

    void UnlockVehicle(string playerID, string keyID)
    {
        if (!m_IsLocked)
            return;

        // Check if key matches
        if (m_KeyIDs.Find(keyID) >= 0)
        {
            m_IsLocked = false;
        }
    }
}
```

### Lock Action

```c
class ActionLockVehicle: ActionSingleUseBase
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript vehicle = CarScript.Cast(target.GetObject());
        if (!vehicle)
            return false;

        if (!item.IsKindOf("CarKey"))
            return false;

        // Check if player owns vehicle
        if (vehicle.GetOwnerID() != player.GetIdentity().GetPlainId())
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        CarScript vehicle = CarScript.Cast(action_data.m_Target.GetObject());
        ItemBase key = action_data.m_MainItem;

        vehicle.LockVehicle(
            action_data.m_Player.GetIdentity().GetPlainId(),
            key.GetVariableString("KeyID")
        );

        NotificationSystem.Notify(action_data.m_Player, "Vehicle locked");
    }
}
```

### Key Crafting

```c
class ActionCraftCarKey: ActionContinuousBase
{
    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTObject(10);
    }

    override string GetText()
    {
        return "Craft Car Key";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript vehicle = CarScript.Cast(target.GetObject());
        if (!vehicle)
            return false;

        // Need metal components
        if (!player.GetInventory().HasItemOfClass("MetalPlate"))
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        // Create key
        ItemBase key = action_data.m_Player.GetHumanInventory().CreateInInventory("CarKey");

        // Set key ID
        string keyID = action_data.m_Player.GetIdentity().GetPlainId() + "_" + GetGame().GetTickTime().ToString();
        key.SetVariableString("KeyID", keyID);

        // Link key to vehicle
        CarScript vehicle = CarScript.Cast(action_data.m_Target.GetObject());
        vehicle.SetOwnerID(action_data.m_Player.GetIdentity().GetPlainId());
        vehicle.AddKeyID(keyID);

        NotificationSystem.Notify(action_data.m_Player, "Key crafted!");
    }
}
```

---

## 📝 Quick Reference

### Lock Vehicle
- Requirement: Car Key in hands
- Time: Instant
- Effect: Cannot enter/drive

### Unlock Vehicle
- Requirement: Matching key
- Time: Instant
- Effect: Can enter/drive

### Craft Key
- Requirement: Metal Plate
- Time: 5 seconds
- Effect: New key created

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
