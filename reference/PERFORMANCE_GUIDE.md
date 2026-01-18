# Performance Guide

**Status:** ✅ Complete Reference
**Target:** Mod developers, server admins
**Goal:** Optimize DayZ mods for minimal performance impact

---

## ⚡ 30-Second Summary

Performance optimization guide for DayZ mods. Covers memory management, network optimization, entity limits, update frequency, and common anti-patterns.

---

## 📊 Performance Metrics

### Target Performance

| Metric | Target | Acceptable | Critical |
|--------|--------|------------|----------|
| Server FPS | 60+ | 40-60 | <40 |
| Client FPS | 60+ | 30-60 | <30 |
| Memory per mod | <100 MB | 100-500 MB | >500 MB |
| Network traffic | <5 KB/s/player | 5-10 KB/s/player | >10 KB/s/player |
| Script execution | <1ms/frame | 1-5ms/frame | >5ms/frame |

---

## 🔧 Memory Management

### Entity Creation Cost

| Entity Type | Memory | Creation Time |
|-------------|--------|---------------|
| Item (small) | ~1 KB | ~0.1ms |
| Item (large) | ~5 KB | ~0.5ms |
| Zombie | ~10 KB | ~1ms |
| Player | ~50 KB | N/A (automatic) |
| Building | ~20 KB | ~2ms |

**Best Practice:** Reuse entities, limit counts

---

### Memory Leak Prevention

#### ❌ BAD: Never clean up

```c
class MyManager
{
    ref array<ref MyObject> m_Objects;

    void MyManager()
    {
        m_Objects = new array<ref MyObject>;
    }

    void SpawnObject()
    {
        // NEVER REMOVED - MEMORY LEAK!
        m_Objects.Insert(new MyObject());
    }
}
```

#### ✅ GOOD: Cleanup regularly

```c
class MyManager
{
    ref array<ref MyObject> m_Objects;

    void MyManager()
    {
        m_Objects = new array<ref MyObject>;
    }

    void SpawnObject()
    {
        MyObject obj = new MyObject();
        obj.Lifetime = 600; // 10 minutes
        m_Objects.Insert(obj);
    }

    void Update(float deltaTime)
    {
        // Remove expired objects
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

---

### Timer Cleanup

#### ❌ BAD: Timers never removed

```c
class MyManager
{
    void MyManager()
    {
        // Timer NEVER cleaned up
        GetGame().GetCallQueue().CallLater(Update, 1000, true);
    }

    void Update()
    {
        // Do something
    }
}
```

#### ✅ GOOD: Store and cleanup timers

```c
class MyManager
{
    protected int m_UpdateTimer;

    void MyManager()
    {
        m_UpdateTimer = GetGame().GetCallQueue().CallLater(Update, 1000, true);
    }

