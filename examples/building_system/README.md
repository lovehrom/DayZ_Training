# Building System Example

**Status:** ✅ Working Example
**Source:** DMT_Starter_Shed (3636106243)
**Complexity:** Simple

---

## ⚡ 30-Second Summary

Mod-based building system with kit construction, instant building, and codelock support.

---

## 🎯 Features

- ✅ **Kit System** - Craft → Place → Build
- ✅ **Instant Building** - OnPlacementComplete()
- ✅ **Codelock Support** - Lockable doors
- ✅ **Health System** - Set building HP

---

## 💡 Key Concepts

### Building Action

```c
override void OnExecuteServer(ActionData action_data)
{
    PlayerBase player = action_data.m_Player;
    ItemBase kit = action_data.m_MainItem;

    // Build
    GetGame().CreateObject("MyShed", player.GetPosition());

    // Delete kit
    kit.Delete();
}
```

### Building Class

```c
class CustomBuilding extends House
{
    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);
        SetHealth("GlobalHealth", "Health", 500);
    }
}
```

---

## 🔗 Related Topics

→ [Building System](../topics/24_building_systems.md) - Mechanics
→ [Actions](../topics/03_actions.md) - Actions

---

*Последнее обновление: 2025-01-18*
