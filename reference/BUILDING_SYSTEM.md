# Building System - Строительство

**Status:** ✅ Verified from raw_data
**Sources:** PlayerBase.c:224,2010-2019,5239,5917,5925
**Related:** [Hook Points](HOOK_POINTS.md) | [Actions](../topics/03_actions.md)

---

## ⚡ 30-Second Summary

DayZ использует `ConstructionActionData` для строительных действий и `IsIgnoredByConstruction()` для исключения объектов из системы строительства.

---

## 🎯 5-Minute Solution: Использовать Construction Action Data

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:2010-2012
ConstructionActionData constructionData = player.GetConstructionActionData();

if (constructionData)
{
    // Данные о строительном действии
}
```

---

## 📚 Deep Dive

### ConstructionActionData

**✅ ПРОВЕРЕНО:** PlayerBase.c:224,5917,5925

```c
// Поле в PlayerBase.c:224
ref ConstructionActionData m_ConstructionActionData;

// Getter: PlayerBase.c:2010-2012
ConstructionActionData GetConstructionActionData()
{
    return m_ConstructionActionData;
}

// Создание: PlayerBase.c:5917,5925
m_ConstructionActionData = new ConstructionActionData();
```

**Использование:**

```c
modded class PlayerBase
{
    override void OnConstructionStart(ActionData action)
    {
        super.OnConstructionStart(action);

        // Создать construction action data
        if (!m_ConstructionActionData)
        {
            m_ConstructionActionData = new ConstructionActionData();
        }

        // Настроить данные
        ConstructionActionData data = GetConstructionActionData();
        if (data)
        {
            // Настройка параметров строительства
        }
    }
}
```

### IsIgnoredByConstruction()

**✅ ПРОВЕРЕНО:** PlayerBase.c:5239 | EntityAI.c:739 | ItemBase.c:2619

```c
// Переопределить чтобы исключить entity из construction системы
override bool IsIgnoredByConstruction()
{
    return true;  // Игнорировать этот объект
}
```

**Пример из raw_data:**

```c
// PlayerBase.c:5239
override bool IsIgnoredByConstruction()
{
    // Игроки игнорируются системой строительства
    return true;
}

// EntityAI.c:739 (базовая реализация)
bool IsIgnoredByConstruction()
{
    return false;  // По умолчанию не игнорируется
}

// ItemBase.c:2619
override bool IsIgnoredByConstruction()
{
    // Предметы могут быть проигнорированы
    return false;
}
```

---

## 💡 Examples

### Example 1: Custom construction action

```c
modded class PlayerBase
{
    override void OnConstructionComplete(ActionData action)
    {
        super.OnConstructionComplete(action);

        ConstructionActionData data = GetConstructionActionData();
        if (data)
        {
            Print("Construction completed!");
            // Обработать завершение строительства
        }

        // Очистить данные
        ResetConstructionActionData();
    }

    void ResetConstructionActionData()
    {
        // ✅ ПРОВЕРЕНО: PlayerBase.c:2015-2019
        if (m_ConstructionActionData)
        {
            m_ConstructionActionData = NULL;
        }
    }
}
```

### Example 2: Исключить объект из строительства

```c
modded class MySpecialBuilding extends Building
{
    override bool IsIgnoredByConstruction()
    {
        // Этот building игнорируется системой строительства
        return true;
    }
}
```

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ✅ Verified from raw_data

**Что проверено:**
- ✅ ConstructionActionData поле - PlayerBase.c:224
- ✅ GetConstructionActionData() - PlayerBase.c:2010-2012
- ✅ IsIgnoredByConstruction() - PlayerBase.c:5239, EntityAI.c:739, ItemBase.c:2619

**Что НЕ проверено:**
- ❌ Полный API ConstructionActionData класса
- ❌ Все строительные действия
- ❌ BaseBuilding система (требует отдельного изучения)

---

*Последнее обновление: 2025-01-16*
*Источник: raw_data/PlayerBase.c*