    void Cleanup()
    {
        // Remove timer
        if (m_UpdateTimer != 0)
        {
            GetGame().GetCallQueue().Remove(m_UpdateTimer);
            m_UpdateTimer = 0;
        }
    }
}
```

---

### Event Handler Leaks

#### ❌ BAD: Never unregister events

```c
modded class PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Event NEVER unregistered
        GetGame().GetCallQueue().CallLater(SomeMethod, 1000, true);
    }
}
```

#### ✅ GOOD: Unregister on disconnect

```c
modded class PlayerBase
{
    protected int m_Timer;

    override void OnConnect()
    {
        super.OnConnect();

        m_Timer = GetGame().GetCallQueue().CallLater(SomeMethod, 1000, true);
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Unregister event
        if (m_Timer != 0)
        {
            GetGame().GetCallQueue().Remove(m_Timer);
        }
    }
}
```

---

## 🌐 Network Optimization

### RPC Bandwidth Usage

| RPC Type | Bandwidth | Frequency |
|----------|----------|-----------|
| Single param | ~50 bytes | Per call |
| Multiple params | ~100 bytes | Per call |
| Broadcast | N × 100 bytes | Per player |

**Best Practice:** Batch RPCs, minimize calls

---

### RPC Batching

#### ❌ BAD: Multiple RPCs

```c
void NotifyPlayer(PlayerBase player)
{
    // 5 separate RPCs = 500 bytes
    NotificationSystem.Notify(player, "Message 1");
    NotificationSystem.Notify(player, "Message 2");
    NotificationSystem.Notify(player, "Message 3");
    NotificationSystem.Notify(player, "Message 4");
    NotificationSystem.Notify(player, "Message 5");
}
```

#### ✅ GOOD: Single batched RPC

```c
void NotifyPlayer(PlayerBase player)
{
    // 1 RPC = ~100 bytes
    array<string> messages = {"Message 1", "Message 2", "Message 3", "Message 4", "Message 5"};
    NotificationSystem.NotifyBatch(player, messages);
}
```

---

### Client-Side Prediction

#### ❌ BAD: Server-authorize everything

```c
modded class PlayerBase
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // RPC every frame - HUGE network traffic!
        GetGame().RPCSingleParam(this, SOME_RPC, new Param1<float>(timeslice), true);
    }
}
```

#### ✅ GOOD: Predict on client

```c
modded class PlayerBase
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Client-side prediction - NO RPC
        if (IsClient())
        {
            // Do something visual only
            UpdateEffect(timeslice);
        }
        else
        {
            // Server validates occasionally
            if (Math.RandomFloat01() < 0.1) // 10% chance per frame
            {
                GetGame().RPCSingleParam(this, SOME_RPC, new Param1<float>(timeslice), true);
            }
        }
    }
}
```

---

### Minimize Network Traffic

```c
class NetworkOptimizer
{
    // ❌ BAD: Send everything always
    void SendAllData(PlayerBase player)
    {
        foreach (EntityAI item : m_AllItems)
        {
            SendItemData(player, item); // 1000+ RPCs
        }
    }

    // ✅ GOOD: Send only changed data
    void SendChangedData(PlayerBase player)
    {
        foreach (EntityAI item : m_ChangedItems)
        {
            SendItemData(player, item); // Only changed items
        }

        m_ChangedItems.Clear();
    }

    // ✅ BETTER: Send on demand
    void SendOnRequest(PlayerBase player, string itemType)
    {
        EntityAI item = GetItem(itemType);
        if (item)
        {
            SendItemData(player, item); // Only requested item
        }
    }
}
```

---

## 🏗️ Entity Limits

### Maximum Entities

| Entity Type | Max Count | Reason |
|-------------|-----------|--------|
| Zombies | 500 | AI CPU cost |
| Items (loot) | 1000 | Memory |
| Buildings | 200 | Render performance |
| Vehicles | 50 | Physics cost |
| Players | 150 | Network bandwidth |

**Best Practice:** Respect limits, implement cleanup

---

### Loot Cleanup System

```c
class LootCleanupManager
{
    protected ref array<EntityAI> m_LootItems;
    protected const int MAX_LOOT = 1000;

    void CheckLootCount()
    {
        int currentCount = m_LootItems.Count();

        if (currentCount > MAX_LOOT)
        {
            Print("[LootCleanup] Too much loot! Removing oldest...");

            // Sort by spawn time
            m_LootItems.Sort(SortBySpawnTime);

            // Remove oldest
            for (int i = 0; i < currentCount - MAX_LOOT; i++)
            {
                GetGame().ObjectDelete(m_LootItems[i]);
            }

            m_LootItems.Resize(MAX_LOOT);
        }
    }

    void OnLootSpawned(EntityAI loot)
    {
        m_LootItems.Insert(loot);
        CheckLootCount();
    }
}
```

---

### Zombie Optimization

```c
class ZombieOptimizer
{
    protected const int MAX_ZOMBIES = 500;
    protected const int SPAWN_INTERVAL = 60; // seconds

