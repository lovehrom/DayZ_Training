# EntityAI Lifecycle - Жизненный цикл объектов

**✅ ПРОВЕРЕНО:** raw_data/EntityAI.c
**Источник:** Распакованный PBO (игровые файлы)

---

## EEInit()

Инициализация объекта (при создании).

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:895
override void EEInit()
{
    super.EEInit();
    // Ваш код инициализации
}
```

**Когда вызывается:**
- При создании объекта
- После spawning
- Перед тем как объект станет активным

**Использовать для:**
- Инициализации переменных
- Установки начальных значений
- Регистрации событий

---

## EEKilled()

Вызывается когда объект убит (health <= 0).

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:1072
override void EEKilled(Object killer)
{
    super.EEKilled(killer);
    // Логика смерти
}
```

**Параметры:**
- `killer` - кто убил (может быть null)

**Когда вызывается:**
- Здоровье объекта опустилось до 0
- Объект "умер"

**Использовать для:**
- Лута при убийстве зомби
- Эффектов смерти
- Очистки ресурсов

---

## EEHealthLevelChanged()

Вызывается при изменении уровня здоровья.

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:1021
override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
{
    super.EEHealthLevelChanged(oldLevel, newLevel, zone);
    // Логика изменения здоровья
}
```

**Параметры:**
- `oldLevel` - предыдущий уровень (0-3)
- `newLevel` - новый уровень (0-3)
- `zone` - зона повреждения ("GlobalHealth", "LeftHand", etc.)

**Уровни здоровья:**
- `3` - Здоров (Pristine/Healthy)
- `2` - Повреждён (Wounded/Damaged)
- `1` - Сильно повреждён (Badly Wounded)
- `0` - Критичное состояние (Critical)

---

## ⚠️ DEPRECATED: EEOnDamageCalculated()

**МЕТОД УДАЛЁН ИЗ ИГРЫ**

```c
// ❌ НЕ ИСПОЛЬЗУЙТЕ - Этот метод больше не существует!
/*
override bool EEOnDamageCalculated(TotalDamageResult damageResult, ...)
{
    // Deprecated! Не работает!
}
*/
```

**Что использовать вместо:**
- `EEHitBy()` - для обработки попаданий
- `EEHealthLevelChanged()` - для изменения здоровья
- `OnDamage()` - если существует

**Примечание:** Мод SH_Agony пользователя использует этот deprecated метод, поэтому и сломался.

---

## Template

### Базовый lifecycle предмета

```c
class MyItem extends ItemBase
{
    private bool m_IsInitialized;

    override void EEInit()
    {
        super.EEInit();
        m_IsInitialized = true;
        Print("[MyItem] Initialized");
    }

    override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
    {
        super.EEHealthLevelChanged(oldLevel, newLevel, zone);

        if (newLevel < oldLevel)
        {
            Print("[MyItem] Health decreased to " + newLevel);
        }
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        Print("[MyItem] Killed by " + killer);

        // Очистка ресурсов
        m_IsInitialized = false;
    }
}
```

### Зомби с лутом при смерти

```c
class ZombieBase extends DayZInfected
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Спавн лута
        if (GetGame().IsServer())
        {
            EntityAI item = GetInventory().CreateInInventory("Apple");
            // Дополнительная логика
        }
    }
}
```

### Эффекты при изменении здоровья

```c
class PlayerBase extends Man
{
    override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
    {
        super.EEHealthLevelChanged(oldLevel, newLevel, zone);

        if (newLevel == 0 && oldLevel > 0)
        {
            // Критическое состояние
            // Показать эффект кровотечения
        }
    }
}
```

---

## Источники

- **raw_data/EntityAI.c** (строки 895, 1021, 1072)

---

## ⚠️ Что НЕ найдено

- `EEOnDamageCalculated()` - **DELETED**, не используйте!
- Полный список всех EE* методов
- `EEHitBy()` синтаксис (требуется поиск)

---

*Последнее обновление: 2025-01-16*
*Статус: Проверено на raw_data*
