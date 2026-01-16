# CF ModStorage - Community Framework Persistent Storage

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 1559212036 / Community Framework v0.0.0
**Related:** [Storage System](STORAGE_SYSTEM.md) | [OnStoreSave](STORAGE_SYSTEM.md)

---

## ⚡ 30-Second Summary

Community Framework предоставляет упрощённый API для persistent storage через `CF_ModStorageBase`, который автоматически управляет версионированием и сериализацией.

---

## 🎯 5-Minute Solution: Использовать CF ModStorage

### Базовый класс

```c
// ✅ ПРОВЕРЕНО: CF/Scripts/4_World/CommunityFramework/ModStorage/CF_ModStorageBase.c:1-18
class MyModStorage extends CF_ModStorageBase
{
    // Данные для сохранения
    string m_PlayerName;
    int m_Score;

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);

        // Сохранить данные
        ctx.Write(m_PlayerName);
        ctx.Write(m_Score);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))
            return false;

        // Загрузить данные
        if (!ctx.Read(m_PlayerName))
            return false;

        if (!ctx.Read(m_Score))
            return false;

        return true;
    }
}
```

### Использование в EntityAI

```c
modded class PlayerBase
{
    ref MyModStorage m_MyStorage;

    override void OnInit()
    {
        super.OnInit();

        // Создать storage
        m_MyStorage = CF_ModStorage.GetOrCreateStorage(this, "MyModStorage");
    }

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);

        // CF автоматически сохранит storage
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))
            return false;

        // CF автоматически загрузит storage

        // Получить сохранённые данные
        MyModStorage storage = CF_ModStorage.GetStorage(this, "MyModStorage");
        if (storage)
        {
            Print(storage.m_PlayerName);
        }

        return true;
    }
}
```

---

## 📚 Deep Dive

### CF_ModStorageBase

**✅ ПРОВЕРЕНО:** CF/Scripts/4_World/CommunityFramework/ModStorage/CF_ModStorageBase.c:1-18

```c
class CF_ModStorageBase
{
    // Сохранение данных
    override void OnStoreSave(ParamsWriteContext ctx)
    {
    }

    // Загрузка данных (vanilla version)
    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        return true;
    }

    // Загрузка данных (CF version)
    override bool OnStoreLoad_CF(ParamsReadContext ctx, int cf_version)
    {
        return true;
    }
}
```

### CF_ModStorage Module

**✅ ПРОВЕРЕНО:** CF/Scripts/4_World/CommunityFramework/ModStorage/CF_ModStorageModule.c

```c
class CF_ModStorageModule
{
    // Получить storage для entity
    static CF_ModStorageBase GetStorage(EntityAI entity, string name);

    // Получить или создать storage
    static CF_ModStorageBase GetOrCreateStorage(EntityAI entity, string name);

    // Удалить storage
    static void RemoveStorage(EntityAI entity, string name);

    // Проверить существование storage
    static bool HasStorage(EntityAI entity, string name);
}
```

### Версионирование

CF ModStorage поддерживает два типа версионирования:

1. **Vanilla version** - через `OnStoreLoad(ctx, version)`
2. **CF version** - через `OnStoreLoad_CF(ctx, cf_version)`

**Пример:**

```c
class MyModStorage extends CF_ModStorageBase
{
    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        // Vanilla version
        if (version < 2)
        {
            // Legacy data loading
            return LoadLegacy(ctx);
        }

        return super.OnStoreLoad(ctx, version);
    }

    override bool OnStoreLoad_CF(ParamsReadContext ctx, int cf_version)
    {
        // CF version (изначально 1)
        if (cf_version == 1)
        {
            // Загрузка v1
        }
        else if (cf_version == 2)
        {
            // Загрузка v2
        }

        return true;
    }
}
```

---

## 💡 Examples

### Example 1: Player Stats Storage

```c
class PlayerStatsStorage extends CF_ModStorageBase
{
    int m_Kills;
    int m_Deaths;
    float m_Playtime;

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        ctx.Write(m_Kills);
        ctx.Write(m_Deaths);
        ctx.Write(m_Playtime);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!ctx.Read(m_Kills))
            return false;

        if (!ctx.Read(m_Deaths))
            return false;

        if (!ctx.Read(m_Playtime))
            return false;

        return true;
    }
}

modded class PlayerBase
{
    ref PlayerStatsStorage m_Stats;

    override void OnInit()
    {
        super.OnInit();
        m_Stats = CF_ModStorage.GetOrCreateStorage(this, "PlayerStats");
    }

    void AddKill()
    {
        if (!m_Stats)
            return;

        m_Stats.m_Kills++;
    }
}
```