    bool CanSpawnZombie()
    {
        // Check current count
        int currentCount = GetZombieCount();

        if (currentCount >= MAX_ZOMBIES)
        {
            Print("[Zombie] Max zombies reached!");
            return false;
        }

        // Check player count (more players = more zombies)
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        int maxZombiesForPlayers = players.Count() * 10;

        if (currentCount >= maxZombiesForPlayers)
        {
            return false;
        }

        return true;
    }
}
```

---

## ⏱️ Update Frequency

### Update Intervals

| System | Update Rate | Reason |
|--------|-------------|--------|
| Physics | Every frame | Accuracy required |
| AI | Every 0.5s | Balance CPU/accuracy |
| Weather | Every 5s | Slow change |
| Economy | Every 60s | Rarely changes |
| Storage | On disconnect | Persist only when needed |

**Best Practice:** Update only as frequently as needed

---

### Optimize Update Frequency

#### ❌ BAD: Update everything every frame

```c
modded class PlayerBase
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Updating EVERYTHING every frame - EXPENSIVE!
        UpdateEconomy(); // Changes rarely
        UpdateWeather(); // Changes slowly
        UpdateAI(); // Expensive
        UpdateStorage(); // Should only save on disconnect
    }
}
```

#### ✅ GOOD: Update at appropriate rates

```c
modded class PlayerBase
{
    protected float m_LastEconomyUpdate;
    protected float m_LastWeatherUpdate;
    protected float m_LastAIUpdate;

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        float currentTime = GetGame().GetTickTime();

        // Economy: Update every 60 seconds
        if (currentTime - m_LastEconomyUpdate > 60.0)
        {
            UpdateEconomy();
            m_LastEconomyUpdate = currentTime;
        }

        // Weather: Update every 5 seconds
        if (currentTime - m_LastWeatherUpdate > 5.0)
        {
            UpdateWeather();
            m_LastWeatherUpdate = currentTime;
        }

        // AI: Update every 0.5 seconds
        if (currentTime - m_LastAIUpdate > 0.5)
        {
            UpdateAI();
            m_LastAIUpdate = currentTime;
        }
    }
}
```

---

### Time-Based vs Event-Based

```c
class OptimizedManager
{
    // ❌ BAD: Polling (check every frame)
    void PollingUpdate()
    {
        if (Math.RandomFloat01() < 0.01) // 1% chance per frame
        {
            OnRandomEvent();
        }
    }

    // ✅ GOOD: Event-based (only when needed)
    void EventBasedUpdate()
    {
        // Register for event
        GetGame().GetCallQueue().CallLater(OnRandomEvent, Math.RandomInt(1000, 60000), false);
    }
}
```

---

## 🚫 Common Anti-Patterns

### Anti-Pattern 1: Expensive Loops

#### ❌ BAD: Nested loops over large arrays

```c
void CheckCollisions()
{
    // O(n²) - VERY EXPENSIVE
    foreach (EntityAI a : m_AllEntities)
    {
        foreach (EntityAI b : m_AllEntities)
        {
            if (a != b && IsColliding(a, b))
            {
                HandleCollision(a, b);
            }
        }
    }
}
```

#### ✅ GOOD: Spatial partitioning

```c
void CheckCollisionsOptimized()
{
    // O(n) - Much faster
    map<string, ref array<EntityAI>> zones = PartitionByZone(m_AllEntities);

    foreach (string zone, array<EntityAI> entities : zones)
    {
        // Only check entities in same zone
        for (int i = 0; i < entities.Count(); i++)
        {
            for (int j = i + 1; j < entities.Count(); j++)
            {
                if (IsColliding(entities[i], entities[j]))
                {
                    HandleCollision(entities[i], entities[j]);
                }
            }
        }
    }
}
```

---

### Anti-Pattern 2: Premature Optimization

```c
// ❌ BAD: Optimize before measuring
void OptimizedCode()
{
    // Complex optimization for rarely-called code
    // Wastes development time
}

