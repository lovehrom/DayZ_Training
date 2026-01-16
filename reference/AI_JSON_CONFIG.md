# AI JSON Config - DayZ AI Bandits Configuration System

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3628006769 / DayZ AI_Bandits v1.0
**Related:** [JSON Parsing](JSON_PARSING.md) | [NPC Classes](AI_NPC_CLASSES.md)

---

## ⚡ 30-Second Summary

DayZ AI Bandits использует `JsonFileLoader` для загрузки/сохранения JSON конфигурации NPC спавнов, loadout, триггеров и поведения из `$profile:\AI_Bandits\`.

---

## 🎯 5-Minute Solution: Использовать JSON конфигурацию

### Структура конфигурации

```c
// ✅ ПРОВЕРЕНО: AI_Bandits/Scripts/3_Game/StaticAIBConfig.c:1-41
class AIBStaticPlace
{
    string name;              // Название места спавна
    int npcid;               // Уникальный ID NPC
    string npcpos;           // Позиция спавна "x y z"
    string npcclass;         // Класс NPC (например "BanditAI_Base")
    ref TStringArray npcloadout;  // Loadout (предметы)
    string npcitem;          // Предмет в руках
    int npcpose;             // Pose ID (анимация)
    string triggerpos;       // Позиция триггера "x y z"
    string triggermins;      // Минимальный размер триггера "x y z"
    string triggermaxs;      // Максимальный размер триггера "x y z"
    float triggerradius;     // Радиус активации триггера
    int triggercooldown;     // Cooldown триггера (секунды)
    ref TStringArray sounds; // Звуки которые воспроизводит
    ref TStringArray emotes; // Emotes которые выполняет
}
```

### Загрузка конфигурации

```c
// ✅ ПРОВЕРЕНО: StaticAIBConfig.c:65-78
class StaticAIBanditsConfig
{
    protected const static string m_ConfigRoot = "$profile:\\AI_Bandits\\";

    static void LoadConfig(string configName, out StaticAIBConfig configData)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(configPath))
        {
            AIBLogger.Log("'" + configName + "' does not exist, creating default config");
            CreateDefaultConfig(configData);
            SaveConfig(configName, configData);
            return;
        }

        JsonFileLoader<StaticAIBConfig>.JsonLoadFile(configPath, configData);
        AIBLogger.Log("'" + configName + "' found, loading existing config");
    }
}
```

### Сохранение конфигурации

```c
// ✅ ПРОВЕРЕНО: StaticAIBConfig.c:82-93
protected static void SaveConfig(string configName, StaticAIBConfig configData)
{
    string configPath = m_ConfigRoot + configName;

    if (!FileExist(m_ConfigRoot))
    {
        AIBLogger.Log("'" + m_ConfigRoot + "' does not exist, creating directory");
        MakeDirectory(m_ConfigRoot);
    }

    JsonFileLoader<StaticAIBConfig>.JsonSaveFile(configPath, configData);
}
```

---

## 📚 Deep Dive

### StaticAIBConfig

**✅ ПРОВЕРЕНО:** StaticAIBConfig.c:44-58

```c
class StaticAIBConfig
{
    bool NPCDebug;  // Режим отладки
    int version;    // Версия конфига
    ref array< ref AIBStaticPlace > NPCLocations;  // Места спавна NPC
    ref array< ref BAIPredefinedWeapons > PredefinedWeapons;  // Оружие

    void StaticAIBConfig()
    {
        NPCLocations = new array< ref AIBStaticPlace >;
        PredefinedWeapons = new array< ref BAIPredefinedWeapons >;
    }
}
```

### JsonFileLoader API

**✅ ПРОВЕРЕНО:** StaticAIBConfig.c:77,92

```c
// Загрузка JSON файла
JsonFileLoader<StaticAIBConfig>.JsonLoadFile(string path, out StaticAIBConfig data);

// Сохранение в JSON файл
JsonFileLoader<StaticAIBConfig>.JsonSaveFile(string path, StaticAIBConfig data);
```

### $profile: путь

**✅ ПРОВЕРЕНО:** StaticAIBConfig.c:63

```c
protected const static string m_ConfigRoot = "$profile:\\AI_Bandits\\";
```

**$profile:** → Путь к профилю сервера:
- Windows: `C:\Users\Username\Documents\DayZ\`
- Linux: `~/.local/share/DayZ/`

### Структура JSON файла

**Пример JSON:**

```json
{
    "NPCDebug": false,
    "version": 1,
    "NPCLocations": [
        {
            "name": "Balota Airfield Guard",
            "npcid": 1,
            "npcpos": "5123.5 234.5 2567.8",
            "npcclass": "BanditAI_Base",
            "npcloadout": ["TShirt_Red", "Jeans_Blue", "AthleticShoes_Red", "Makarov_IJ70"],
            "npcitem": "Makarov_IJ70",
            "npcpose": 0,
            "triggerpos": "5123.5 234.5 2567.8",
            "triggermins": "50 10 50",
            "triggermaxs": "50 10 50",
            "triggerradius": 100.0,
            "triggercooldown": 300,
            "sounds": ["idle_whistle_1"],
            "emotes": ["Wave"]
        }
    ],
    "PredefinedWeapons": []
}
```

---

## 💡 Examples

### Example 1: Создать custom config систему

```c
class MyModConfig
{
    protected const static string m_ConfigRoot = "$profile:\\MyMod\\";

