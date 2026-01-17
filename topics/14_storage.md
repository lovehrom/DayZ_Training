# Persistent Storage Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Advanced Modding](11_advanced_modding.md) | [JSON Parsing](../reference/JSON_PARSING.md)

---

## ⚡ 30-Second Summary

Cross-server storage: хранение данных между серверами, recursive сохранение предметов, Steam64 ID-based storage. Полная документация: [PERSISTENT_STORAGE.md](../reference/PERSISTENT_STORAGE.md)

---

## 🎯 5-Minute Solution: Personal Storage

### Step 1: Создайте класс хранимого предмета

```c
class StoredItem
{
    string ClassName;
    float Health;
    float Quantity;

    // Для вложенных предметов (attachments, cargo)
    ref array<ref StoredItem> Attachments = new array<ref StoredItem>;
    ref array<ref StoredItem> Cargo = new array<ref StoredItem>;
}
```

### Step 2: Создайте класс хранилища

```c
class PlayerStorage
{
    string Steam64_ID;
    ref array<ref StoredItem> Items = new array<ref StoredItem>;

    void PlayerStorage(string steam64)
    {
        Steam64_ID = steam64;
    }
}
```

### Step 3: Создайте менеджер хранилища

```c
class StorageManager
{
    protected ref map<string, ref PlayerStorage> m_Storages = new map<string, ref PlayerStorage>;

    void LoadStorage(string steam64)
    {
        string filePath = "$profile:storage_" + steam64 + ".json";

        if (FileExist(filePath))
        {
            PlayerStorage storage = new PlayerStorage(steam64);
            JsonFileLoader<PlayerStorage>.JsonLoadFile(filePath, storage);
            m_Storages.Insert(steam64, storage);
        }
        else
        {
            // Создаем новое хранилище
            PlayerStorage storage = new PlayerStorage(steam64);
            m_Storages.Insert(steam64, storage);
        }
    }

    void SaveStorage(string steam64)
    {
        PlayerStorage storage = m_Storages.Get(steam64);

        if (storage)
        {
            string filePath = "$profile:storage_" + steam64 + ".json";
            JsonFileLoader<PlayerStorage>.JsonSaveFile(filePath, storage);
        }
    }

    void StoreItem(string steam64, ItemBase item)
    {
        PlayerStorage storage = m_Storages.Get(steam64);

        if (!storage)
            return;

        // Рекурсивно сохраняем предмет
        StoredItem stored = StoreItemRecursive(item);
        storage.Items.Insert(stored);

        // Удаляем предмет из игры
        GetGame().ObjectDelete(item);
    }

    StoredItem StoreItemRecursive(ItemBase item)
    {
        StoredItem stored = new StoredItem;
        stored.ClassName = item.GetType();
        stored.Health = item.GetHealth();
        stored.Quantity = item.GetQuantity();

        // Сохраняем attachments
        for (int i = 0; i < item.GetInventory().AttachmentCount(); i++)
        {
            ItemBase attachment = ItemBase.Cast(item.GetInventory().GetAttachmentFromIndex(i));
            if (attachment)
            {
                stored.Attachments.Insert(StoreItemRecursive(attachment));
            }
        }

        // Сохраняем cargo (вложенные предметы)
        CargoBase cargo = item.GetInventory().GetCargo();
        if (cargo)
        {
            for (int j = 0; j < cargo.GetItemCount(); j++)
            {
                ItemBase cargoItem = cargo.GetItem(j);
                if (cargoItem)
                {
                    stored.Cargo.Insert(StoreItemRecursive(cargoItem));
                }
            }
        }

        return stored;
    }

    ItemBase RestoreItem(StoredItem stored, vector position)
    {
        // Создаем предмет
        ItemBase item = ItemBase.Cast(GetGame().CreateObject(stored.ClassName, position));

        if (!item)
            return null;

        // Восстанавливаем состояние
        item.SetHealth(stored.Health);
        item.SetQuantity(stored.Quantity);

        // Рекурсивно восстанавливаем attachments
        foreach (StoredItem storedAttachment : stored.Attachments)
        {
            ItemBase attachment = RestoreItem(storedAttachment, position);
            if (attachment)
            {
                item.GetInventory().AttachmentEntityAttachment(attachment);
            }
        }

        // Рекурсивно восстанавливаем cargo
        foreach (StoredItem storedCargo : stored.Cargo)
        {
            ItemBase cargoItem = RestoreItem(storedCargo, position);
            if (cargoItem)
            {
                item.GetInventory().GetCargo().AddEntityToCargo(cargoItem);
            }
        }

        return item;
    }

    array<ItemBase> RetrieveItems(string steam64, vector position)
    {
        PlayerStorage storage = m_Storages.Get(steam64);

        if (!storage)
            return null;

        array<ItemBase> items = new array<ItemBase>;

        foreach (StoredItem stored : storage.Items)
        {
            ItemBase item = RestoreItem(stored, position);
            if (item)
            {
                items.Insert(item);
            }
        }

        // Очищаем хранилище
        storage.Items.Clear();

        return items;
    }
}
```

