# Zombie Customization (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [AI System](AI_SYSTEM.md) | [RPC System](RPC_NETWORKING.md)

---

## ⚡ 30-Second Summary

Кастомные зомби: JSON/XML спавн конфигурация, territory системы, enhanced damage, кастомный loot drop.

---

## 🎯 5-Minute Solution: Custom Zombie Type

### Config definition

```cpp
class CfgPatches
{
    class MyCustomZombie
    {
        units[] = {"ZombieMale_Miner"};
        requiredVersion = 0.1;
    };
};

class CfgVehicles
{
    class ZombieMaleBase;
    class ZombieMale_Miner: ZombieMaleBase
    {
        scope = 2;
        displayName = "Miner Zombie";

        // Increased health
        health = 300;

        // Increased damage
        damage = 20;

        // Custom sounds
        SoundBreath = "zombie_breath_miner_SoundSet";
    };
};
```

### Custom behavior

```c
class ZombieMale_Miner extends ZombieMaleBase
{
    override void Init()
    {
        super.Init();

        // Custom initialization
        SetHealth(300);
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Drop custom loot
        SpawnLoot();
    }

    void SpawnLoot()
    {
        // Spawn mining helmet
        GetGame().CreateObject("MiningHelmet", GetPosition());

        // Spawn ore
        GetGame().CreateObject("Ore_Iron", GetPosition());
    }
}
```

---

## 📚 Deep Dive

## Dutch_Bunker (2718893193) - Zombie Territory System

### XML-based territory definition

```xml
<!-- Zombies/Bunkerzombie_territories.xml -->
<territories>
    <territory>
        <name>Bunker_Alpha</name>
        <position x="5000" z="5000" />
        <radius>100</radius>
        <zombie_class>ZombieMale_Bunker</zombie_class>
        <max_zombies>20</max_zombies>
        <respawn_time>300</respawn_time>
    </territory>
</territories>
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\2718893193\Zombies\Bunkerzombie_territories.xml:1`

### Event spawn configuration

```xml
<!-- Zombies/cfgeventspawns.xml -->
<event_spawn>
    <name>BunkerZombieEvent</name>
    <position x="5000" z="5000" />
    <radius>100</radius>
    <zombie_class>ZombieMale_Bunker</zombie_class>
    <count>20</count>
    <respawn>300</respawn>
</event_spawn>
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\2718893193\Zombies\cfgeventspawns.xml:1`

### Custom building loader

```c
class EditorLoaderModule
{
    void LoadCustomBuildings()
    {
        // Load bunker from .dze file
        string dzeFile = "$profile:DutchBunker\\Dutch_Bunker.dze";

        if (FileExist(dzeFile))
        {
            // Parse and spawn buildings
            ParseDZEFile(dzeFile);
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\2718893193\addons\DutchBunker\Dutch_Bunker\DutchBunker\5_Mission\EditorLoaderModule.c:1`

---

## Zombie_Bears_thePurge (3116788142) - Enhanced Animal AI

### Three-tier bear system

```cpp
class CfgVehicles
{
    class AnimalBase;
    class Animal_UrsusArctos: AnimalBase
    {
        class DamageModifiers
        {
            // Normal bear
            class Health
            {
                health = 200;
            }
        }
    };

    class Animal_UrsusArctos_Killer: Animal_UrsusArctos
    {
        // Enhanced bear
        health = 400;
        damage = 50;
    };

    class Animal_UrsusArctos_Elite: Animal_UrsusArctos_Killer
    {
        // Elite bear (boss)
        health = 800;
        damage = 100;
    };
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3116788142\addons\KillerBears\KillerBears\config.cpp:86-663`

### Tiered damage system

```cpp
class Animal_UrsusArctos_Killer: Animal_UrsusArctos
{
    class HitZones
    {
        class Head
        {
            health = 100;
            modifier = 2.0; // 2x damage to head
        }

        class Torso
        {
            health = 200;
            modifier = 1.0;
        }

        class Legs
        {
            health = 50;
            modifier = 1.5;
        }
    };
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3116788142\addons\KillerBears\KillerBears\config.cpp:114-153`

### Custom resource gathering

```cpp
class Animal_UrsusArctos_Killer: Animal_UrsusArctos
{
    class Gathering
    {
        class Meat
        {
            item = "BearMeat";
            min = 4;
            max = 8;
        }

        class Pelt
        {
            item = "BearPelt";
            min = 2;
            max = 4;
        }

        class Fat
        {
            item = "AnimalFat";
            min = 1;
            max = 3;
        }

        class Bones
        {
            item = "AnimalBone";
            min = 5;
            max = 10;
        }
    };
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3116788142\addons\KillerBears\KillerBears\config.cpp:240-277`

---

## Specimen RO-9 (3645022168) - Custom AI Entity

### Extended zombie with unique behavior

