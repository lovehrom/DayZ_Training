# Persistent Storage Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [JSON Parsing](JSON_PARSING.md) | [Storage System](STORAGE_SYSTEM.md)

---

## ⚡ 30-Second Summary

Системы для сохранения данных между сессиями: cross-server storage, recursive item preservation, JSON persistence.

---

## 🎯 5-Minute Solution: Cross-server personal storage

### StoredItem recursive structure

```c
class StoredItem
{
    string classname;
    float health01;
    int quantity;
    int ammoCount;

    // Рекурсивные структуры
    ref array<ref StoredItem> attachments;
    ref array<ref StoredItem> cargo;

    // Еда
    bool IsFood;
    int m_FoodStageType;
    float m_Temperature;
}
```

### Checkout/Return для предотвращения dupes

```c
EntityAI CheckoutItem(StoredItem itemData)
{
    EntityAI item = GetGame().CreateObject(itemData.classname, GetPosition());
    item.SetHealth01("", "", itemData.health01);

    // Рекурсивно восстановить attachments
    foreach (StoredItem attachment : itemData.attachments)
    {
        EntityAI att = CheckoutItem(attachment);
        item.GetInventory().AttachEntity(att);
    }

    return item;
}
```

---

## 📚 Deep Dive

## EnderChest (3601774845) - Cross-Server Personal Storage

### Привязка по Steam64 ID

```c
string playerId = player.GetIdentity().GetId();  // Steam64
string filename = "$profile:\EnderChest\\" + playerId + ".json";
```

### Session management (5-minute auto-close)

```c
void OpenSession()
{
    m_IsSessionOpen = true;
    m_SessionTimer = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CloseSession, 300000, false);
}

void CloseSession()
{
    m_IsSessionOpen = false;
    SaveAllItems();
    ClearInventory();
}
```

## CF_ModStorage (1559212036) - CF Persistent Storage

### CF_ModStorageBase

```c
class CF_ModStorageBase
{
    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
        // Custom storage logic
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))
            return false;
        // Custom load logic
        return true;
    }
}
```

---

## 💡 Examples

### Example 1: Сохранить прогресс игрока

```c
void SaveSkillProgress(PlayerBase player)
{
    string playerID = player.GetIdentity().GetId();
    string filePath = "$profile:\MyMod\\" + playerID + ".json";

    PlayerData data = new PlayerData();
    data.SurvivalEXP = GetSkillEXP("Survival");

    JsonFileLoader<PlayerData>.JsonSaveFile(filePath, data);
}
```

### Example 2: Anti-dupe checkout

```c
void SafeCheckout()
{
    // Шаг 1: Сохранить все предметы
    SaveAllItems();

    // Шаг 2: Очистить инвентарь
    ClearInventory();

    // Шаг 3: Выдать предметы игроку
    GiveItemsToPlayer();
}
```

---

## 🔗 Related Topics

→ [Storage System](STORAGE_SYSTEM.md) - OnStoreSave/OnStoreLoad patterns
→ [Inventory System](INVENTORY_SYSTEM.md) - GetInventory() паттерны
→ [JSON Parsing](JSON_PARSING.md) - JSON file loading

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ EnderChest: Steam64 ID binding, 5-minute auto-close, recursive inventory
- ✅ CF_ModStorage: CF_ModStorageBase with OnStoreSave/OnStoreLoad
- ✅ Anti-dupe patterns (checkout/return)

**Что НЕ проверено:**
- ❌ Полная реализация без модов
- ❌ Cross-server работоспособность

---

*Последнее обновление: 2025-01-16*
*Источники: EnderChest (3601774845), CF Framework (1559212036)*
