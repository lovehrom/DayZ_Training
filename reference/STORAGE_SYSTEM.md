# Storage System - Сохранение и загрузка данных

**Status:** ✅ Verified from raw_data
**Sources:** PlayerBase.c:6802-6968, ItemBase.c:3092-3216, EntityAI.c:2864-2942
**Related:** [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) | [PlayerBase API](PLAYERBASE_API.md)

---

## ⚡ 30-Second Summary

DayZ использует `OnStoreSave()` / `OnStoreLoad()` методы для сохранения состояния entity в базу данных. Это vanilla системаpersistent storage.

---

## 🎯 5-Minute Solution: Сохранить custom данные

### Шаблон для PlayerBase

```c
modded class PlayerBase
{
    protected bool m_MyCustomFlag;
    protected int m_MyCustomValue;

    // ✅ ПРОВЕРЕНО: PlayerBase.c:6802
    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);  // ВАЖНО!

        // Сохранить custom данные
        ctx.Write(m_MyCustomFlag);
        ctx.Write(m_MyCustomValue);
    }

    // ✅ ПРОВЕРЕНО: PlayerBase.c:6860
    override void OnStoreLoad(ParamsReadContext ctx, int version)
    {
        super.OnStoreLoad(ctx, version);  // ВАЖНО!

        // Загрузить custom данные
        ctx.Read(m_MyCustomFlag);
        ctx.Read(m_MyCustomValue);
    }
}
```

### Шаблон для ItemBase

```c
modded class MyCustomItem extends ItemBase
{
    protected float m_Durability;

    // ✅ ПРОВЕРЕНО: ItemBase.c:3193
    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);

        ctx.Write(m_Durability);
    }

    override void OnStoreLoad(ParamsReadContext ctx, int version)
    {
        super.OnStoreLoad(ctx, version);

        ctx.Read(m_Durability);
    }
}
```

---

## 📚 Deep Dive

### Структура сохранения PlayerBase

**✅ ПРОВЕРЕНО:** PlayerBase.c:6802-6968 (166 строк!)

PlayerBase сохраняет в OnStoreSave():
```c
override void OnStoreSave(ParamsWriteContext ctx)
{
    // 1. Version check
    if (GetGame().SaveVersion() < 102)
    {
        ctx.Write(ACT_STORE_SAVE_VERSION);
    }

    // 2. Stats (здоровье, голод, жажда...)
    m_PlayerStats.OnStoreSave(ctx);

    // 3. Modifiers (модификаторы состояния)
    m_Modifiers.OnStoreSave(ctx);

    // 4. Agents (агенты: инфекция, кровотечение...)
    m_Agents.OnStoreSave(ctx);

    // 5. Bleeding (кровотечение)
    ctx.Write(m_BleedingSourcesServer);

    // 6. И другие данные...
}
```

### Структура загрузки PlayerBase

**✅ ПРОВЕРЕНО:** PlayerBase.c:6860-6968

```c
override void OnStoreLoad(ParamsReadContext ctx, int version)
{
    // 1. Version check
    if (version < 102)
    {
        int savedVersion;
        ctx.Read(savedVersion);
    }

    // 2. Load stats
    m_PlayerStats.OnStoreLoad(ctx, version);

    // 3. Load modifiers
    m_Modifiers.OnStoreLoad(ctx, version);

    // 4. Load agents
    m_Agents.OnStoreLoad(ctx, version);

    // 5. Load bleeding
    ctx.Read(m_BleedingSourcesServer);

    // 6. Load other data...
}
```

### ParamsWriteContext API

**✅ ПРОВЕРЕНО:** PlayerBase.c, ItemBase.c