```c
class Subject extends ZombieMaleBase
{
    protected vector m_Scale;
    protected Sound m_BreathingSound;

    override void Init()
    {
        super.Init();

        // Set custom scale
        m_Scale = "1.5 1.5 1.5";
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Dynamic scaling
        SetScale(m_Scale);

        // Restrict rotation to Y-axis only
        vector orientation = GetOrientation();
        SetOrientation(Vector(0, orientation[1], 0));

        // Breathing sound
        PlayBreathingSound();
    }

    void PlayBreathingSound()
    {
        if (!m_BreathingSound || !m_BreathingSound.IsSoundPlaying())
        {
            PlaySound("breathing_subject_SoundSet");
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645022168\addons\subject\subject\scripts\4_world\subject.c:3, 25-48, 52-68`

### Loot drop system with container

```c
class Subject extends ZombieMaleBase
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Spawn loot crate
        EntityAI crate = GetGame().CreateObject("SubjectCrate", GetPosition());

        // Fill crate with random loot
        FillCrate(crate);
    }

    void FillCrate(EntityAI crate)
    {
        // Random loot table
        array<string> loot = {
            "Rifle",
            "Magazine_Random",
            "AmmoBox",
            "MedicalKit"
        };

        for (int i = 0; i < 3; i++)
        {
            string item = loot.GetRandomElement();
            crate.GetInventory().CreateInInventory(item);
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645022168\addons\subject\subject\scripts\4_world\subject.c:79-88`

### Static loot crate

```c
class SubjectCrate extends Container_Base
{
    override void SetActions()
    {
        super.SetActions();

        // Disable moving
        AddAction(ActionTogglePlaceObject);
        RemoveAction(ActionMove);
    }

    override bool IsInventoryVisible()
    {
        return true; // Always show inventory
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645022168\addons\subject\subject\scripts\4_world\Crate.c:3-17`

### XML loot configuration

```xml
<!-- server_files/cfgspawnabletypes.xml -->
<type name="SubjectCrate">
    <cargo>
        <item name="Rifle" chance="0.3" />
        <item name="Magazine_Random" chance="0.5" />
        <item name="AmmoBox" chance="0.7" />
    </cargo>
</type>
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645022168\server_files\cfgspawnabletypes.xml:1-6`

---

## 💡 Examples

### Example 1: Fast zombie

```cpp
class ZombieMale_Fast extends ZombieMaleBase
{
    // Movement speed multiplier
    float m_SpeedMultiplier = 1.5;

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Run 50% faster
        SetMovementSpeed(m_SpeedMultiplier);
    }
}
```

### Example 2: Tank zombie

```c
class ZombieMale_Tank extends ZombieMaleBase
{
    override void Init()
    {
        super.Init();

        // 5x normal health
        SetHealth(1000);
    }

    override float GetDamageMultiplier(string damageType)
    {
        // Reduced damage from bullets
        if (damageType == "bullet")
        {
            return 0.5; // 50% damage
        }

        return 1.0; // Normal damage
    }
}
```

---

## 🔗 Related Topics

→ [AI System](AI_SYSTEM.md) - AI behavior patterns
→ [Storage System](STORAGE_SYSTEM.md) - Saving AI state
→ [RPC System](RPC_NETWORKING.md) - AI event communication

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ XML territory system: Bunkerzombie_territories.xml:1
- ✅ Event spawn config: cfgeventspawns.xml:1
- ✅ Enhanced bear AI: config.cpp:86-663
- ✅ Tiered damage: config.cpp:114-153
- ✅ Custom resource gathering: config.cpp:240-277
- ✅ Custom AI entity: subject.c:3, 25-48, 52-68
- ✅ Loot drop system: subject.c:79-88
- ✅ Static crate: Crate.c:3-17

**Что НЕ проверено:**
- ❌ Работают ли territory системы без DayZ Expansion
- ❌ Производительность с большим количеством зомби
- ❌ Совместимость с другими AI модами

---

## 💡 Tips

### Best Practices

1. **Используйте XML** для конфигурации спавна (легко редактировать)
2. **Наследуйтесь от ZombieMaleBase** для кастомных зомби
3. **Переопределяйте EEKilled()** для кастомного лута
4. **Используйте HitZones** для детальной системы урона
5. **Ограничивайте rotation** для AI entities (только Y-axis)

### Zombie Types

**Базовые типы:**
- `ZombieMaleBase` - мужчины зомби
- `ZombieFemaleBase` - женщины зомби
- `AnimalBase` - животные

**Enhanced:**
- Increased health/damage
- Custom loot tables
- Territory systems

### Performance Tips

- **Ограничьте количество** AI в одной зоне (< 50)
- **Используйте respawn** вместо постоянного спавна
- **Оптимизируйте HitZones** (минимум 3 зоны)
- **Удаляйте loot** если не подобран (> 30 мин)

---

*Последнее обновление: 2025-01-16*
*Источники: Dutch_Bunker (2718893193), Zombie_Bears_thePurge (3116788142), Specimen RO-9 (3645022168)*