// ✅ GOOD: Profile first, then optimize
void ProfiledCode()
{
    // Simple code
    // If profiling shows it's slow, THEN optimize
}
```

**Rule:** Measure first, optimize second

---

### Anti-Pattern 3: String Concatenation in Loops

#### ❌ BAD: Concatenate strings

```c
string BuildString(array<string> items)
{
    string result;

    // Creates new string every iteration - SLOW
    foreach (string item : items)
    {
        result = result + ", " + item;
    }

    return result;
}
```

#### ✅ GOOD: Use array, join at end

```c
string BuildStringOptimized(array<string> items)
{
    array<string> parts = new array<string>;

    // Just append to array - FAST
    foreach (string item : items)
    {
        parts.Insert(item);
    }

    // Join once at end
    return string.Join(", ", parts);
}
```

---

### Anti-Pattern 4: Unnecessary File I/O

#### ❌ BAD: Write to file every frame

```c
modded class PlayerBase
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Writing to disk EVERY frame - VERY SLOW!
        SaveData();
    }
}
```

#### ✅ GOOD: Write to file on disconnect

```c
modded class PlayerBase
{
    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Write once on disconnect - FAST
        SaveData();
    }
}
```

---

### Anti-Pattern 5: Creating Objects Every Frame

#### ❌ BAD: Create objects in loop

```c
modded class PlayerBase
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Creates new array EVERY frame - MEMORY LEAK!
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
    }
}
```

#### ✅ GOOD: Reuse objects

```c
modded class PlayerBase
{
    protected ref array<Man> m_Players;

    override void OnConnect()
    {
        super.OnConnect();

        // Create once
        m_Players = new array<Man>;
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Reuse array - NO ALLOCATION
        m_Players.Clear();
        GetGame().GetPlayers(m_Players);
    }
}
```

---

## 📈 Profiling

### Enable Script Profiling

```c
// In config.cpp
class CfgScripts
{
    class MyMod
    {
        // Enable profiling
        profilingEnabled = 1;
    };
};
```

### Profile Specific Functions

```c
class MyManager
{
    void ExpensiveFunction()
    {
        float startTime = GetGame().GetTickTime();

        // Do expensive work
        DoWork();

        float endTime = GetGame().GetTickTime();
        float duration = endTime - startTime;

        Print("[Profile] ExpensiveFunction took " + (duration * 1000) + " ms");

        if (duration > 0.005) // 5ms
        {
            Print("[Profile] WARNING: Function took too long!");
        }
    }
}
```

### Performance Benchmarks

| Operation | Target | Acceptable |
|-----------|--------|------------|
| Create entity | <1ms | <5ms |
| Save JSON | <10ms | <50ms |
| Load JSON | <10ms | <50ms |
| Find path (AI) | <5ms | <20ms |
| RPC to client | <1ms | <5ms |

---

## 🔍 Troubleshooting

### Symptom: Low Server FPS

**Possible causes:**
1. Too many entities spawned
2. Expensive loops in OnUpdate()
3. Excessive file I/O
4. Memory leaks

**Solutions:**
1. Reduce entity counts
2. Optimize update frequency
3. Batch file operations
4. Check for memory leaks

---

### Symptom: High Memory Usage

**Possible causes:**
1. Entities never deleted
2. Arrays never cleared
3. Timers never removed
4. Event handlers never unregistered

**Solutions:**
1. Implement entity cleanup
2. Clear arrays regularly
3. Remove timers when done
4. Unregister events

---

### Symptom: Network Lag

**Possible causes:**
1. Too many RPC calls
2. Large RPC payloads
3. Broadcasts to all players
4. No client-side prediction

**Solutions:**
1. Batch RPCs
2. Compress data
3. Target specific players
4. Implement prediction

---

## 📚 Best Practices Summary

### DO ✅

- Reuse objects
- Update at appropriate frequency
- Batch RPCs
- Clean up entities
- Profile before optimizing
- Use event-driven updates
- Implement spatial partitioning

### DON'T ❌

- Create objects every frame
- Update everything every frame
- Send RPC every frame
- Ignore memory leaks
- Prematurely optimize
- Poll for events
- Use nested loops over large arrays

---

## 🔗 Related Topics

→ [EntityAI API](ENTITYAI_COMPLETE_API.md) - Entity lifecycle
→ [PlayerBase API](PLAYERBASE_COMPLETE_API.md) - Player updates
→ [Network](../topics/08_network.md) - RPC communication
→ [Troubleshooting](../troubleshooting/performance.md) - Common issues

---

*Last updated: 2025-01-18*
*Target: Mod developers, server admins*
