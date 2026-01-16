# Animation System - Анимации

**Status:** ✅ Verified from raw_data
**Sources:** EntityAI.c:3313,3322,4107 | PlayerBase.c:4450-4470 | ItemBase.c:316,371,8375
**Related:** [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) | [Hook Points](HOOK_POINTS.md)

---

## ⚡ 30-Second Summary

DayZ использует `SetAnimationPhase()` для показа/скрытия частей модели и hand animation refresh для обновления анимаций рук.

---

## 🎯 5-Minute Solution: Показать/скрыть часть модели

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:3313,3322
SetAnimationPhase(selection_name, phase);

// 0 = показать, 1 = скрыть
SetAnimationPhase("hide_muzzle_1", 1);  // Скрыть ствол
SetAnimationPhase("hide_muzzle_1", 0);  // Показать ствол
```

---

## 📚 Deep Dive

### SetAnimationPhase()

**✅ ПРОВЕРЕНО:** EntityAI.c:3313,3322

```c
// Сигнатура:
void SetAnimationPhase(
    string selection_name,  // Имя selection в модели
    float phase            // 0 = show, 1 = hide
)
```

**Примеры из raw_data:**

```c
// EntityAI.c:3313
SetAnimationPhase(selection_name, 1);  // Скывает selection

// EntityAI.c:3322
SetAnimationPhase(selection_name, 0);  // Показывает selection
```

**Обычный use case - скрытие частей модели:**

```c
modded class MyWeapon extends Weapon_Base
{
    void HideSuppressor()
    {
        SetAnimationPhase("hide_suppressor", 1);  // Скрыть глушитель
    }

    void ShowSuppressor()
    {
        SetAnimationPhase("hide_suppressor", 0);  // Показать глушитель
    }
}
```

### Hand Animation Refresh

**✅ ПРОВЕРЕНО:** PlayerBase.c:4450-4470, EntityAI.c:4107

```c
// Обновить hand animation
// ✅ ПРОВЕРЕНО: EntityAI.c:4107
string GetInventoryHandAnimation()
{
    // Возвращает имя анимации руки
}

// ✅ ПРОВЕРЕНО: PlayerBase.c:4450-4470
void RequestHandAnimationStateRefresh()
{
    // Запросить обновление hand animation
}

void RefreshHandAnimationState()
{
    // Обновить hand animation
}
```

**Пример использования:**

```c
modded class Weapon_Base
{
    override void OnAttach(Widget w, EntityAI parent)
    {
        super.OnAttach(w, parent);

        // Обновить hand animation при присоединении
        PlayerBase player = PlayerBase.Cast(parent);
        if (player)
        {
            player.RefreshHandAnimationState();
        }
    }
}
```

### Animation Override System

**✅ ПРОВЕРЕНО:** ItemBase.c:316,371,8375

```c
// ✅ ПРОВЕРЕНО: ItemBase.c:316,371
void SetActionAnimOverrides(
    string anim1,  // Animation override 1
    string anim2   // Animation override 2
)

void OverrideActionAnimation(
    string anim  // Override animation
)
```

**Пример из ItemBase.c:8375:**

```c
// CheckAnimationOverrides() - проверка overrides
void CheckAnimationOverrides()
{
    // Проверяет и применяет animation overrides
}
```

---

## 💡 Examples

### Example 1: Custom weapon с hideable частями

```c
modded class MyCustomWeapon extends Weapon_Base
{
    protected bool m_HasSuppressor;

    override void OnInit()
    {
        super.OnInit();

        // Скрывать глушитель если не надет
        if (!m_HasSuppressor)
        {
            SetAnimationPhase("hide_suppressor", 1);
        }
    }

    void AttachSuppressor()
    {
        m_HasSuppressor = true;
        SetAnimationPhase("hide_suppressor", 0);  // Показать
    }

    void DetachSuppressor()
    {
        m_HasSuppressor = false;
        SetAnimationPhase("hide_suppressor", 1);  // Скрыть
    }
}
```

### Example 2: Hand animation refresh

```c
modded class PlayerBase
{
    void ForceUpdateHandAnimation()
    {
        RequestHandAnimationStateRefresh();

        // Или напрямую
        GetWeaponManager().RefreshAnimationState();
    }

    override void OnItemInHandsChanged(EntityAI old_item, EntityAI new_item)
    {
        super.OnItemInHandsChanged(old_item, new_item);

        // Обновить анимацию при смене предмета
        RefreshHandAnimationState();
    }
}
```

---

## 🔗 Related Topics

→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - OnInit, EEKilled
→ [Hook Points](HOOK_POINTS.md) - modded class паттерны
→ [Inventory System](INVENTORY_SYSTEM.md) - GetInventory()

---

## 🚨 Troubleshooting

**Ошибка:** SetAnimationPhase не работает
- **Причина:** Неправильное имя selection
- **Решение:** Проверьте модель в Object Builder

**Ошибка:** Hand animation не обновляется
- **Причина:** Вызываете на сервере (animation client-side)
- **Решение:** Используйте `#ifndef SERVER`

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ✅ Verified from raw_data

**Что проверено:**
- ✅ SetAnimationPhase() - EntityAI.c:3313,3322
- ✅ GetInventoryHandAnimation() - EntityAI.c:4107
- ✅ RequestHandAnimationStateRefresh() - PlayerBase.c:4450-4470
- ✅ SetActionAnimOverrides() - ItemBase.c:316,371
- ✅ OverrideActionAnimation() - ItemBase.c:371

**Что НЕ проверено:**
- ❌ PlayAnimation() - не найдено в raw_data
- ❌ PlayMove() - не найдено в raw_data
- ❌ Полный список animation phases

---

*Последнее обновление: 2025-01-16*
*Источник: raw_data/EntityAI.c, PlayerBase.c, ItemBase.c*
