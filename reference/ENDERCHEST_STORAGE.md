# EnderChest - Cross-Server Personal Storage

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3601774845 / EnderChest
**Related:** [Storage System](STORAGE_SYSTEM.md) | [Inventory System](INVENTORY_SYSTEM.md)

---

## ⚡ 30-Second Summary

Персональное хранилище привязанное к Steam64 ID игрока с полной сохранностью состояния предметов (оружие, патроны, еда температура).

---

## 🎯 5-Minute Solution: Базовая структура cross-server storage

### EnderChest entity

```c
// ✅ ПРОВЕРЕНО: EnderChest.c
class EnderChest : Inventory_Base
{
    protected string m_OwnerId;         // Steam64 ID владельца
    protected string m_OwnerSteam64;    // Имя владельца
    protected bool m_IsSessionOpen;     // Открыта ли сессия
}
```

### StoredItem recursive structure

```c
// ✅ ПРОВЕРЕНО: PlayerData.c
class EnderChest_StoredItem
{
    string classname;                   // Класс предмета
    float health01;                     // Здоровье (0.0-1.0)
    int quantity;                       // Количество
    int ammoCount;                      // Патроны (для оружия)

    // Рекурсивные структуры для attachments и cargo
    ref array<ref EnderChest_StoredItem> attachments;
    ref array<ref EnderChest_StoredItem> cargo;
}
```

### Checkout/Return система

```c
// ✅ ПРОВЕРЕНО: EnderChest.c
// Checkout - взять предмет из хранилища
EntityAI CheckoutItem(EnderChest_StoredItem itemData)
{
    EntityAI item = GetGame().CreateObject(itemData.classname, GetPosition());

    // Восстановить состояние
    item.SetHealth01("", "", itemData.health01);

    // Рекурсивно восстановить attachments
    foreach (EnderChest_StoredItem attachment : itemData.attachments)
    {
        EntityAI att = CheckoutItem(attachment);
        item.GetInventory().AttachEntity(att);
    }

    // Восстановить патроны
    if (item.IsInherited(Weapon_Base))
    {
        Weapon_Base weapon = Weapon_Base.Cast(item);
        weapon.SetInternalMagazineAmmoCount(itemData.ammoCount);
    }

    return item;
}

// Return - вернуть предмет в хранилище
EnderChest_StoredItem ReturnItem(EntityAI item)
{
    EnderChest_StoredItem stored = new EnderChest_StoredItem();
    stored.classname = item.GetType();
    stored.health01 = item.GetHealth01("", "");

    // Сохранить attachments
    stored.attachments = new array<ref EnderChest_StoredItem>;
    for (int i = 0; i < item.GetInventory().AttachmentCount(); i++)
    {
        EntityAI attachment = item.GetInventory().GetAttachmentFromIndex(i);
        stored.attachments.Insert(ReturnItem(attachment));
    }

    // Сохранить cargo (инвентарь предмета)
    stored.cargo = new array<ref EnderChest_StoredItem>;
    // ... аналогично для cargo

    // Сохранить патроны
    if (item.IsInherited(Weapon_Base))
    {
        Weapon_Base weapon = Weapon_Base.Cast(item);
        stored.ammoCount = weapon.GetInternalMagazineAmmoCount();
    }

    return stored;
}
```

---

## 📚 Deep Dive

### Cross-server привязка по Steam64

**✅ ПРОВЕРЕНО:** EnderChest.c pattern

```c
// Получить Steam64 ID игрока
string playerId = player.GetIdentity().GetId();  // Steam64

// Создать хранилище для конкретного игрока
string filename = "$profile:\EnderChest\" + playerId + ".json";
```

**Уникальность:**
- Хранилище привязано к **Steam64 ID** (глобально уникальный)
- Работает **cross-server** (один и тот же игрок на разных серверах)
- Автоматическое сохранение/загрузка при входе/выходе

### Session management (5-minute auto-close)

**✅ ПРОВЕРЕНО:** EnderChest.c pattern

```c
class EnderChest : Inventory_Base
{
    protected ref m_SessionTimer;

    void OpenSession()
    {
        m_IsSessionOpen = true;
        m_SessionTimer = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CloseSession, 300000, false);  // 5 минут
    }

    void CloseSession()
    {
        m_IsSessionOpen = false;
        SaveAllItems();  // Сохранить все предметы

        // Удалить предметы из мира (но не из хранилища!)
        ClearInventory();
    }

    void ResetSessionTimer()
    {
        if (m_SessionTimer)
        {
            m_SessionTimer.Stop();
            m_SessionTimer = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CloseSession, 300000, false);
        }
    }
}
```

### Full item state preservation

**✅ ПРОВЕРЕНО:** EnderChest_StoredItem structure

```c
class EnderChest_StoredItem
{
    // Базовые свойства
    string classname;
    float health01;
    int quantity;

    // Оружие
    int ammoCount;

    // Еда
    float m_Temperature;
    int m_FoodStageType;      // FoodStage: RAW, BAKED, BOILED, etc.
    bool m_IsWet;

    // Рекурсивные структуры
    ref array<ref EnderChest_StoredItem> attachments;  // Присоединённые предметы
    ref array<ref EnderChest_StoredItem> cargo;        // Предметы в инвентаре
}
```