### Step 4: Интегрируйте в PlayerBase

```c
modded class PlayerBase
{
    protected StorageManager m_StorageManager;

    override void OnConnect()
    {
        super.OnConnect();

        string steam64 = GetIdentity().GetId();
        m_StorageManager = new StorageManager();
        m_StorageManager.LoadStorage(steam64);
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        if (m_StorageManager)
        {
            string steam64 = GetIdentity().GetId();
            m_StorageManager.SaveStorage(steam64);
        }
    }
}
```

---

## 📚 Deep Dive

### Advanced: Cross-Server Storage (как в EnderChest)

**EnderChest** реализует:
- Cross-server personal storage на основе Steam64 ID
- Recursive item preservation (weapon ammo, attachments, food temperature)
- Session management с 5-minute auto-close
- Anti-dupe через SafeCheckout()

**Уникальность:**
- Хранилище доступно на ЛЮБЫХ серверах с этим модом
- Полное сохранение состояния оружия (патроны в магазине, attachments)
- Автоматическая очистка пустых crate

**Полная документация:** [PERSISTENT_STORAGE.md](../reference/PERSISTENT_STORAGE.md)

---

## 💡 Examples

### Пример 1: Простое хранилище без recursive

```c
class SimpleStorage
{
    array<string> StoredItems = new array<string>;

    void StoreItem(string className)
    {
        StoredItems.Insert(className);
    }

    void Save(string steam64)
    {
        string filePath = "$profile:storage_" + steam64 + ".json";
        JsonFileLoader<array<string>>::JsonSaveFile(filePath, StoredItems);
    }

    void Load(string steam64)
    {
        string filePath = "$profile:storage_" + steam64 + ".json";
        if (FileExist(filePath))
        {
            JsonFileLoader<array<string>>::JsonLoadFile(filePath, StoredItems);
        }
    }
}
```

### Пример 2: Хранение простых данных

```c
class PlayerData
{
    string Steam64_ID;
    int Coins;
    int Level;
    float LastOnline;

    void Save()
    {
        string filePath = "$profile:player_" + Steam64_ID + ".json";
        JsonFileLoader<PlayerData>.JsonSaveFile(filePath, this);
    }

    static PlayerData Load(string steam64)
    {
        string filePath = "$profile:player_" + steam64 + ".json";

        if (FileExist(filePath))
        {
            PlayerData data = new PlayerData();
            JsonFileLoader<PlayerData>.JsonLoadFile(filePath, data);
            return data;
        }

        return null;
    }
}
```

### Пример 3: Anti-dupe система

```c
class AntiDupeStorage
{
    protected map<string, bool> m_CheckedOutItems = new map<string, bool>;

    bool SafeCheckout(string itemId, string steam64)
    {
        // Проверяем что предмет не checked out
        if (m_CheckedOutItems.Get(itemId))
            return false;

        // Mark как checked out
        m_CheckedOutItems.Insert(itemId, true);
        return true;
    }

    void Checkin(string itemId)
    {
        m_CheckedOutItems.Remove(itemId);
    }
}
```

---

## 🔗 Related Topics

→ [JSON Parsing](../reference/JSON_PARSING.md) - Работа с JSON
→ [Advanced Modding](11_advanced_modding.md) - Обзор mechanics
→ [Inventory System](../reference/INVENTORY_SYSTEM.md) - Работа с инвентарем

---

## ⚠️ SOURCE VERIFICATION

**Status:** ⚠️ Mod-derived

**Источник:** EnderChest (3601774845)

**Что проверено:**
- ✅ Recursive сохранение работает
- ✅ Steam64 ID-based storage работает
- ✅ Cross-server доступ работает

**Что НЕ проверено:**
- ❌ Работает ли без EnderChest мода
- ❌ Производительность с большим количеством предметов
- ❌ Совместимость с другими storage модами

---

## 💡 Tips

### Best Practices:

1. **Используйте Steam64 ID** для идентификации игрока
2. **Рекурсивно сохраняйте** предметы с attachments
3. **Очищайте хранилище** после извлечения
4. **Используйте SafeCheckout** для anti-dupe
5. **Сохраняйте при дисконнекте** для безопасности

### Steam64 ID:

```c
string GetSteam64ID(PlayerBase player)
{
    if (player.GetIdentity())
    {
        return player.GetIdentity().GetId();
    }
    return "";
}
```

### Performance Tips:

- **Ограничьте количество** предметов в хранилище (< 100)
- **Не сохраняйте** бесконечные nested items (ограничьте recursion)
- **Используйте batch saving** (не сохраняйте каждый предмет)
- **Очищайте пустые** хранилища автоматически

### Common Pitfalls:

- ❌ **Забыть recursive** → потеря attachments
- ❌ **Не использовать Steam64** → conficts между игроками
- ❌ **Не очищать хранилище** → дублирование предметов
- ❌ **Сохранять слишком часто** → performance drop

---

*Последнее обновление: 2025-01-16*
*Источник: EnderChest (3601774845)*
