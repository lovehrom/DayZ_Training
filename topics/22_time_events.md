# Time-Based Events

**Status:** ⚠️ Mod-derived
**Related:** [Mission](17_mission_system.md) | [NPC](15_zombie_ai.md)

---

## ⚡ 30-Second Summary

События по времени: day/night спавн, динамическая активация, GetWorld().GetWorldTime() для времени суток (0-24).

---

## 🎯 5-Minute Solution

### Day/night spawn

```c
class TimeSpawner
{
    void Update()
    {
        float gameTime = GetGame().GetWorld().GetWorldTime();

        // Night (20:00 - 06:00)
        if (gameTime >= 20.0 || gameTime < 6.0)
        {
            SpawnNightMobs();
        }
        else
        {
            DespawnMobs();
        }
    }
}
```

---

## 💡 Examples

### Пример 1: Dawn/Dusk

```c
void CheckDawnDusk()
{
    float gameTime = GetGame().GetWorld().GetWorldTime();

    // Dawn at 06:00
    if (gameTime >= 6.0 && gameTime < 6.1)
    {
        OnDawn();
    }

    // Dusk at 18:00
    if (gameTime >= 18.0 && gameTime < 18.1)
    {
        OnDusk();
    }
}
```

### Пример 2: Seasonal

```c
int GetMonth()
{
    int year, month, day, hour, minute;
    GetGame().GetWorld().GetDate(year, month, day, hour, minute);
    return month;
}

void CheckSeason()
{
    int month = GetMonth();

    // Winter (Dec, Jan, Feb)
    if (month == 12 || month == 1 || month == 2)
    {
        SetColdWeather();
    }
}
```

---

## 🔗 Related Topics

→ [Mission](17_mission_system.md) - WorldData
→ [NPC](15_zombie_ai.md) - Night spawns
→ [Reference: Time Events](../reference/TIME_BASED_EVENTS.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
*Источник: SLP_Realistic_Ambient (3619503223)*
