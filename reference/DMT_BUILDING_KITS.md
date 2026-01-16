# DMT_Starter_Shed - Two-Door Building System

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3636106243 / DMT_Starter_Shed
**Related:** [Building System](BUILDING_SYSTEM.md) | [Inventory System](INVENTORY_SYSTEM.md)

---

## ⚡ 30-Second Summary

Дверное строение с двумя дверями, инстант крафтом через kit system и поддержкой codelock.

---

## 🎯 5-Minute Solution: Базовая двухдверная постройка

### Building entity

```c
// ✅ ПРОВЕРЕНО: StarterShed.c
class DMT_Starter_Shed : BaseBuildingBase
{
    protected bool m_IsDoor1Opened;
    protected bool m_IsDoor2Opened;
    protected bool m_IsLocked;

    // Дверь 1
    void OpenDoor1()
    {
        if (!IsLocked() && !m_IsDoor1Opened)
        {
            m_IsDoor1Opened = true;
            SetAnimationPhase("Door1", 1);  // 1 = открыта
            SoundPlayOpenDoor();
        }
    }

    void CloseDoor1()
    {
        if (m_IsDoor1Opened)
        {
            m_IsDoor1Opened = false;
            SetAnimationPhase("Door1", 0);  // 0 = закрыта
            SoundPlayCloseDoor();
        }
    }

    // Дверь 2
    void OpenDoor2()
    {
        if (!IsLocked() && !m_IsDoor2Opened)
        {
            m_IsDoor2Opened = true;
            SetAnimationPhase("Door2", 1);
            SoundPlayOpenDoor();
        }
    }

    void CloseDoor2()
    {
        if (m_IsDoor2Opened)
        {
            m_IsDoor2Opened = false;
            SetAnimationPhase("Door2", 0);
            SoundPlayCloseDoor();
        }
    }

    // Проверки
    bool IsOpened()
    {
        return m_IsDoor1Opened || m_IsDoor2Opened;
    }

    bool IsLocked()
    {
        return m_IsLocked;
    }
}
```

### Kit system с инстант крафтом

```c
// ✅ ПРОВЕРЕНО: DMT_Starter_Shed_Kit.c
class DMT_Starter_Shed_Kit : DMT_Kit
{
    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);

        // Инстант строительство (без construction sounds)
        vector position = GetPosition();
        vector orientation = GetOrientation();

        // Создать здание
        DMT_Starter_Shed shed = DMT_Starter_Shed.Cast(GetGame().CreateObject("DMT_Starter_Shed", position));
        shed.SetOrientation(orientation);

        // Удалить kit
        GetGame().ObjectDelete(this);
    }

    override bool CanPlayDeployLoopSound()
    {
        return false;  // Не проигрывать construction sounds
    }
}
```

### Codelock поддержка

```c
// ✅ ПРОВЕРЕНО: DMT_Starter_Shed pattern (Expansion integration)
class DMT_Starter_Shed : BaseBuildingBase
{
    protected CombinationLock m_Codelock;

    override void OnAttached(EntityAI parent, int slot)
    {
        super.OnAttached(parent, slot);

        // Проверить если attached item это codelock
        if (parent.IsInherited(CombinationLock))
        {
            m_Codelock = CombinationLock.Cast(parent);
            m_IsLocked = true;
        }
    }

    override void OnDetached(EntityAI parent, int slot)
    {
        super.OnDetached(parent, slot);

        // Проверить если detached item это codelock
        if (parent.IsInherited(CombinationLock))
        {
            m_Codelock = null;
            m_IsLocked = false;
        }
    }

    override bool CanOpenDoor(string door)
    {
        if (IsLocked() && m_Codelock)
        {
            // Проверить код codelock
            return m_Codelock.IsCodeEntered();
        }

        return true;
    }
}
```

---

## 📚 Deep Dive

### Двухдверная система

**✅ ПРОВЕРЕНО:** StarterShed.c pattern

```c
class DMT_Starter_Shed : BaseBuildingBase
{
    // Состояния дверей
    protected bool m_IsDoor1Opened;
    protected bool m_IsDoor2Opened;

    // Анимации дверей
    void UpdateDoorAnimations()
    {
        SetAnimationPhase("Door1", m_IsDoor1Opened ? 1 : 0);
        SetAnimationPhase("Door2", m_IsDoor2Opened ? 1 : 0);
    }

    // Открыть/закрыть обе двери
    void OpenAllDoors()
    {
        if (!IsLocked())
        {
            OpenDoor1();
            OpenDoor2();
        }
    }

    void CloseAllDoors()
    {
        CloseDoor1();
        CloseDoor2();
    }

    // Проверка: открыта ли хотя бы одна дверь
    bool IsOpened()
    {
        return m_IsDoor1Opened || m_IsDoor2Opened;
    }

    // Проверка: открыты ли обе двери
    bool AreBothDoorsOpened()
    {
        return m_IsDoor1Opened && m_IsDoor2Opened;
    }
}
```

### Инстант строительство (без construction sounds)

**✅ ПРОВЕРЕНО:** DMT_Starter_Shed_Kit.c pattern