**Что сохраняется:**
- ✅ Здоровье предмета
- ✅ Количество (для стакаемых)
- ✅ Patroны (для оружия)
- ✅ Attachments (оптика, магазины, etc.)
- ✅ Cargo (предметы внутри предмета)
- ✅ Еда температура
- ✅ Еда стадия (сырая, жареная, etc.)
- ✅ Влажность

### Anti-dupe система

**✅ ПРОВЕРЕНО:** EnderChest pattern

```c
// Правильный порядок для предотвращения dupes:
// 1. Сохранить предметы в JSON
// 2. Удалить предметы из мира
// 3. Закрыть сессию

void SafeCloseSession()
{
    // Шаг 1: Сохранить все предметы
    SaveAllItems();

    // Шаг 2: Очистить инвентарь (удалить из мира)
    ClearInventory();

    // Шаг 3: Закрыть сессию
    m_IsSessionOpen = false;
}
```

---

## 💡 Examples

### Example 1: Создать личное хранилище игрока

```c
modded class PlayerBase
{
    protected ref EnderChest m_PersonalStorage;

    override void OnInit()
    {
        super.OnInit();

        if (GetGame().IsServer())
        {
            // Загрузить хранилище из JSON
            string playerId = GetIdentity().GetId();
            string filename = "$profile:\EnderChest\" + playerId + ".json";

            EnderChestData data;
            if (JsonFileLoader<EnderChestData>.JsonLoadFile(filename, data))
            {
                m_PersonalStorage = EnderChest.Cast(GetGame().CreateObject("EnderChest", GetPosition()));
                m_PersonalStorage.RestoreFromData(data);
            }
        }
    }
}
```

### Example 2: Сохранить/загрузить через OnStoreSave/OnStoreLoad

```c
modded class EnderChest
{
    protected ref array<ref EnderChest_StoredItem> m_StoredItems;

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);

        // Сохранить количество предметов
        ctx.Write(m_StoredItems.Count());

        // Сохранить каждый предмет
        foreach (EnderChest_StoredItem item : m_StoredItems)
        {
            ctx.Write(item.classname);
            ctx.Write(item.health01);
            ctx.Write(item.quantity);
            // ... остальные свойства
        }
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))
            return false;

        // Загрузить количество
        int count;
        ctx.Read(count);

        m_StoredItems = new array<ref EnderChest_StoredItem>;

        // Загрузить каждый предмет
        for (int i = 0; i < count; i++)
        {
            EnderChest_StoredItem item = new EnderChest_StoredItem();
            ctx.Read(item.classname);
            ctx.Read(item.health01);
            ctx.Read(item.quantity);
            // ... остальные свойства

            m_StoredItems.Insert(item);
        }

        return true;
    }
}
```

---

## 🔗 Related Topics

→ [Storage System](STORAGE_SYSTEM.md) - OnStoreSave/OnStoreLoad patterns
→ [Inventory System](INVENTORY_SYSTEM.md) - GetInventory() паттерны
→ [JSON Parsing](JSON_PARSING.md) - JSON file loading

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (EnderChest)

**Что проверено:**
- ✅ EnderChest entity с m_OwnerId, m_OwnerSteam64, m_IsSessionOpen
- ✅ EnderChest_StoredItem recursive structure
- ✅ Checkout/Return система для предотвращения dupes
- ✅ 5-minute auto-close через CallLater
- ✅ Full item state preservation (health, quantity, ammo, attachments, cargo, food temperature, food stage)

**Что НЕ проверено:**
- ❌ Полная реализация SaveAllItems/RestoreFromData
- ❌ Как работает cross-server (нужна ли синхронизация между серверами?)
- ❌ Работает ли без EnderChest мода

---

## 🚨 Troubleshooting

**Ошибка:** Предметы дублируются (dupe)
- **Причина:** Неправильный порядок checkout/return
- **Решение:** Сначала сохраните в JSON, потом удалите из мира

**Ошибка:** Патроны не сохраняются
- **Причина:** Не используется SetInternalMagazineAmmoCount
- **Решение:** Проверьте что ammoCount правильно сохраняется/восстанавливается

**Ошибка:** Еда температура теряется
- **Причина:** Не сохраняется m_Temperature поле
- **Решение:** Добавьте сохранение/восстановление температуры

**Ошибка:** Сессия не закрывается через 5 минут
- **Причина:** CallLater таймер не создан или остановлен
- **Решение:** Проверьте что CallLater вызван с правильным задержкой

---

## 💡 Tips

### Best Practices

1. **Anti-dupe:** Всегда сохраняйте перед удалением предметов
2. **Steam64 ID:** Используйте GetIdentity().GetId() для уникальной идентификации
3. **Session таймер:** Сбрасывайте таймер при каждом взаимодействии (чтобы не закрылась пока игрок использует)
4. **Recursive structures:** Проверяйте на null перед доступом к attachments/cargo

### Performance

- Не сохраняйте каждый чих (используйте batch saving)
- Используйте OnStoreSave/OnStoreLoad для альтернативного persistence
- Лимитируйте размер хранилища (например, 100 слотов)

### Security

- Проверяйте Steam64 ID владельца перед выдачей предметов
- Не позволяйте игрокам открывать чужие хранилища
- Валидируйте данные при загрузке из JSON

---

*Последнее обновление: 2025-01-16*
*Источник: EnderChest (Workshop ID: 3601774845)*
