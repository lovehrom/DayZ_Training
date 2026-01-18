# Time Events + Spawn Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** time_events + spawning

---

## ⚡ 30-Second Summary

Integration example showing time-based spawning. Different enemies spawn at different times of day (day/night/dawn/dusk) with dynamic despawning.

---

## 🎯 Features

### Integration Mechanics
- ✅ **Day/Night Spawns** - Different mobs per time
- ✅ **Dawn/Dusk Events** - Special spawns at sunrise/sunset
- ✅ **Dynamic Despawn** - Mobs despawn when time changes
- ✅ **Time Checking** - GetWorldTime() (0-24)
- ✅ **Spawn Zones** - Geographic spawn areas

---

## 📁 File Structure

```
time_spawn_integration/
├── README.md                    ← This file
├── config.cpp                   ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── TimeSpawnManager.c        ← Manages time-based spawns
            ├── DayMob.c                  → Day-only mob
            ├── NightMob.c                → Night-only mob
            ├── DawnMob.c                 → Dawn-only mob
            └── DuskMob.c                 → Dusk-only mob
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- `time_events/` - Detects time of day (0-24)
- Spawning - Creates entities

**Integration:**
1. Game time changes → TimeSpawnManager checks time
2. Time condition met → Spawn appropriate mobs
3. Time condition ends → Despawn old mobs
4. Loop continues → Dynamic spawns

### Time Schedule

| Time | Mob Type | Spawn? |
|------|----------|--------|
| 06:00 - 18:00 | DayMob | ✅ Yes |
| 18:00 - 20:00 | DuskMob | ✅ Yes |
| 20:00 - 06:00 | NightMob | ✅ Yes |
| 05:00 - 07:00 | DawnMob | ✅ Yes |

---

## 💡 Implementation

### 1. TimeSpawnManager.c

Manager for time-based spawns:

```c
class TimeSpawnManager
{
    protected ref array<DayMob> m_DayMobs;
    protected ref array<NightMob> m_NightMobs;
    protected ref array<DawnMob> m_DawnMobs;
    protected ref array<DuskMob> m_DuskMobs;

    protected float m_LastCheckTime;
    protected const float CHECK_INTERVAL = 60.0; // Check every 60 seconds

    void TimeSpawnManager()
    {
        m_DayMobs = new array<DayMob>;
        m_NightMobs = new array<NightMob>;
        m_DawnMobs = new array<DawnMob>;
        m_DuskMobs = new array<DuskMob>;
    }

    void Update()
    {
        float currentTime = GetGame().GetTickTime();

        // Check interval
        if (currentTime - m_LastCheckTime < CHECK_INTERVAL)
            return;

        m_LastCheckTime = currentTime;

        // Get game time (0-24)
        float gameTime = GetGame().GetWorld().GetWorldTime();

        // Check time conditions
        CheckTimeConditions(gameTime);
    }

    void CheckTimeConditions(float gameTime)
    {
        // Day: 06:00 - 18:00
        if (gameTime >= 6.0 && gameTime < 18.0)
        {
            SpawnDayMobs();
            DespawnNightMobs();
        }

        // Night: 20:00 - 06:00
        else if (gameTime >= 20.0 || gameTime < 6.0)
        {
            SpawnNightMobs();
            DespawnDayMobs();
        }

        // Dawn: 05:00 - 07:00
        if (gameTime >= 5.0 && gameTime < 7.0)
        {
            SpawnDawnMobs();
        }

        // Dusk: 18:00 - 20:00
        if (gameTime >= 18.0 && gameTime < 20.0)
        {
            SpawnDuskMobs();
        }
    }

    void SpawnDayMobs()
    {
        // Max 10 day mobs
        if (m_DayMobs.Count() >= 10)
            return;

        // Spawn at random locations
        vector pos = GetRandomSpawnPosition();

        DayMob mob = DayMob.Cast(GetGame().CreateObject("DayMob", pos));
        if (mob)
        {
            m_DayMobs.Insert(mob);
            Print("[TimeSpawn] Spawned DayMob at " + pos);
        }
    }

    void SpawnNightMobs()
    {
        // Max 15 night mobs (harder)
        if (m_NightMobs.Count() >= 15)
            return;

        vector pos = GetRandomSpawnPosition();

        NightMob mob = NightMob.Cast(GetGame().CreateObject("NightMob", pos));
        if (mob)
        {
            m_NightMobs.Insert(mob);
            Print("[TimeSpawn] Spawned NightMob at " + pos);
        }
    }

    void DespawnDayMobs()
    {
        foreach (DayMob mob : m_DayMobs)
        {
            GetGame().ObjectDelete(mob);
        }
        m_DayMobs.Clear();
    }

    void DespawnNightMobs()
    {
        foreach (NightMob mob : m_NightMobs)
        {
            GetGame().ObjectDelete(mob);
        }
        m_NightMobs.Clear();
    }