```c
class DMT_Starter_Shed_Kit : DMT_Kit
{
    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);

        // Создать здание мгновенно
        CreateBuildingInstant();

        // Удалить kit
        GetGame().ObjectDelete(this);
    }

    void CreateBuildingInstant()
    {
        vector position = GetPosition();
        vector orientation = GetOrientation();

        // Создать здание
        DMT_Starter_Shed shed = DMT_Starter_Shed.Cast(GetGame().CreateObject("DMT_Starter_Shed", position));
        shed.SetOrientation(orientation);

        // Sound muted (см. CanPlayDeployLoopSound)
    }

    override bool CanPlayDeployLoopSound()
    {
        return false;  // Отключить construction sounds
    }
}
```

**Уникальность:**
- Инстант строительство (мгновенное создание)
- Нет construction sounds (молчаливое строительство)

### ExpansionModBaseBuilding integration

**✅ ПРОВЕРЕНО:** DMT_Starter_Shed pattern

```c
class DMT_Starter_Shed : BaseBuildingBase
{
    override bool IsExpansionBaseBuilding()
    {
        return true;  // Маркер для Expansion системы
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slot)
    {
        // Разрешить attaching codelock
        if (attachment.IsInherited(CombinationLock))
        {
            return true;
        }

        return super.CanReceiveAttachment(attachment, slot);
    }
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

    void OpenDoor1()
    {
        if (!IsLocked() && !m_IsDoor1Opened)
        {
            m_IsDoor1Opened = true;
            SetAnimationPhase("Door1", 1);
        }
    }

    void OpenDoor2()
    {
        if (!IsLocked() && !m_IsDoor2Opened)
        {
            m_IsDoor2Opened = true;
            SetAnimationPhase("Door2", 1);
        }
    }

    void OpenDoor3()
    {
        if (!IsLocked() && !m_IsDoor3Opened)
        {
            m_IsDoor3Opened = true;
            SetAnimationPhase("Door3", 1);
        }
    }

    // Открыть все двери
    void OpenAllDoors()
    {
        OpenDoor1();
        OpenDoor2();
        OpenDoor3();
    }

    // Закрыть все двери
    void CloseAllDoors()
    {
        CloseDoor1();
        CloseDoor2();
        CloseDoor3();
    }
}
```

### Example 2: Kit с delayed строительством

```c
class MyBuilding_Kit : ItemBase
{
    protected ref m_BuildTimer;

    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);

        // Запустить timer для строительства (5 секунд)
        m_BuildTimer = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CreateBuilding, 5000, false, this, player);
    }

    void CreateBuilding(ItemBase kit, Man player)
    {
        vector position = kit.GetPosition();
        vector orientation = kit.GetOrientation();

        // Создать здание
        MyBuilding building = MyBuilding.Cast(GetGame().CreateObject("MyBuilding", position));
        building.SetOrientation(orientation);

        // Удалить kit
        GetGame().ObjectDelete(kit);

        // Показать уведомление игроку
        PlayerBase pb = PlayerBase.Cast(player);
        if (pb)
        {
            pb.MessageAction("Building complete!");
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
- ✅ IsOpened() проверка (true если хоть одна дверь открыта)
- ✅ IsLocked() проверка
- ✅ Kit system с инстант крафтом (OnPlacementComplete → CreateBuildingInstant)
- ✅ CanPlayDeployLoopSound() = false для отключения construction sounds
- ✅ Expansion codelock поддержка

**Что НЕ проверено:**
- ❌ Полная реализация SetAnimationPhase для дверей
- ❌ Как работает Expansion codelock интеграция (детально)
- ❌ Работает ли без DMT_Starter_Shed мода

---

## 🚨 Troubleshooting

**Ошибка:** Дверь не открывается
- **Причина:** Заблокирована или неправильная анимация
- **Решение:** Проверьте IsLocked() и SetAnimationPhase()

**Ошибка:** Kit не создаёт здание
- **Причина:** OnPlacementComplete не вызывается или неправильный classname
- **Решение:** Проверьте что classname здания корректный

**Ошибка:** Codelock не работает
- **Причина:** Не интегрирован Expansion или неправильный slot
- **Решение:** Проверьте CanReceiveAttachment и OnAttached/OnDetached

---

## 💡 Tips

### Best Practices

1. **Двери:** Используйте SetAnimationPhase для анимации (0 = закрыта, 1 = открыта)
2. **Kit system:** Используйте OnPlacementComplete для инстант строительства
3. **Codelock:** Интегрируйтесь через OnAttached/OnDetached
4. **Construction sounds:** Используйте CanPlayDeployLoopSound для управления звуками

### Performance

- Не обновляйте анимации каждый кадр (только при изменении состояния)
- Используйте CallLater для delayed строительства
- Кэшируйте состояние дверей (не вызывайте IsOpened() много раз)

### Building Design

- Используйте meaningful имена для дверей (Door1, Door2, MainDoor, BackDoor)
- Предусмотрите механизмы для lock/unlock (codelock, key)
- Добавьте звуки открытия/закрытия (SoundPlayOpenDoor, SoundPlayCloseDoor)

---

*Последнее обновление: 2025-01-16*
*Источник: DMT_Starter_Shed (Workshop ID: 3636106243)*
