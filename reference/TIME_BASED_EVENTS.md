# Time-Based Events (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [GetGame API](GETGAME_API.md) | [Mission API](MISSION_API.md)

---

## ⚡ 30-Second Summary

События по времени суток: динамическая активация, призрачные объекты, звуковая система, day/night механики.

---

## 🎯 5-Minute Solution: Day/Night Spawner

### Time-based entity spawner

```c
class TimeBasedSpawner
{
    protected ref array<EntityAI> m_SpawnedEntities = new array<EntityAI>;

    void Update()
    {
        float gameTime = GetGame().GetWorld().GetWorldTime();

        // Spawn ghosts at night (20:00 - 06:00)
        if (gameTime >= 20.0 || gameTime < 6.0)
        {
            SpawnGhosts();
        }
        else
        {
            DespawnGhosts();
        }
    }

    void SpawnGhosts()
    {
        if (m_SpawnedEntities.Count() > 0)
            return; // Already spawned

        // Spawn 3 ghosts
        for (int i = 0; i < 3; i++)
        {
            vector pos = GetRandomPosition();
            EntityAI ghost = GetGame().CreateObject("GhostEntity", pos);
            m_SpawnedEntities.Insert(ghost);
        }
    }

    void DespawnGhosts()
    {
        foreach (EntityAI entity : m_SpawnedEntities)
        {
            GetGame().ObjectDelete(entity);
        }

        m_SpawnedEntities.Clear();
    }
}
```

---

## 📚 Deep Dive

## SLP_Realistic_Ambient (3619503223) - Ghost Day System

### Time-based activation system

```c
class Ghostday extends House
{
    protected bool m_IsActive;
    protected float m_ActivationTime = 20.0; // 20:00 (8 PM)
    protected float m_DeactivationTime = 6.0; // 06:00 (6 AM)

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        float gameTime = GetGame().GetWorld().GetWorldTime();

        // Check if should activate
        if (!m_IsActive && gameTime >= m_ActivationTime)
        {
            Activate();
        }
        // Check if should deactivate
        else if (m_IsActive && gameTime < m_DeactivationTime)
        {
            Deactivate();
        }
    }

    void Activate()
    {
        m_IsActive = true;

        // Show entity
        SetAnimationPhase("visibility", 0);

        // Enable collision
        EnableCollision(true);

        // Play ambient sound
        SEffectManager.PlaySound("ghost_ambient_SoundSet", GetPosition());
    }

    void Deactivate()
    {
        m_IsActive = false;

        // Hide entity
        SetAnimationPhase("visibility", 1);

        // Disable collision
        EnableCollision(false);
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3619503223\addons\SLP_Realistic_Ambient\SLP_Realistic_Ambient\scripts\4_world\Ghostday.c:34-48, 230-279`

### Animated ghost entities

```c
class GhostEntity extends DayZCreatureAI
{
    protected float m_JumpHeight = 2.0;
    protected float m_JumpInterval = 5.0;
    protected float m_LastJump;

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        float currentTime = GetGame().GetTickTime();

        // Jump every 5 seconds
        if (currentTime - m_LastJump > m_JumpInterval)
        {
            PerformJump();
            m_LastJump = currentTime;
        }

        // Float animation
        vector pos = GetPosition();
        pos[1] = pos[1] + Math.Sin(currentTime) * 0.1; // Float up/down
        SetPosition(pos);
    }

    void PerformJump()
    {
        // Jump into the air
        vector jumpVel = Vector(0, m_JumpHeight, 0);
        AddVelocity(jumpVel);

        // Play jump sound
        SEffectManager.PlaySound("ghost_jump_SoundSet", GetPosition());
    }

    override bool CanBeTargetedByAI()
    {
        return false; // Ghosts cannot be attacked by AI
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3619503223\addons\SLP_Realistic_Ambient\SLP_Realistic_Ambient\scripts\4_world\Ghostday.c:139-196`

### Dynamic ambient sound system

```c
class AmbientSoundManager
{
    protected Sound m_DaySound;
    protected Sound m_NightSound;

    void Update()
    {
        float gameTime = GetGame().GetWorld().GetWorldTime();

        // Play day sounds (06:00 - 20:00)
        if (gameTime >= 6.0 && gameTime < 20.0)
        {
            if (!m_DaySound || !m_DaySound.IsSoundPlaying())
            {
                m_DaySound = SEffectManager.PlaySound("ambient_day_SoundSet", GetPosition());
            }

            // Stop night sound
            if (m_NightSound && m_NightSound.IsSoundPlaying())
            {
                m_NightSound.SoundStop();
            }
        }
        // Play night sounds (20:00 - 06:00)
        else
        {
            if (!m_NightSound || !m_NightSound.IsSoundPlaying())
            {
                m_NightSound = SEffectManager.PlaySound("ambient_night_SoundSet", GetPosition());
            }

            // Stop day sound
            if (m_DaySound && m_DaySound.IsSoundPlaying())
            {
                m_DaySound.SoundStop();
            }
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3619503223\addons\SLP_Realistic_Ambient\SLP_Realistic_Ambient\scripts\4_world\Ghostday.c:64-72`

