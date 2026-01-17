# Building System (Mod)

**Status:** ⚠️ Mod-derived
**Related:** [Building](../reference/BUILDING_SYSTEM.md) | [Actions](03_actions.md)

---

## ⚡ 30-Second Summary

Building system: kit system, instant construction, OnPlacementComplete(), codelock поддержка, multi-door mechanics.

---

## 🎯 5-Minute Solution

### Building item

```cpp
class CfgVehicles
{
    class House;
    class MyShed: House
    {
        scope = 2;
    };
}
```

### Action

```c
modded class ActionBuildBase
{
    override void OnExecuteServer(ActionData action_data)
    {
        super.OnExecuteServer(action_data);

        PlayerBase player = action_data.m_Player;
        ItemBase kit = action_data.m_MainItem;

        // Build
        GetGame().CreateObject("MyShed", player.GetPosition());

        // Delete kit
        kit.Delete();
    }
}
```

---

## 💡 Examples

### Пример 1: Custom building

```c
class CustomBuilding extends House
{
    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);

        // Set health
        SetHealth("GlobalHealth", "Health", 500);
    }
}
```

### Пример 2: Codelock

```c
class CodelockBuilding extends CustomBuilding
{
    override bool CanUseConstruction()
    {
        if (HasCodelock())
        {
            return HasCodelockAccess();
        }
        return true;
    }
}
```

---

## 🔗 Related Topics

→ [Building (Vanilla)](../reference/BUILDING_SYSTEM.md) - Base system
→ [Actions](03_actions.md) - Actions
→ [Reference: Building Mod](../reference/BUILDING_SYSTEMS_MOD.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
*Источник: DMT_Starter_Shed (3636106243)*
