# Time-Based Events Example

**Status:** ✅ Working Example
**Source:** SLP_Realistic_Ambient (3619503223)
**Complexity:** Simple

---

## ⚡ 30-Second Summary

Day/night event system with time-based spawning, dawn/dusk triggers, and seasonal mechanics.

---

## 🎯 Features

- ✅ **Day/Night Spawning** - Different mobs per time
- ✅ **Dawn/Dusk Events** - Triggers at specific times
- ✅ **Time Checking** - GetWorldTime() (0-24)
- ✅ **Seasonal System** - Month-based events

---

## 💡 Key Concepts

### Get World Time

```c
float gameTime = GetGame().GetWorld().GetWorldTime();
// Returns 0.0 - 24.0

// Night: 20:00 - 06:00
if (gameTime >= 20.0 || gameTime < 6.0)
{
    SpawnNightMobs();
}
```

### Dawn/Dusk

```c
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
```

---

## 🔗 Related Topics

→ [Time Events](../topics/22_time_events.md) - Mechanics
→ [Mission](../topics/17_mission_system.md) - WorldData

---

*Последнее обновление: 2025-01-18*
