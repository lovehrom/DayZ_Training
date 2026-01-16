# Building Systems (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Building System](BUILDING_SYSTEM.md) | [Animation System](ANIMATION_SYSTEM.md)

---

## ⚡ 30-Second Summary

Кастомные building системы: multi-door строения, instant craft через kit system, codelock поддержка.

---

## 🎯 5-Minute Solution: Двухдверная постройка

### Building entity

```c
class DMT_Starter_Shed : BaseBuildingBase
{
    protected bool m_IsDoor1Opened;
    protected bool m_IsDoor2Opened;
    protected bool m_IsLocked;

    void OpenDoor1()
    {
        if (!IsLocked() && !m_IsDoor1Opened)
        {
            m_IsDoor1Opened = true;
            SetAnimationPhase("Door1", 1);  // 1 = открыта
        }
    }

    void CloseDoor1()
    {
        if (m_IsDoor1Opened)
        {
            m_IsDoor1Opened = false;
            SetAnimationPhase("Door1", 0);  // 0 = закрыта
        }
    }
}
```

### Kit system с инстант крафтом

```c
class DMT_Starter_Shed_Kit : DMT_Kit
{
    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);

        vector position = GetPosition();
        vector orientation = GetOrientation();

        DMT_Starter_Shed shed = DMT_Starter_Shed.Cast(GetGame().CreateObject("DMT_Starter_Shed", position));
        shed.SetOrientation(orientation);

        GetGame().ObjectDelete(this);  // Удалить kit
    }

    override bool CanPlayDeployLoopSound()
    {
        return false;  // Отключить construction sounds
    }
}
```

---

## 📚 Deep Dive

## DMT_Starter_Shed (3636106243) - Two-Door Building

### Двухдверная система

```c
bool IsOpened()
{
    return m_IsDoor1Opened || m_IsDoor2Opened;
}

void OpenAllDoors()
{
    if (!IsLocked())
    {
        OpenDoor1();
        OpenDoor2();
    }
}
```

### Codelock поддержка

```c
override void OnAttached(EntityAI parent, int slot)
{
    super.OnAttached(parent, slot);

    if (parent.IsInherited(CombinationLock))
    {
        m_Codelock = CombinationLock.Cast(parent);
        m_IsLocked = true;
    }
}

override bool CanOpenDoor(string door)
{
    if (IsLocked() && m_Codelock)
    {
        return m_Codelock.IsCodeEntered();
    }

    return true;
}
```

### ExpansionModBaseBuilding integration

```c
override bool IsExpansionBaseBuilding()
{
    return true;
}

override bool CanReceiveAttachment(EntityAI attachment, int slot)
{
    if (attachment.IsInherited(CombinationLock))
    {
        return true;
    }

    return super.CanReceiveAttachment(attachment, slot);
}
```

---

## 💡 Examples

### Example 1: Трёхдверный гараж

```c
class MyGarage : BaseBuildingBase
{
    protected bool m_IsDoor1Opened;
    protected bool m_IsDoor2Opened;
    protected bool m_IsDoor3Opened;

    void OpenAllDoors()
    {
        if (!IsLocked())
        {
            OpenDoor1();
            OpenDoor2();
            OpenDoor3();
        }
    }
}
```

---

## 🔗 Related Topics

→ [Building System](BUILDING_SYSTEM.md) - ConstructionActionData
→ [Animation System](ANIMATION_SYSTEM.md) - SetAnimationPhase
→ [Inventory System](INVENTORY_SYSTEM.md) - GetInventory() паттерны

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (DMT_Starter_Shed)

**Что проверено:**
- ✅ Two-door system: OpenDoor1/CloseDoor1, OpenDoor2/CloseDoor2
- ✅ IsOpened() проверка
- ✅ Kit instant craft (OnPlacementComplete)
- ✅ CanPlayDeployLoopSound() = false
- ✅ Expansion codelock поддержка

**Что НЕ проверено:**
- ❌ Полная реализация SetAnimationPhase
- ❌ Работает ли без DMT_Starter_Shed мода

---

## 💡 Tips

### Best Practices

1. **Двери:** SetAnimationPhase (0 = закрыта, 1 = открыта)
2. **Kit system:** OnPlacementComplete для инстант строительства
3. **Codelock:** OnAttached/OnDetached для интеграции
4. **Construction sounds:** CanPlayDeployLoopSound для управления звуками

---

*Последнее обновление: 2025-01-16*
*Источник: DMT_Starter_Shed (Workshop ID: 3636106243)*
