# Performance Troubleshooting - Решение проблем с производительностью

**Диагностика и исправление проблем с производительностью сервера и клиента.**

---

## 🚊 Symptom: Низкий FPS на сервере

### Симптомы
- Server FPS < 40
- Лаги у всех игроков
- Долгий отклик

---

### Диагностика

#### 1. Проверить server FPS

```c
// В консоли сервера
#monitor fps
```

Или проверьте `server_debug.log`

#### 2. Проверить количество entities

```c
// В скрипте
void PrintEntityCounts()
{
    array<Object> objects = new array<Object>;
    GetGame().GetObjectsAtPosition(GetPosition(), 1000, objects, NULL);

    Print("[Debug] Entity count: " + objects.Count());
}
```

---

### Решения

#### Problem: Too many entities

**Solution:**
1. Уменьшите количество спавна зомби
2. Очистите старый loot
3. Удалите неиспользуемые buildings

```c
// Cleanup loot
void CleanupOldLoot()
{
    array<EntityAI> loot = new array<EntityAI>;
    GetGame().GetEntitiesAtPosition(pos, 100, loot);

    foreach (EntityAI item : loot)
    {
        if (item.IsKindOf("SurvivorBase")) continue;
        if (item.IsKindOf("ZombieBase")) continue;

        // Check if old
        if (item.GetAge() > 3600) // 1 hour
        {
            GetGame().ObjectDelete(item);
        }
    }
}
```