```c
// Запись простых типов
ctx.Write(bool value);      // 1 byte
ctx.Write(int value);       // 4 bytes
ctx.Write(float value);     // 4 bytes
ctx.Write(vector value);    // 12 bytes (3 floats)
ctx.Write(string value);    // Переменная длина

// Пример:
ctx.Write(true);           // bool
ctx.Write(100);            // int
ctx.Write(3.14);           // float
ctx.Write("1.5 2.0 3.0");  // vector (как строка!)
ctx.Write("PlayerName");   // string
```

### ParamsReadContext API

**✅ ПРОВЕРЕНО:** PlayerBase.c, ItemBase.c

```c
// Чтение простых типов
ctx.Read(out bool value);      // out параметр!
ctx.Read(out int value);
ctx.Read(out float value);
ctx.Read(out vector value);    // vector из строки
ctx.Read(out string value);

// Пример:
bool flag;
int number;
float decimal;
vector position;
string name;

ctx.Read(flag);        // out bool
ctx.Read(number);      // out int
ctx.Read(decimal);     // out float
ctx.Read(position);    // out vector
ctx.Read(name);        // out string
```

### Version handling

**✅ ПРОВЕРЕНО:** PlayerBase.c:6805-6812, ItemBase.c:3092

```c
override void OnStoreSave(ParamsWriteContext ctx)
{
    // Version check для совместимости
    if (GetGame().SaveVersion() < 102)
    {
        ctx.Write(ACT_STORE_SAVE_VERSION);  // Сохранить версию
    }

    // ... остальной код
}

override void OnStoreLoad(ParamsReadContext ctx, int version)
{
    // version = версия из сохранённого файла

    if (version < 102)
    {
        // Load старого формата
        int savedVersion;
        ctx.Read(savedVersion);
    }

    // ... остальной код
}
```

### Complex данные (массивы)

**✅ ПРОВЕРЕНО:** PlayerBase.c:6923 (m_BleedingSourcesServer - array)

```c
// Сохранение массива
ref array<bleedingSource> m_BleedingSourcesServer = new array<bleedingSource>;

override void OnStoreSave(ParamsWriteContext ctx)
{
    super.OnStoreSave(ctx);

    // Сохранить размер массива
    ctx.Write(m_BleedingSourcesServer.Count());

    // Сохранить каждый элемент
    for (int i = 0; i < m_BleedingSourcesServer.Count(); i++)
    {
        bleedingSource src = m_BleedingSourcesServer.Get(i);
        ctx.Write(src.m_BoneIdx);      // int
        ctx.Write(src.m_BleedingType); // int
    }
}

override void OnStoreLoad(ParamsReadContext ctx, int version)
{
    super.OnStoreLoad(ctx, version);

    // Прочитать размер массива
    int count;
    ctx.Read(count);

    // Очистить массив
    m_BleedingSourcesServer.Clear();

    // Прочитать каждый элемент
    for (int i = 0; i < count; i++)
    {
        bleedingSource src;
        ctx.Read(src.m_BoneIdx);
        ctx.Read(src.m_BleedingType);

        m_BleedingSourcesServer.Insert(src);
    }
}
```

---

## 💡 Examples

### Example 1: Custom Player Stats

```c
modded class PlayerBase
{
    protected float m_CustomEXP;
    protected int m_KillCount;

    override void OnInit()
    {
        super.OnInit();
        m_CustomEXP = 0;
        m_KillCount = 0;
    }

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);

        ctx.Write(m_CustomEXP);
        ctx.Write(m_KillCount);
    }

    override void OnStoreLoad(ParamsReadContext ctx, int version)
    {
        super.OnStoreLoad(ctx, version);

        ctx.Read(m_CustomEXP);
        ctx.Read(m_KillCount);

        Print("Loaded EXP: " + m_CustomEXP);
        Print("Loaded Kills: " + m_KillCount);
    }

    void AddEXP(float amount)
    {
        m_CustomEXP += amount;
        Print("EXP: " + m_CustomEXP);
    }
}
```

### Example 2: Custom Item Durability

