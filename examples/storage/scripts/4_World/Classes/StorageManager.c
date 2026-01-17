class StorageManager
{
    protected string m_Steam64_ID;
    protected ref array<ref StoredItem> m_StoredItems = new array<ref StoredItem>;
    protected const int MAX_VERSION = 1;

    void StorageManager(string steam64)
    {
        m_Steam64_ID = steam64;
        Load();
    }

    void Save()
    {
        string filePath = "$profile:storage_" + m_Steam64_ID + ".json";
        JsonFileLoader<array<ref StoredItem>>::JsonSaveFile(filePath, m_StoredItems);
        Print("Saved storage for " + m_Steam64_ID + " (" + m_StoredItems.Count() + " items)");
    }

    void Load()
    {
        string filePath = "$profile:storage_" + m_Steam64_ID + ".json";

        if (FileExist(filePath))
        {
            JsonFileLoader<array<ref StoredItem>>::JsonLoadFile(filePath, m_StoredItems);
            Print("Loaded storage for " + m_Steam64_ID + " (" + m_StoredItems.Count() + " items)");
        }
        else
        {
            Print("No storage file found for " + m_Steam64_ID);
        }
    }

    StoredItem StoreItemRecursive(ItemBase item)
    {
        if (!item)
            return null;

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
                StoredItem storedAttachment = StoreItemRecursive(attachment);
                if (storedAttachment)
                {
                    stored.Attachments.Insert(storedAttachment);
                }
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
                    StoredItem storedCargo = StoreItemRecursive(cargoItem);
                    if (storedCargo)
                    {
                        stored.Cargo.Insert(storedCargo);
                    }
                }
            }
        }

        return stored;
    }

    ItemBase RestoreItem(StoredItem stored, vector position)
    {
        if (!stored)
            return null;

        // Создаем предмет
        ItemBase item = ItemBase.Cast(GetGame().CreateObjectEx(stored.ClassName, position, ECE_PLACE_ON_SURFACE));

        if (!item)
        {
            Print("Failed to create item: " + stored.ClassName);
            return null;
        }

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

    void StoreItem(ItemBase item)
    {
        if (!item)
            return;

        StoredItem stored = StoreItemRecursive(item);
        if (stored)
        {
            m_StoredItems.Insert(stored);

            // Удаляем предмет из игры
            GetGame().ObjectDelete(item);

            // Сохраняем
            Save();
        }
    }

    void StoreInventory(EntityAI source)
    {
        if (!source)
            return;

        // Сохраняем все предметы из контейнера
        CargoBase cargo = source.GetInventory().GetCargo();
        if (cargo)
        {
            for (int i = cargo.GetItemCount() - 1; i >= 0; i--)
            {
                ItemBase item = cargo.GetItem(i);
                if (item)
                {
                    StoreItem(item);
                }
            }
        }

        Save();
    }

    void RestoreInventory(EntityAI target, vector position)
    {
        if (!target)
            return;

        CargoBase cargo = target.GetInventory().GetCargo();
        if (!cargo)
            return;

        // Восстанавливаем все предметы
        foreach (StoredItem stored : m_StoredItems)
        {
            ItemBase item = RestoreItem(stored, position);
            if (item)
            {
                if (!cargo.AddEntityToCargo(item))
                {
                    Print("Failed to add item to cargo: " + stored.ClassName);
                    GetGame().ObjectDelete(item);
                }
            }
        }

        // Очищаем хранилище
        m_StoredItems.Clear();
        Save();
    }

    int GetItemCount()
    {
        return m_StoredItems.Count();
    }
}
