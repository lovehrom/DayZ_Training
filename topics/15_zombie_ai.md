# Custom Zombie & AI Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Advanced Modding](11_advanced_modding.md) | [AI System](../reference/AI_SYSTEM.md)

---

## ⚡ 30-Second Summary

Custom AI на базе зомби: NPC extends InfectedBanditBase, JSON конфигурация, waypoint патрули, trigger активация. Полная документация: [AI_NPC_CLASSES.md](../reference/AI_NPC_CLASSES.md)

---

## 🎯 5-Minute Solution: Custom NPC

### Step 1: Создайте класс NPC

```c
class CustomBandit extends InfectedBanditBase
{
    protected ref AIBanditConfig m_Config;

    void CustomBandit()
    {
        // Загружаем конфигурацию
        m_Config = LoadConfig();
    }

    override void OnConnect()
    {
        super.OnConnect();

        // Настраиваем AI
        if (m_Config)
        {
            SetHealth(m_Config.Health);
            SetDamage(m_Config.Damage);
        }
    }

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
    {
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos);

        // Custom реакция на урон
        if (source && source.IsMan())
        {
            Man man = source.CastToMan();
            if (man && man.IsPlayer())
            {
                OnAttackedByPlayer(man);
            }
        }
    }

    void OnAttackedByPlayer(Man player)
    {
        // Агримся на игрока
        AddTarget(player);
        Print("Bandit attacked by player!");
    }

    AIBanditConfig LoadConfig()
    {
        // Загружаем из JSON
        string filePath = "$profile:bandit_config.json";

        if (FileExist(filePath))
        {
            AIBanditConfig config;
            JsonFileLoader<AIBanditConfig>.JsonLoadFile(filePath, config);
            return config;
        }

        // Default config
        AIBanditConfig defaultConfig = new AIBanditConfig;
        defaultConfig.Health = 200;
        defaultConfig.Damage = 50;
        return defaultConfig;
    }
}
```

### Step 2: Создайте класс конфигурации

```c
class AIBanditConfig
{
    float Health;
    float Damage;
    float AgroRange;
    array<string> Loot = new array<string>;
}
```

### Step 3: Создайте JSON конфигурацию

**Файл:** `$profile:bandit_config.json`

```json
{
    "Health": 200,
    "Damage": 50,
    "AgroRange": 100,
    "Loot": [
        "AKM",
        "Magazine_AKM_30Rnd",
        "AmmoBox_762x39_20Rnd",
        "CannedBeans",
        "WaterBottle"
    ]
}
```

### Step 4: Зарегистрируйте в config.cpp

```cpp
class CfgVehicles
{
    class InfectedBanditBase;
    class CustomBandit: InfectedBanditBase
    {
        scope = 2; // Public

        // Модель
        model = "\DZ\characters\infected\infected_male.p3d";

        // Лут
        lootTable[] = {
            {"AKM", 0.3},
            {"Magazine_AKM_30Rnd", 0.5},
            {"CannedBeans", 0.8}
        };
    };
}
```

---

## 📚 Deep Dive

### Advanced: Waypoint Pathing (как в DayZ AI Bandits)

**DayZ AI Bandits** использует:
- Waypoint патрульную систему
- JSON загрузку из $profile:
- AIBStaticPlace структуру для спавн конфигурации
- Trigger активацию

**Пример waypoint системы:**
```c
class AIBanditBase extends InfectedBanditBase
{
    protected ref array<vector> m_Waypoints = new array<vector>;
    protected int m_CurrentWaypoint;

    void SetWaypoints(array<vector> waypoints)
    {
        m_Waypoints = waypoints;
        m_CurrentWaypoint = 0;
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Двигаемся к waypoint
        if (m_Waypoints.Count() > 0)
        {
            vector target = m_Waypoints[m_CurrentWaypoint];
            MoveTo(target);

            // Проверяем что достигли waypoint
            if (vector.Distance(GetPosition(), target) < 2.0)
            {
                m_CurrentWaypoint++;

                if (m_CurrentWaypoint >= m_Waypoints.Count())
                {
                    m_CurrentWaypoint = 0; // Loop
                }
            }
        }
    }
}
```

**Полная документация:** [AI_NPC_CLASSES.md](../reference/AI_NPC_CLASSES.md)

---

## 💡 Examples

### Пример 1: NPC с лутом