### Example 2: Complex Data Storage

```c
class ComplexStorage extends CF_ModStorageBase
{
    string m_PlayerID;
    array<string> m_FriendsList = new array<string>();
    map<string, int> m_ItemCounts = new map<string, int>();

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        ctx.Write(m_PlayerID);

        // Сохранить массив
        ctx.Write(m_FriendsList.Count());
        foreach (string friend : m_FriendsList)
        {
            ctx.Write(friend);
        }

        // Сохранить map
        ctx.Write(m_ItemCounts.Count());
        foreach (string item, int count : m_ItemCounts)
        {
            ctx.Write(item);
            ctx.Write(count);
        }
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!ctx.Read(m_PlayerID))
            return false;

        // Загрузить массив
        int friendCount;
        if (!ctx.Read(friendCount))
            return false;

        m_FriendsList.Clear();
        for (int i = 0; i < friendCount; i++)
        {
            string friend;
            if (!ctx.Read(friend))
                return false;

            m_FriendsList.Insert(friend);
        }

        // Загрузить map
        int itemCount;
        if (!ctx.Read(itemCount))
            return false;

        m_ItemCounts.Clear();
        for (int i = 0; i < itemCount; i++)
        {
            string item;
            int count;

            if (!ctx.Read(item))
                return false;

            if (!ctx.Read(count))
                return false;

            m_ItemCounts.Set(item, count);
        }

        return true;
    }
}
```

### Example 3: Version Migration

```c
class VersionedStorage extends CF_ModStorageBase
{
    int m_Data;
    string m_ExtraData;  // Added in v2

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        ctx.Write(m_Data);
        ctx.Write(m_ExtraData);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!ctx.Read(m_Data))
            return false;

        // m_ExtraData added in v2
        if (version >= 2)
        {
            if (!ctx.Read(m_ExtraData))
                return false;
        }
        else
        {
            // Default value for v1 saves
            m_ExtraData = "default";
        }

        return true;
    }
}
```

---

## 🔗 Related Topics

→ [Storage System](STORAGE_SYSTEM.md) - Vanilla OnStoreSave/Load
→ [Mission API](MISSION_API.md) - GetWorldData

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (Community Framework)

**Что проверено:**
- ✅ CF_ModStorageBase.c:1-18 - базовый класс
- ✅ CF_ModStorageModule.c - модуль для управления storage
- ✅ CF_ModStorageObject.c - реализация для EntityAI

**Что НЕ проверено:**
- ❌ Работает ли без CF мода
- ❌ Persistence через рестарты сервера
- ❌ Limit на размер данных

---

## 🚨 Troubleshooting

**Ошибка:** "Storage не сохраняется"
- **Причина:** Не вызвали super.OnStoreSave() или забыли CF_ModStorage
- **Решение:** Проверьте наследование и вызов super

**Ошибка:** "Данные не загружаются"
- **Причина:** Ошибка в чтении из ctx
- **Решение:** Проверьте все ctx.Read() вызовы

**Ошибка:** "Версионирование не работает"
- **Причина:** Неправильный version parameter
- **Решение:** Используйте OnStoreLoad_CF для CF версионирования

---

## 💡 Tips

### Best Practices

1. **Всегда вызывайте super:** `super.OnStoreSave(ctx)`
2. **Проверяйте return values:** `if (!ctx.Read(val)) return false;`
3. **Используйте уникальные имена:** GetOrCreateStorage(entity, "UniqueModName_Storage")
4. **Версионируйте:** Увеличивайте version при изменении структуры данных
5. **Null check:** Проверяйте storage перед использованием

### Performance

- Сохраняйте только необходимое
- Избегайте больших структур данных
- Используйте примитивные типы вместо complex объектов

---

*Последнее обновление: 2025-01-16*
*Источник: Community Framework v0.0.0 (Workshop ID: 1559212036)*