### Config-based time activation

```cpp
class CfgVehicles
{
    class House;
    class Ghostday: House
    {
        // Activation time (0-24 format)
        activationTime = 20.0; // 20:00

        // Deactivation time
        deactivationTime = 6.0; // 06:00

        // Ambient sounds
        soundDay = "ambient_day_SoundSet";
        soundNight = "ambient_night_SoundSet";
    };
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3619503223\addons\SLP_Realistic_Ambient\SLP_Realistic_Ambient\config.cpp:196-203`

---

## 💡 Examples

### Example 1: Dawn/Dusk events

```c
class TimeEventManager
{
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

    void OnDawn()
    {
        // Spawn morning fog
        GetGame().GetWeather().SetFog(0.8);

        // Stop night events
        StopNightEvents();
    }

    void OnDusk()
    {
        // Enable night vision zones
        EnableNightZones();

        // Start night events
        StartNightEvents();
    }
}
```

### Example 2: Seasonal events

```c
class SeasonalEventManager
{
    int GetCurrentMonth()
    {
        int year, month, day, hour, minute;
        GetGame().GetWorld().GetDate(year, month, day, hour, minute);
        return month;
    }

    void UpdateSeasonalEvents()
    {
        int month = GetCurrentMonth();

        // Winter events (December, January, February)
        if (month == 12 || month == 1 || month == 2)
        {
            EnableWinterEvents();
        }
        // Summer events (June, July, August)
        else if (month == 6 || month == 7 || month == 8)
        {
            EnableSummerEvents();
        }
    }

    void EnableWinterEvents()
    {
        // Increase cold
        GetGame().GetWeather().SetTemperature(-10);

        // Spawn snow
        GetGame().GetWeather().SetOvercast(0.9);
    }

    void EnableSummerEvents()
    {
        // Increase heat
        GetGame().GetWeather().SetTemperature(30);

        // Clear sky
        GetGame().GetWeather().SetOvercast(0.1);
    }
}
```

---

## 🔗 Related Topics

→ [GetGame API](GETGAME_API.md) - GetWorld(), GetWorldTime()
→ [Mission API](MISSION_API.md) - WorldData access
→ [Sound System](SOUND_SYSTEM.md) - SEffectManager patterns

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ Time-based activation: Ghostday.c:34-48, 230-279
- ✅ Animated ghosts: Ghostday.c:139-196
- ✅ Ambient sound system: Ghostday.c:64-72
- ✅ Config activation: config.cpp:196-203

**Что НЕ проверено:**
- ❌ Работает ли без SLP_Realistic_Ambient мода
- ❌ Производительность с множеством entities
- ❌ Совместимость с другими weather модами

---

## 💡 Tips

### Best Practices

1. **Используйте GetWorld().GetWorldTime()** для получения времени (0-24)
2. **Проверяйте deltaTime** в OnUpdate (не каждый кадр)
3. **Используйте SetAnimationPhase** для show/hide entities
4. **EnableCollision(false)** для non-interactive ghosts
5. **Останавливайте звуки** при деактивации

### Time Format

**DayZ Time Format:**
- 0.0 = 00:00 (midnight)
- 6.0 = 06:00 (6 AM)
- 12.0 = 12:00 (noon)
- 18.0 = 18:00 (6 PM)
- 23.99 = 23:59 (11:59 PM)

**Common Times:**
- Dawn: 6.0 (06:00)
- Noon: 12.0 (12:00)
- Dusk: 18.0 (18:00)
- Midnight: 0.0 (00:00)
- Night Start: 20.0 (20:00)

### Performance Tips

- **Не обновляйте каждый кадр** (используйте timer)
- **Удаляйте entities** при деактивации
- **Используйте object pooling** для частых спавнов
- **Ограничьте количество** активных entities (< 20)

### Common Pitfalls

- ❌ **Обновлять каждый кадр** → performance drop
- ❌ **Игнорировать timezone** → неправильное время
- ❌ **Забыть EnableCollision** → interactive ghosts
- ❌ **Не останавливать звуки** → overlapping audio

---

*Последнее обновление: 2025-01-16*
*Источник: SLP_Realistic_Ambient (3619503223)*