    string m_ConfigName;
    int m_ConfigVersion;
    ref array<string> m_SpawnPoints = new array<string>();

    static void LoadConfig(string configName, out MyModConfig config)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(configPath))
        {
            CreateDefaultConfig(config);
            SaveConfig(configName, config);
            return;
        }

        JsonFileLoader<MyModConfig>.JsonLoadFile(configPath, config);
    }

    static void SaveConfig(string configName, MyModConfig config)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(m_ConfigRoot))
        {
            MakeDirectory(m_ConfigRoot);
        }

        JsonFileLoader<MyModConfig>.JsonSaveFile(configPath, config);
    }

    protected static void CreateDefaultConfig(out MyModConfig config)
    {
        config = new MyModConfig();
        config.m_ConfigName = "MyMod";
        config.m_ConfigVersion = 1;
        config.m_SpawnPoints.Insert("5000 0 5000");
    }
}
```

### Example 2: Использовать JSON config

```c
modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();

        // Загрузить конфиг
        MyModConfig config;
        MyModConfig.LoadConfig("MyConfig.json", config);

        // Использовать конфиг
        foreach (string spawnPoint : config.m_SpawnPoints)
        {
            SpawnNPC(spawnPoint);
        }
    }

    void SpawnNPC(string position)
    {
        // Spawn NPC на позиции
        vector pos = position.ToVector();
        // ... спавн логика
    }
}
```

### Example 3: Dynamic config updates

```c
class DynamicConfigManager
{
    protected ref MyModConfig m_Config;

    void LoadOrCreateConfig()
    {
        MyModConfig.LoadConfig("MyConfig.json", m_Config);
    }

    void AddSpawnPoint(string position)
    {
        if (!m_Config)
            LoadOrCreateConfig();

        m_Config.m_SpawnPoints.Insert(position);

        // Сохранить изменения
        MyModConfig.SaveConfig("MyConfig.json", m_Config);
    }

    array<string> GetSpawnPoints()
    {
        if (!m_Config)
            LoadOrCreateConfig();

        return m_Config.m_SpawnPoints;
    }
}
```

---

## 🔗 Related Topics

→ [JSON Parsing](JSON_PARSING.md) - JsonFileLoader API
→ [NPC Classes](AI_NPC_CLASSES.md) - NPC классы

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (DayZ AI Bandits)

**Что проверено:**
- ✅ StaticAIBConfig.c:1-41 - AIBStaticPlace структура
- ✅ StaticAIBConfig.c:44-58 - StaticAIBConfig класс
- ✅ StaticAIBConfig.c:65-78 - LoadConfig метод
- ✅ StaticAIBConfig.c:82-93 - SaveConfig метод
- ✅ JsonFileLoader API (из JSON_PARSING.md)

**Что НЕ проверено:**
- ❌ Работает ли без AI Bandits мода
- ❌ Performance больших JSON файлов
- ❌ Версионирование конфигов (UpgradeConfig)

---

## 🚨 Troubleshooting

**Ошибка:** "Config file does not exist"
- **Причина:** Файл не существует в $profile:
- **Решение:** Создайте default config через `CreateDefaultConfig()`

**Ошибка:** "Failed to load JSON"
- **Причина:** Неверный JSON синтаксис
- **Решение:** Проверьте JSON валидатором

**Ошибка:** "$profile: path not found"
- **Причина:** Путь не существует
- **Решение:** Используйте `MakeDirectory()` для создания папки

---

## 💡 Tips

### Best Practices

1. **Всегда проверяйте FileExist:** Проверяйте существование файла перед загрузкой
2. **Создавайте папку:** Используйте `MakeDirectory()` если папка не существует
3. **Default config:** Всегда создавайте default config если файл не найден
4. **Версионирование:** Используйте `version` поле для миграции конфигов
5. **Валидация:** Проверяйте загруженные данные перед использованием

### File I/O Performance

- Сохраняйте конфиг только при изменениях
- Не загружайте конфиг каждый кадр
- Используйте кэширование загруженных данных

---

*Последнее обновление: 2025-01-16*
*Источник: DayZ AI_Bandits v1.0 (Workshop ID: 3628006769)*
