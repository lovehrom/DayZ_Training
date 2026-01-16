# AI System - NPC Bandits

**Status:** ⚠️ Mod-derived (requires verification)
**Source:** [AI Bandits](https://steamcommunity.com/sharedfiles/filedetails/?id=3628006769)
**Related:** [PlayerBase API](PLAYERBASE_API.md) | [Entity Lifecycle](ENTITYAI_LIFECYCLE.md)

---

## ⚡ 30-Second Summary

AI Bandits мод реализует NPC на основе DayZPlayer/Man (НЕ Animal!), с JSON конфигурацией для спавнов, патрульной системой по waypoint'ам и триггерной активацией.

---

## 🎯 5-Minute Solution: Создать простого NPC

### Понять архитектуру NPC

**⚠️ ВАЖНО:** NPC основаны на DayZPlayer/Man, НЕ на AnimalBase!

```
DayZPlayer              (Базовый класс игрока)
└── Man                 (Человек)
    └── DayZInfected    (Зомби)
        └── InfectedBanditBase    (Базовый класс бандита)
            └── BanditAI_Base      (AI логика)
                ├── BanditAI_Adam  (Конкретный NPC)
                ├── BanditAI_Boris
                └── ...
```

### Базовый NPC класс

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

```c
class BanditAI_Base extends DayZInfected
{
    protected ref TStringArray m_LoadoutItems;
    protected ref array<vector> m_Waypoints;
    protected int m_CurrentWaypoint;

    override void OnInit()
    {
        super.OnInit();

        // Настройка AI
        m_LoadoutItems = {"HuntingJacket_Brown", "Jeans_Blue", "AthleticShoes_Black"};
        m_Waypoints = {};

        // Отключить стандартные звуки зомби
        // (настройки...)
    }

    override void OnLoadFinished()
    {
        super.OnLoadFinished();

        // Экипировать loadout
        foreach (string itemName : m_LoadoutItems)
        {
            EntityAI item = GetInventory().CreateInInventory(itemName);
        }
    }

    void AddWaypoint(vector position)
    {
        m_Waypoints.Insert(position);
    }

    void ProcessPatrol()
    {
        if (m_Waypoints.Count() == 0)
            return;

        vector targetPos = m_Waypoints[m_CurrentWaypoint];

        // Движение к waypoint
        // (AI логика...)

        // Проверка достижения
        if (vector.Distance(GetPosition(), targetPos) < 2.0)
        {
            m_CurrentWaypoint = (m_CurrentWaypoint + 1) % m_Waypoints.Count();
        }
    }
};

class BanditAI_Adam extends BanditAI_Base
{
    override void OnInit()
    {
        super.OnInit();

        // Уникальные настройки для Adam
        m_LoadoutItems = {"M65Jacket_Black", "CargoPants_Black", "CombatBoots_Black"};
    }
}
```

---

## 📚 Deep Dive

### JSON конфигурация спавнов

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

**StaticAIB.json:**
```json
{
    "version": 1,
    "staticaib": [
        {
            "name": "KamenkaTrader",
            "npcpos": [1155.5, 6.5, 2354.3],
            "npcclass": "BanditAI_Adam",
            "npcloadout": [
                "M65Jacket_Black",
                "CargoPants_Black",
                "CombatBoots_Black",
                "AKM",
                "Mag_AKM_30Rnd",
                "Mag_AKM_30Rnd"
            ],
            "triggerradius": 50.0,
            "waypoints": [
                [1155.5, 6.5, 2354.3],
                [1160.0, 6.5, 2360.0],
                [1165.0, 6.5, 2355.0]
            ],
            "weapon": "AKM",
            "magazine": "Mag_AKM_30Rnd",
            "ammo": 30
        }
    ]
}
```

**DynamicAIB.json:**
```json
{
    "version": 1,
    "dynamicaib": [
        {
            "name": "BanditPatrol",
            "min": 2,
            "max": 4,
            "npcclass": "BanditAI_Adam",
            "spawnradius": 5000.0,
            "center": [5000.0, 0.0, 5000.0],
            "respawntime": 300
        }
    ]
}
```

### Trigger система

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

```c
class AIBTalkTrigger extends Trigger
{
    protected ref array<Object> m_PlayerList;

    override void OnEnter(Object obj)
    {
        PlayerBase player = PlayerBase.Cast(obj);
        if (player)
        {
            // Активировать AI при входе игрока
            ActivateNearbyBandits(player.GetPosition());
        }
    }

    override void OnLeave(Object obj)
    {
        PlayerBase player = PlayerBase.Cast(obj);
        if (player)
        {
            // Деактивировать AI при выходе
            DeactivateNearbyBandits(player.GetPosition());
        }
    }

    void ActivateNearbyBandits(vector center)
    {
        // Найти всех BanditAI_Base в радиусе
        // Активировать их AI
    }
}
```

### Loadout система

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

```c
class BanditAI_Base extends DayZInfected
{
    void EquipLoadout(array<string> loadoutItems)
    {
        foreach (string itemName : loadoutItems)
        {
            // Создать предмет
            EntityAI item = GetInventory().CreateInInventory(itemName);

            // Если это оружие
            if (item.IsInherited(Weapon_Base))
            {
                // Создать магазин
                EntityAI mag = GetInventory().CreateInInventory("Mag_AKM_30Rnd");
                // Загрузить патроны
                Magazine magCast = Magazine.Cast(mag);
                if (magCast)
                {
                    magCast.ServerSetAmmoCount(30);
                }
            }
        }
    }
}
```

### Waypoint патрулирование

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

```c
class BanditAI_Base extends DayZInfected
{
    protected ref array<vector> m_Waypoints;
    protected int m_CurrentWaypoint;
    protected bool m_IsPatrolling;

    void StartPatrol()
    {
        if (m_Waypoints.Count() > 0)
        {
            m_CurrentWaypoint = 0;
            m_IsPatrolling = true;
            ProcessPatrol();
        }
    }

    void ProcessPatrol()
    {
        if (!m_IsPatrolling)
            return;

        vector targetPos = m_Waypoints[m_CurrentWaypoint];

        // Использовать DayZInfected команды движения
        // StartCommand_Move(targetPos);

        // Проверка достижения waypoint
        // (обычно через OnUpdate или timer)
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        if (m_IsPatrolling)
        {
            vector targetPos = m_Waypoints[m_CurrentWaypoint];
            float dist = vector.Distance(GetPosition(), targetPos);

            if (dist < 2.0)
            {
                // Waypoint достигнут
                m_CurrentWaypoint = (m_CurrentWaypoint + 1) % m_Waypoints.Count();
            }
        }
    }
}
```

### Weapon preset система

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

```json
{
    "weaponpresets": [
        {
            "name": "AKMPreset",
            "weapon": "AKM",
            "magazine": "Mag_AKM_30Rnd",
            "ammo": 30,
            "attachments": ["AK_Suppressor", "AK_Buttstock", "AK_Handguard"]
        },
        {
            "name": "M4Preset",
            "weapon": "M4A1",
            "magazine": "Mag_STANAG_30Rnd",
            "ammo": 30,
            "attachments": ["M4_Suppressor", "M4_CQBButtstock", "M4_RISHandguard"]
        }
    ]
}
```

---

## 💡 Examples

### Example 1: Создать static NPC

**config.cpp:**
```cpp
class CfgPatches
{
    class MyNPCs
    {
        units[] = {"BanditAI_Adam"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};
```

**scripts/4_World/Entities/AI/BanditAI_Adam.c:**
```c
class BanditAI_Adam extends DayZInfected
{
    override void OnInit()
    {
        super.OnInit();
        // Настройки NPC
    }
}
```

**types.xml:**
```xml
<type name="BanditAI_Adam">
    <nominal>10</nominal>
    <min>5</min>
    <max>15</max>
    <lifetime>7200</lifetime>
    <restock>0</restock>
</type>
```

### Example 2: JSON спавн конфигурация

**$profile:\MyNPCs\StaticAIB.json:**
```json
{
    "version": 1,
    "staticaib": [
        {
            "name": "ChernoTrader",
            "npcpos": [3600.0, 10.0, 5300.0],
            "npcclass": "BanditAI_Adam",
            "npcloadout": ["M65Jacket_Black", "CargoPants_Black"],
            "triggerradius": 30.0,
            "waypoints": [
                [3600.0, 10.0, 5300.0],
                [3610.0, 10.0, 5310.0]
            ]
        }
    ]
}
```

### Example 3: Waypoint патруль

```c
void CreatePatrolRoute()
{
    array<vector> waypoints = {
        "3600.0 10.0 5300.0",
        "3610.0 10.0 5310.0",
        "3620.0 10.0 5305.0",
        "3615.0 10.0 5295.0"
    };

    BanditAI_Adam npc = BanditAI_Adam.Cast(GetGame().CreateObject("BanditAI_Adam", "3600.0 10.0 5300.0"));

    foreach (vector wp : waypoints)
    {
        npc.AddWaypoint(wp);
    }

    npc.StartPatrol();
}
```

---

## 🔗 Related Topics

→ [PlayerBase API](PLAYERBASE_API.md) - Работа с игроками
→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - OnInit, EEKilled
→ [GetGame API](GETGAME_API.md) - CreateObject

---

## 🚨 Troubleshooting

**Ошибка:** "NPC not responding"
- **Решение:** Проверьте что NPC наследуется от DayZPlayer/Man

**Ошибка:** "Waypoints not working"
- **Решение:** Проверьте формат vector: "x y z" (строка)

**Ошибка:** "Loadout not equipping"
- **Решение:** Проверьте что предметы существуют в config.cpp

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived
**Источник:** AI Bandits mod
**Требует проверки:** Да

**Почему ⚠️?**
- Это НЕ официальная документация DayZ
- AI Bandits НЕ использует raw_data patterns
- DayZInfected как основа для NPC - необычный подход

**Что проверено:**
- ✅ Общая концепция JSON конфигурации
- ✅ Waypoint система концептуально
- ✅ Trigger система

**Что НЕ проверено:**
- ❌ Точный синтаксис DayZInfected наследования
- ❌ Все AI команды движения
- ❌ Полный список weapon preset параметров

**Что нужно проверить:**
- raw_data/ для DayZInfected класса
- raw_data/ для AI command system
- Official samples для AI examples

---

## 🔍 Как использовать эту информацию

1. **Для концепций:** Используйте для понимания JSON/Waypoint подхода
2. **Для кода:** ПРОВЕРЯЙТЕ на raw_data/DayZInfected.c
3. **Для production:** Изучайте AI Bandits мод напрямую

---

*Последнее обновление: 2025-01-16*
*Источник: AI Bandits mod (требует raw_data проверки)*