→ [Performance Guide](../reference/PERFORMANCE_GUIDE.md#entity-limits) - Entity limits

---

#### Problem: Expensive OnUpdate()

**Solution:**
1. Уменьшите частоту обновлений
2. Используйте event-based updates вместо polling

```c
// ❌ BAD: Update every frame
override void OnUpdate(float timeslice)
{
    UpdateEconomy(); // Expensive!
    UpdateWeather();
    UpdateAI();
}

// ✅ GOOD: Update at appropriate rates
protected float m_LastEconUpdate;
protected float m_LastWeatherUpdate;
protected float m_LastAIUpdate;

override void OnUpdate(float timeslice)
{
    float now = GetGame().GetTickTime();

    if (now - m_LastEconUpdate > 60.0) // Every 60s
    {
        UpdateEconomy();
        m_LastEconUpdate = now;
    }

    if (now - m_LastWeatherUpdate > 5.0) // Every 5s
    {
        UpdateWeather();
        m_LastWeatherUpdate = now;
    }

    if (now - m_LastAIUpdate > 0.5) // Every 0.5s
    {
        UpdateAI();
        m_LastAIUpdate = now;
    }
}
```

→ [Performance Guide](../reference/PERFORMANCE_GUIDE.md#update-frequency) - Update frequency

---

#### Problem: Memory leaks

**Solution:**
1. Проверьте что все таймеры удаляются
2. Проверьте что все event handlers отписываются
3. Проверьте что все objects удаляются

```c
// ✅ GOOD: Cleanup
class MyManager
{
    protected int m_Timer;

    void MyManager()
    {
        m_Timer = GetGame().GetCallQueue().CallLater(Update, 1000, true);
    }

    void Cleanup()
    {
        if (m_Timer != 0)
        {
            GetGame().GetCallQueue().Remove(m_Timer);
            m_Timer = 0;
        }
    }

    ~MyManager()
    {
        Cleanup();
    }
}
```

→ [Performance Guide](../reference/PERFORMANCE_GUIDE.md#memory-leak-prevention) - Memory leaks

---

## 🌐 Symptom: Network Lag

### Симптомы
- Высокий ping у всех игроков
- Rubber-banding (возврат назад)
- Пакеты теряются

---

### Диагностика

#### 1. Проверить network traffic

```c
// Monitor RPCs
class RPCMonitor
{
    static int m_RPCCount = 0;
    static float m_LastCheck = 0;

    static void OnRPC()
    {
        m_RPCCount++;

        float now = GetGame().GetTickTime();
        if (now - m_LastCheck > 1.0)
        {
            Print("[Network] RPCs per second: " + m_RPCCount);
            m_RPCCount = 0;
            m_LastCheck = now;
        }
    }
}
```

---

### Решения

#### Problem: Too many RPCs

**Solution:**
1. Batch RPCs together
2. Reduce RPC frequency
3. Use client-side prediction

```c
// ❌ BAD: RPC every frame
override void OnUpdate(float timeslice)
{
    GetGame().RPCSingleParam(player, MY_RPC, new Param1<float>(timeslice), true);
}

// ✅ GOOD: Batch RPCs
void SendBatchUpdates(array<float> values)
{
    GetGame().RPCSingleParam(player, MY_RPC, new Param1<array<float>>(values), true);
}

// ✅ BETTER: Client-side prediction
override void OnUpdate(float timeslice)
{
    if (IsClient())
    {
        // Do visual only - NO RPC
        UpdateEffect(timeslice);
    }
    else
    {
        // Server validates occasionally
        if (Math.RandomFloat01() < 0.1) // 10% chance
        {
            GetGame().RPCSingleParam(player, MY_RPC, new Param1<float>(timeslice), true);
        }
    }
}
```

→ [Performance Guide](../reference/PERFORMANCE_GUIDE.md#network-optimization) - Network optimization

---

## 💾 Symptom: High Memory Usage

### Симптомы
- DayZ сервер использует >4GB RAM
- Memory постоянно растёт
- Server crash с out of memory

---

### Диагностика

#### 1. Проверить memory usage

```c
void PrintMemoryStats()
{
    array<Object> objects = new array<Object>;
    GetGame().GetObjectsAtPosition(GetPosition(), 1000, objects, NULL);

    int itemCount = 0;
    int zombieCount = 0;
    int playerCount = 0;

    foreach (Object obj : objects)
    {
        if (obj.IsInherited(ItemBase))
            itemCount++;

        if (obj.IsInherited(ZombieBase))
            zombieCount++;

        if (obj.IsInherited(PlayerBase))
            playerCount++;
    }

    Print("[Memory] Items: " + itemCount);
    Print("[Memory] Zombies: " + zombieCount);
    Print("[Memory] Players: " + playerCount);
    Print("[Memory] Total entities: " + objects.Count());
}
```

---

### Решения

#### Problem: Arrays never cleared

**Solution:**
```c
// ✅ GOOD: Clear arrays
class MyManager
{
    ref array<ref MyObject> m_Objects;

    void Cleanup()
    {
        for (int i = m_Objects.Count() - 1; i >= 0; i--)
        {
            if (m_Objects[i].IsExpired())
            {
                m_Objects.Remove(i);
            }
        }
    }
}
```

→ [Performance Guide](../reference/PERFORMANCE_GUIDE.md#memory-management) - Memory management

---

## 🐌 Symptom: Slow Loading

### Симптомы
- Долгая загрузка сервера (>5 минут)
- Долгий коннект игрока
- "Waiting for server" зависает

---

### Решения

#### Problem: Large JSON files

**Solution:**
1. Compress JSON data
2. Split into smaller files
3. Load on-demand

```c
// ❌ BAD: Load all at once
void LoadAllData()
{
    // 50MB JSON file - VERY SLOW
    LoadJSON("$profile:huge_file.json");
}

// ✅ GOOD: Load on demand
void LoadPlayerData(string steam64ID)
{
    // Small file per player - FAST
    LoadJSON("$profile:players/" + steam64ID + ".json");
}
```

---

## 📊 Profiling

### Enable script profiling

**config.cpp:**
```cpp
class CfgScripts
{
    class MyMod
    {
        profilingEnabled = 1;
    };
};
```

### Profile functions

```c
void MyExpensiveFunction()
{
    float startTime = GetGame().GetTickTime();

    // Do work
    DoWork();

    float duration = GetGame().GetTickTime() - startTime;

    Print("[Profile] MyExpensiveFunction took " + (duration * 1000) + " ms");

    if (duration > 0.005) // 5ms
    {
        Print("[Profile] WARNING: Too slow!");
    }
}
```

→ [Performance Guide](../reference/PERFORMANCE_GUIDE.md#profiling) - Profiling

---

## 🔧 Common Performance Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Low server FPS | Too many entities | Reduce spawn counts |
| High memory | Memory leaks | Cleanup properly |
| Network lag | Too many RPCs | Batch RPCs |
| Slow loading | Large JSON | Split files |
| Expensive loops | Nested loops | Use spatial partitioning |

---

## 📚 Additional Resources

→ [Performance Guide](../reference/PERFORMANCE_GUIDE.md) - Complete optimization guide
→ [Script Profiler](https://community.bistudio.com/wiki/DayZ:Script_Profiling) - Official docs
→ [Performance Tips](../topics/26_deployment.md) - Server setup tips

---

*Последнее обновление: 2025-01-18*