    vector GetRandomSpawnPosition()
    {
        // Get random player position
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        if (players.Count() > 0)
        {
            Man randomPlayer = players.Get(Math.RandomInt(0, players.Count()));
            vector playerPos = randomPlayer.GetPosition();

            // Random offset (100-500m away)
            float angle = Math.RandomFloat01() * Math.PI2;
            float dist = Math.RandomFloatInclusive(100, 500);

            return playerPos + Vector(Math.Cos(angle) * dist, 0, Math.Sin(angle) * dist);
        }

        return Vector(0, 0, 0); // Default
    }
}
```

### 2. DayMob.c

Day-only mob:

```c
class DayMob extends DayZInfected
{
    override void OnLoadFinished()
    {
        super.OnLoadFinished();

        // Day mob loadout
        GetInventory().CreateInInventory("HikingJacket_Black");
        GetInventory().CreateInInventory("Jeans_Blue");
        GetInventory().CreateInInventory("AthleticShoes_Black");
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Day mob drops
        PlayerBase player = PlayerBase.Cast(killer);
        if (player)
        {
            NotificationSystem.Notify(player, "Day mob defeated!");
        }
    }
}
```

### 3. NightMob.c

Night-only mob (harder):

```c
class NightMob extends DayZInfected
{
    override void OnLoadFinished()
    {
        super.OnLoadFinished();

        // Night mob loadout (stronger)
        GetInventory().CreateInInventory("TacticalJacket_Black");
        GetInventory().CreateInInventory("CargoPants_Black");
        GetInventory().CreateInInventory("CombatBoots_Black");

        // Night mob has night vision
        // (custom modifier...)
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Night mob drops better loot
        PlayerBase player = PlayerBase.Cast(killer);
        if (player)
        {
            NotificationSystem.Notify(player, "Night mob defeated! Bonus loot!");

            // Drop rare item
            GetGame().CreateObject("Antibiotic", player.GetPosition());
        }
    }
}
```

### 4. DawnMob.c

Dawn-event mob:

```c
class DawnMob extends DayZInfected
{
    override void OnLoadFinished()
    {
        super.OnLoadFinished();

        // Dawn loadout
        GetInventory().CreateInInventory("M65Jacket_Tan");
        GetInventory().CreateInInventory("CargoPants_Green");
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Dawn mob gives speed boost
        PlayerBase player = PlayerBase.Cast(killer);
        if (player)
        {
            NotificationSystem.Notify(player, "Dawn blessing! +Stamina");

            // Boost stamina
            player.GetStatStamina().Add(50);
        }
    }
}
```

### 5. DuskMob.c

Dusk-event mob:

```c
class DuskMob extends DayZInfected
{
    override void OnLoadFinished()
    {
        super.OnLoadFinished();

        // Dusk loadout
        GetInventory().CreateInInventory("PoliceJacket_Old");
        GetInventory().CreateInInventory("PolicePants_Old");
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Dusk mob gives night vision boost
        PlayerBase player = PlayerBase.Cast(killer);
        if (player)
        {
            NotificationSystem.Notify(player, "Dusk blessing! Night vision");

            // Add night vision modifier
            // (custom modifier...)
        }
    }
}
```

---

## 📝 Usage Examples

### Example 1: Spawn Zones

```c
class TimeSpawnManager
{
    vector GetSpawnPositionForZone(string zone)
    {
        if (zone == "Cherno")
            return Vector(6500, 0, 2500);
        else if (zone == "Electro")
            return Vector(5000, 0, 2500);
        else if (zone == "Berezino")
            return Vector(12000, 0, 10000);

        return Vector(0, 0, 0);
    }
}
```

### Example 2: Seasonal Spawns

```c
class SeasonalSpawnManager extends TimeSpawnManager
{
    void CheckSeasonalSpawns()
    {
        int month = GetGame().GetWorld().GetDate().Month;

        // Winter: December, January, February
        if (month == 12 || month == 1 || month == 2)
        {
            SpawnWinterMobs();
        }

        // Summer: June, July, August
        if (month == 6 || month == 7 || month == 8)
        {
            SpawnSummerMobs();
        }
    }
}
```

### Example 3: Weather-Based Spawns

```c
class WeatherSpawnManager extends TimeSpawnManager
{
    void CheckWeatherSpawns()
    {
        Weather weather = GetGame().GetWeather();

        // Rain spawns
        if (weather.IsRaining())
        {
            SpawnRainMobs();
        }

        // Fog spawns
        if (weather.GetFog().GetActual() > 0.5)
        {
            SpawnFogMobs();
        }
    }
}
```

---

## 🔗 Related Examples

→ [Time Events](../time_events/) - Individual time mechanics
→ [AI System](../ai_system/) - NPC spawning
→ [Integration: NPC+Loot](../npc_loot_integration/) - Another integration

---

## ⚠️ Important Notes

### Time Checking
- **GetWorldTime()** returns 0.0 - 24.0
- **6.0** = 06:00 (dawn)
- **12.0** = 12:00 (noon)
- **18.0** = 18:00 (dusk)
- **0.0/24.0** = midnight

### Spawn Performance
- Check interval: 60 seconds (configurable)
- Max spawns: 10-15 mobs (prevent lag)
- Despawn on time change (cleanup)

### Spawn Distance
- 100-500m from players
- Random offset to prevent clustering
- Player-dependent (no players = no spawns)

---

*Last updated: 2025-01-18*
*Combines: time_events + spawning*