```c
modded class MyCustomItem extends ItemBase
{
    protected float m_Durability;  // 0-100

    override void OnInit()
    {
        super.OnInit();
        m_Durability = 100.0;
    }

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
        ctx.Write(m_Durability);
    }

    override void OnStoreLoad(ParamsReadContext ctx, int version)
    {
        super.OnStoreLoad(ctx, version);
        ctx.Read(m_Durability);

        Print("Item durability: " + m_Durability);
    }

    void DamageItem(float amount)
    {
        m_Durability -= amount;

        if (m_Durability <= 0)
        {
            Delete();  // Удалить предмет
        }
    }
}
```

### Example 3: Complex Data Structure

```c
modded class PlayerBase
{
    class PlayerQuestData
    {
        string QuestID;
        int QuestStage;
        float QuestProgress;

        void Save(ParamsWriteContext ctx)
        {
            ctx.Write(QuestID);
            ctx.Write(QuestStage);
            ctx.Write(QuestProgress);
        }

        void Load(ParamsReadContext ctx)
        {
            ctx.Read(QuestID);
            ctx.Read(QuestStage);
            ctx.Read(QuestProgress);
        }
    }

    protected ref array<ref PlayerQuestData> m_Quests = new array<ref PlayerQuestData>;

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);

        ctx.Write(m_Quests.Count());

        for (int i = 0; i < m_Quests.Count(); i++)
        {
            m_Quests[i].Save(ctx);
        }
    }

    override void OnStoreLoad(ParamsReadContext ctx, int version)
    {
        super.OnStoreLoad(ctx, version);

        int count;
        ctx.Read(count);

        m_Quests.Clear();

        for (int i = 0; i < count; i++)
        {
            PlayerQuestData quest = new PlayerQuestData;
            quest.Load(ctx);
            m_Quests.Insert(quest);
        }
    }
}
```

---

## 🔗 Related Topics

→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - OnInit, EEKilled
→ [PlayerBase API](PLAYERBASE_API.md) - OnInit, OnUpdate
→ [Hook Points](HOOK_POINTS.md) - modded class паттерны

---

## 🚨 Troubleshooting

**Ошибка:** Данные не сохраняются
- **Причина:** Забыли вызвать super.OnStoreSave()
- **Решение:** ВСЕГДА вызывайте super первым делом

**Ошибка:** Данные теряются после рестарта
- **Причина:** Возможно это temporary entity (временный предмет)
- **Решение:** Проверьте что предмет добавлен в economy (types.xml)

**Ошибка:** Version mismatch
- **Причина:** Изменилась структура данных
- **Решение:** Используйте version параметр для backward compatibility

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ✅ Verified from raw_data

**Что проверено:**
- ✅ OnStoreSave() - PlayerBase.c:6802-6858, ItemBase.c:3193-3215
- ✅ OnStoreLoad() - PlayerBase.c:6860-6968, ItemBase.c:3092-3190
- ✅ ParamsWriteContext.Write(bool/int/float/vector/string)
- ✅ ParamsReadContext.Read(out bool/int/float/vector/string)
- ✅ Version handling - PlayerBase.c:6805-6812
- ✅ Array save/load - PlayerBase.c:6923

**Что НЕ проверено:**
- ❌ Полный список всех типов для Write/Read
- ❌ Максимальный размер данных
- ❌ Performance implications больших данных

---

## 🔍 Как использовать эту информацию

1. **Custom player stats:** Используйте OnStoreSave/Load в PlayerBase
2. **Custom item properties:** Используйте в ItemBase
3. **Complex data:** Сохраняйте array размер + элементы
4. **Version compatibility:** Обрабатывайте version параметр

**ВАЖНО:** Всегда вызывайте super.OnStoreSave(ctx) и super.OnStoreLoad(ctx, version) первым делом!

---

*Последнее обновление: 2025-01-16*
*Источник: raw_data/PlayerBase.c:6802-6968, ItemBase.c:3092-3216*