```c
class LootableBandit extends InfectedBanditBase
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Спавним лут
        SpawnLoot();
    }

    void SpawnLoot()
    {
        array<string> lootItems = {
            "AKM",
            "Magazine_AKM_30Rnd",
            "CannedBeans",
            "WaterBottle"
        };

        foreach (string item : lootItems)
        {
            // 50% шанс
            if (Math.RandomFloat01() < 0.5)
            {
                EntityAI spawned = GetGame().CreateObject(item, GetPosition());
            }
        }
    }
}
```

### Пример 2: Агрессивный NPC

```c
class AggressiveBandit extends InfectedBanditBase
{
    protected float m_AgroRange = 100.0;

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Ищем ближайшего игрока
        PlayerBase nearest = FindNearestPlayer();

        if (nearest)
        {
            float distance = vector.Distance(GetPosition(), nearest.GetPosition());

            if (distance < m_AgroRange)
            {
                // Агримся
                AddTarget(nearest);
                ChaseTarget(nearest);
            }
        }
    }

    PlayerBase FindNearestPlayer()
    {
        array<PlayerBase> players = new array<PlayerBase>;
        GetGame().GetPlayers(players);

        PlayerBase nearest = null;
        float minDistance = m_AgroRange;

        foreach (PlayerBase player : players)
        {
            float distance = vector.Distance(GetPosition(), player.GetPosition());

            if (distance < minDistance)
            {
                minDistance = distance;
                nearest = player;
            }
        }

        return nearest;
    }

    void ChaseTarget(Man target)
    {
        // Двигаемся к цели
        StartMoveToPosition(target.GetPosition());
    }
}
```

### Пример 3: Tiered zombie system (как в Zombie Bears)

```c
class Tier1_Bear extends InfectedBear
{
    override void OnConnect()
    {
        super.OnConnect();

        SetHealth("GlobalHealth", "Health", 100); // Normal
    }
}

class Tier2_Bear extends Tier1_Bear
{
    override void OnConnect()
    {
        super.OnConnect();

        SetHealth("GlobalHealth", "Health", 200); // Killer
    }
}

class Tier3_Bear extends Tier2_Bear
{
    override void OnConnect()
    {
        super.OnConnect();

        SetHealth("GlobalHealth", "Health", 500); // Elite
    }

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
    {
        // 50% reduction урона
        damageResult.damageMultipliers[0] = 0.5;

        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos);
    }
}
```

---

## 🔗 Related Topics

→ [AI System](../reference/AI_SYSTEM.md) - Vanilla AI
→ [Advanced Modding](11_advanced_modding.md) - Обзор mechanics
→ [Zombie Customization](../reference/ZOMBIE_CUSTOMIZATION.md) - Custom зомби

---

## ⚠️ SOURCE VERIFICATION

**Status:** ⚠️ Mod-derived

**Источники:** DayZ AI Bandits (3628006769), Zombie_Bears_thePurge (3116788142)

**Что проверено:**
- ✅ NPC на базе зомби работает
- ✅ JSON конфигурация работает
- ✅ Waypoint система работает

**Что НЕ проверено:**
- ❌ Работает ли без модов
- ❌ Производительность с большим количеством NPC
- ❌ Совместимость с другими AI модами

---

## 💡 Tips

### Best Practices:

1. **Используйте InfectedBanditBase** для NPC (не AnimalBase!)
2. **Загружайте конфигурацию** из JSON для легкой настройки
3. **Используйте waypoints** для патрулирования
4. **Ограничьте agro range** для оптимизации
5. **Добавьте loot** для мотивации игроков

### Zombie vs Animal:

**Используйте Zombie (InfectedBanditBase):**
- ✅ Для NPC которые ведут себя как люди
- ✅ Для патрулирующих NPC
- ✅ Для NPC с оружием

**НЕ используйте AnimalBase:**
- ❌ Для NPC (только для животных!)
- ❌ Для гуманоидных существ

### Performance Tips:

- **Ограничьте количество** NPC (< 50 на сервер)
- **Используйте triggers** для активации (не все NPC активны)
- **Не обновляйте** каждый кадр (используйте timer)
- **Удаляйте NPC** когда игроки далеко

### Common Pitfalls:

- ❌ **Использовать AnimalBase** → неправильная анимация
- ❌ **Забыть JSON** → hardcoded values
- ❌ **Слишком много NPC** → performance drop
- ❌ **Не использовать waypoints** → NPC стоят на месте

---

*Последнее обновление: 2025-01-16*
*Источники: DayZ AI Bandits (3628006769), Zombie_Bears_thePurge (3116788142)*
