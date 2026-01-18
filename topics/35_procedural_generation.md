# Procedural Generation in DayZ - Complete Guide

**Status:** ✅ Advanced Guide
**Difficulty:** Advanced
**Time to Learn:** 90 minutes
**Related:** [AI System](../reference/AI_SYSTEM.md) | [Building Systems](24_building_systems.md)

---

## ⚡ 30-Second Summary

Procedural generation creates game content **dynamically** instead of placing it manually. In DayZ, this means generating loot, zombies, buildings, events, and more using algorithms rather than hand-placing everything in the editor.

---

## 🎯 What You'll Learn

1. **What is Procedural Generation?** - Concepts and benefits
2. **Random Number Generation** - Math.RandomFloat01(), seeds
3. **Loot Generation** - Dynamic loot spawning
4. **Zombie Spawning** - Procedural AI placement
5. **Building Generation** - Dynamic base creation
6. **Event Generation** - Random events (crashes, airdrops)
7. **Performance Optimization** - Efficient generation

---

## 💡 What is Procedural Generation?

### Definition

**Procedural Generation** = Creating content algorithmically instead of manually

**Manual Placement:**
```
Map Editor → Place zombie at (1000, 100, 2000)
Map Editor → Place zombie at (1005, 100, 2005)
Map Editor → Place zombie at (1010, 100, 2010)
... (repeat 1000 times)
```

**Procedural Generation:**
```c
for (int i = 0; i < 1000; i++)
{
    vector pos = GetRandomPosition();
    SpawnZombie(pos);
}
```

### Benefits

**Why use procedural generation?**

✅ **Replayability** - Every playthrough is different
✅ **Time-saving** - Don't place everything manually
✅ **Dynamic** - Content changes based on conditions
✅ **Scalable** - Easy to add more content
✅ **Storage-efficient** - Don't store every entity position

### Drawbacks

❌ **Less control** - Harder to craft specific experiences
❌ **Complexity** - More code to write
❌ **Performance** - Can be expensive if not optimized
❌ **Predictability** - Harder to test

---

## 🔧 Random Number Generation

### Basic Random Numbers

**Math.RandomFloat01()** - Random float between 0.0 and 1.0:
```c
float r = Math.RandomFloat01();  // 0.0 ≤ r < 1.0

// Example: 50% chance
if (Math.RandomFloat01() < 0.5)
{
    Print("Heads!");
}
else
{
    Print("Tails!");
}
```

**Math.RandomIntInclusive(min, max)** - Random integer between min and max:
```c
int r = Math.RandomIntInclusive(1, 10);  // 1 to 10 inclusive

Print(r);  // Could be 1, 2, 3, ..., 10
```

---

### Random Vectors (Positions)

**Random Position in Circle:**
```c
vector GetRandomPositionInCircle(vector center, float radius)
{
    float angle = Math.RandomFloat01() * Math.PI2;
    float distance = Math.RandomFloat01() * radius;

    float x = center[0] + Math.Cos(angle) * distance;
    float z = center[2] + Math.Sin(angle) * distance;

    return Vector(x, center[1], z);
}

// Usage:
vector center = Vector(5000, 0, 5000);
float radius = 100;
vector pos = GetRandomPositionInCircle(center, radius);
```

**Random Position in Square:**
```c
vector GetRandomPositionInSquare(vector center, float size)
{
    float halfSize = size / 2;

    float x = center[0] + Math.RandomFloatInclusive(-halfSize, halfSize);
    float z = center[2] + Math.RandomFloatInclusive(-halfSize, halfSize);

    return Vector(x, center[1], z);
}
```

---

### Seeded Random (Reproducible)

**Problem:** `Math.RandomFloat01()` is different every time

**Solution:** Use seeds for reproducibility

```c
class SeededRandom
{
    protected int m_Seed;

    void SeededRandom(int seed)
    {
        m_Seed = seed;
    }

    float RandomFloat01()
    {
        m_Seed = m_Seed * 1103515245 + 12345;
        return (m_Seed & 0x7FFFFFFF) / 2147483647.0;
    }
}

// Usage:
SeededRandom rng = new SeededRandom(12345);  // Same seed = same sequence
float r1 = rng.RandomFloat01();  // Always 0.1234...
float r2 = rng.RandomFloat01();  // Always 0.5678...
```

---

## 🎁 Loot Generation

### Basic Loot Spawning

**Spawn Random Loot in Area:**
```c
class LootSpawner
{
    void SpawnLootInArea(vector center, float radius, int count)
    {
        for (int i = 0; i < count; i++)
        {
            // Get random position
            vector pos = GetRandomPositionInCircle(center, radius);

            // Get random loot type
            string lootType = GetRandomLootType();

            // Spawn
            EntityAI loot = GetGame().CreateObject(lootType, pos);

            // Random rotation
            loot.SetOrientation(Vector(0, Math.RandomFloatInclusive(0, 360), 0));
        }
    }

    string GetRandomLootType()
    {
        array<string> lootTypes = {
            "AK74",
            "M4A1",
            "Mosin",
            "SodaCan_Cola",
            "Apple",
            "Rice"
        };

        int index = Math.RandomInt(0, lootTypes.Count() - 1);
        return lootTypes[index];
    }
}
```

---

### Weighted Loot Tables

**Problem:** Not all loot should be equally likely

**Solution:** Use weights

```c
class WeightedLootTable
{
    protected ref array<ref LootEntry> m_LootEntries;

    void AddLoot(string type, float weight)
    {
        LootEntry entry = new LootEntry();
        entry.Type = type;
        entry.Weight = weight;
        m_LootEntries.Insert(entry);
    }

    string GetRandomLoot()
    {
        // Calculate total weight
        float totalWeight = 0;
        foreach (LootEntry entry : m_LootEntries)
        {
            totalWeight += entry.Weight;
        }

        // Get random value
        float r = Math.RandomFloat01() * totalWeight;

        // Find matching loot
        float currentWeight = 0;
        foreach (LootEntry entry : m_LootEntries)
        {
            currentWeight += entry.Weight;
            if (r <= currentWeight)
                return entry.Type;
        }

        return m_LootEntries[m_LootEntries.Count() - 1].Type;
    }
}

class LootEntry
{
    string Type;
    float Weight;
}

// Usage:
WeightedLootTable table = new WeightedLootTable();
table.AddLoot("AK74", 10.0);      // 10% chance
table.AddLoot("M4A1", 5.0);       // 5% chance
table.AddLoot("SodaCan_Cola", 50.0);  // 50% chance
table.AddLoot("Apple", 35.0);     // 35% chance
// Total: 100%
```

---

### Tiered Loot System

**Common → Uncommon → Rare → Legendary:**

```c
class TieredLootSystem
{
    string GetRandomLoot()
    {
        float r = Math.RandomFloat01() * 100;

        // 60% common
        if (r < 60)
            return GetCommonLoot();

        // 30% uncommon
        if (r < 90)
            return GetUncommonLoot();

        // 9% rare
        if (r < 99)
            return GetRareLoot();

        // 1% legendary
        return GetLegendaryLoot();
    }

    string GetCommonLoot()
    {
        array<string> commons = {"SodaCan_Cola", "Apple", "Rice"};
        return commons.Get(Math.RandomInt(0, commons.Count() - 1));
    }

    string GetUncommonLoot()
    {
        array<string> uncommons = {"Mag_AKM_30Rnd", "Battery9V", "Compass"};
        return uncommons.Get(Math.RandomInt(0, uncommons.Count() - 1));
    }

    string GetRareLoot()
    {
        array<string> rares = {"AK74", "M4A1", "Mosin"};
        return rares.Get(Math.RandomInt(0, rares.Count() - 1));
    }

    string GetLegendaryLoot()
    {
        array<string> legendaries = {"M4A1_Optical", "SVD", "VSS"};
        return legendaries.Get(Math.RandomInt(0, legendaries.Count() - 1));
    }
}
```

---

### Dynamic Loot Based on Location

**Different loot in different areas:**

```c
string GetLootForLocation(vector pos)
{
    // Get terrain type at position
    string terrainType = GetTerrainType(pos);

    // City = military loot
    if (terrainType == "City")
    {
        return GetMilitaryLoot();
    }
    // Forest = hunting/survival loot
    else if (terrainType == "Forest")
    {
        return GetSurvivalLoot();
    }
    // Coast = basic loot
    else if (terrainType == "Coast")
    {
        return GetBasicLoot();
    }

    return GetBasicLoot();
}
```

---

## 🧟 Zombie Spawning

### Basic Zombie Spawning

**Spawn Zombies Randomly:**
```c
class ZombieSpawner
{
    void SpawnZombiesInArea(vector center, float radius, int count)
    {
        for (int i = 0; i < count; i++)
        {
            // Get random position
            vector pos = GetRandomPositionInCircle(center, radius);

            // Get random zombie type
            string zombieType = GetRandomZombieType();

            // Spawn
            ZombieBase zombie = ZombieBase.Cast(GetGame().CreateObject(zombieType, pos));

            // Random rotation
            zombie.SetOrientation(Vector(0, Math.RandomFloatInclusive(0, 360), 0));
        }
    }

    string GetRandomZombieType()
    {
        array<string> zombieTypes = {
            "ZombieMale",
            "ZombieFemale",
            "ZombieBoater"
        };

        return zombieTypes.Get(Math.RandomInt(0, zombieTypes.Count() - 1));
    }
}
```

---

### Zombie Difficulty Scaling

**Harder zombies away from coast:**
```c
string GetZombieTypeForLocation(vector pos)
{
    // Distance from coast (0 to 1000+ meters)
    float distance = GetDistanceFromCoast(pos);

    // Near coast = basic zombies
    if (distance < 200)
    {
        return "ZombieMale";  // Easy
    }
    // Mid-range = medium zombies
    else if (distance < 500)
    {
        if (Math.RandomFloat01() < 0.5)
            return "ZombieMale";
        else
            return "ZombieFemale";  // Medium
    }
    // Far from coast = hard zombies
    else
    {
        if (Math.RandomFloat01() < 0.3)
            return "ZombieMale";
        else if (Math.RandomFloat01() < 0.6)
            return "ZombieFemale";
        else
            return "ZombieBoater";  // Hard
    }
}
```

---

## 🏗️ Building Generation

### Simple Building Placement

**Place buildings procedurally:**
```c
class BuildingGenerator
{
    void GenerateBuildingsInArea(vector center, float radius, int count)
    {
        for (int i = 0; i < count; i++)
        {
            // Get random position
            vector pos = GetRandomPositionInCircle(center, radius);

            // Get random building type
            string buildingType = GetRandomBuildingType();

            // Spawn
            Object building = GetGame().CreateObject(buildingType, pos);

            // Random rotation
            float rotation = Math.RandomFloatInclusive(0, 360);
            building.SetOrientation(Vector(0, rotation, 0));
        }
    }

    string GetRandomBuildingType()
    {
        array<string> buildingTypes = {
            "Land_City_Gate",
            "Land_City_House",
            "Land_Village_House"
        };

        return buildingTypes.Get(Math.RandomInt(0, buildingTypes.Count() - 1));
    }
}
```

---

### Procedural Base Building

**Generate a base with walls:**
```c
class BaseGenerator
{
    void GenerateBase(vector center, float size, int wallCount)
    {
        // Place walls in circle
        for (int i = 0; i < wallCount; i++)
        {
            float angle = (Math.PI2 / wallCount) * i;
            float x = center[0] + Math.Cos(angle) * size;
            float z = center[2] + Math.Sin(angle) * size;

            vector wallPos = Vector(x, center[1], z);
            vector wallRot = Vector(0, Math.RandomFloat01() * 360, 0);

            Object wall = GetGame().CreateObject("Land_Wall", wallPos);
            wall.SetOrientation(wallRot);
        }

        // Place gate
        vector gatePos = Vector(center[0] + size, center[1], center[2]);
        Object gate = GetGame().CreateObject("Land_Gate", gatePos);
        gate.SetOrientation(Vector(0, 90, 0));
    }
}
```

---

## 🎪 Event Generation

### Helicopter Crash Sites

**Generate crash sites procedurally:**
```c
class EventGenerator
{
    void GenerateHelicopterCrash(vector center, float radius)
    {
        // Get random position
        vector crashPos = GetRandomPositionInCircle(center, radius);

        // Spawn helicopter wreck
        Object heli = GetGame().CreateObject("Land_MH1_Wreck", crashPos);
        heli.SetOrientation(Vector(0, Math.RandomFloatInclusive(0, 360), 0));

        // Spawn loot around heli
        SpawnLootInArea(crashPos, 5, 10);  // 10 items within 5 meters

        // Spawn fire
        Object fire = GetGame().CreateObject("FirePlace", crashPos);

        // Spawn zombies
        SpawnZombiesInArea(crashPos, 10, 3);  // 3 zombies within 10 meters
    }
}
```

---

### Airdrop Events

**Generate airdrops:**
```c
void GenerateAirdrop(vector targetPos)
{
    // Find safe drop position (100m in air)
    vector dropPos = Vector(targetPos[0], targetPos[1] + 100, targetPos[2]);

    // Spawn cargo container
    Object cargo = GetGame().CreateObject("SeaCargoContainer", dropPos);

    // Spawn high-tier loot
    SpawnLootInArea(dropPos, 2, 5);  // 5 items within 2 meters

    // Create smoke signal
    Object smoke = GetGame().CreateObject("SmokeGrenade", dropPos);
}
```

---

## ⚡ Performance Optimization

### Don't Generate Every Frame

**❌ BAD:**
```c
override void EOnFrame(IEntity other, float timeSlice)
{
    // Generates every frame = LAG!
    SpawnZombiesInArea(center, 100, 10);
}
```

**✅ GOOD:**
```c
protected float m_LastSpawnTime;

override void EOnFrame(IEntity other, float timeSlice)
{
    float now = GetGame().GetTickTime();

    // Spawn every 60 seconds
    if (now - m_LastSpawnTime > 60)
    {
        SpawnZombiesInArea(center, 100, 10);
        m_LastSpawnTime = now;
    }
}
```

---

### Limit Entity Count

**❌ BAD:**
```c
// Spawns unlimited zombies
for (int i = 0; i < 999999; i++)
{
    SpawnZombie();
}
```

**✅ GOOD:**
```c
// Limit to max 100 zombies
const int MAX_ZOMBIES = 100;

int currentZombies = GetZombieCount();
if (currentZombies < MAX_ZOMBIES)
{
    int toSpawn = MAX_ZOMBIES - currentZombies;
    SpawnZombies(toSpawn);
}
```

---

### Use Object Pooling

**Reuse entities instead of creating/destroying:**
```c
class ZombiePool
{
    protected array<ZombieBase> m_Pool;

    ZombieBase GetZombie()
    {
        if (m_Pool.Count() > 0)
            return m_Pool.Remove(0);  // Reuse
        else
            return ZombieBase.Cast(GetGame().CreateObject("ZombieMale", "0 0 0"));  // Create new
    }

    void ReturnZombie(ZombieBase zombie)
    {
        zombie.SetPosition("0 0 0");  // Reset
        zombie.SetHealth(1.0);  // Reset health
        m_Pool.Insert(zombie);
    }
}
```

---

## 📝 Quick Reference

### Random Numbers

| Function | Range | Usage |
|----------|-------|------|
| `Math.RandomFloat01()` | 0.0 to 1.0 | Percentages |
| `Math.RandomIntInclusive(min, max)` | min to max | Integers |
| `Math.RandomFloatInclusive(min, max)` | min to max | Floats |

### Procedural Generation Types

| Type | Example | Complexity |
|------|---------|------------|
| Loot | Random loot spawning | Easy |
| Zombies | Random zombie spawning | Easy |
| Buildings | Random building placement | Medium |
| Events | Crash sites, airdrops | Medium |
| Bases | Procedural base building | Hard |
| Terrain | Heightmap generation | Very Hard |

---

## ⚠️ Common Mistakes

### ❌ Mistake 1: Generating Every Frame

```c
// WRONG: Generates every frame = LAG
override void EOnFrame(IEntity other, float timeSlice)
{
    SpawnZombies();
}

// CORRECT: Use timer
override void EOnFrame(IEntity other, float timeSlice)
{
    if (GetGame().GetTickTime() - m_LastTime > 60)
        SpawnZombies();
}
```

### ❌ Mistake 2: No Limits

```c
// WRONG: Infinite generation
while (true) { SpawnZombie(); }

// CORRECT: Limit count
for (int i = 0; i < 100; i++) { SpawnZombie(); }
```

### ❌ Mistake 3: Spawning Underground

```c
// WRONG: No height check
vector pos = Vector(x, 0, z);  // Might be underground

// CORRECT: Check surface height
float surfaceY = GetGame().SurfaceY(x, z);
vector pos = Vector(x, surfaceY, z);
```

---

## 🎓 Next Steps

**After learning procedural generation:**
1. **Read:** [AI System Reference](../reference/AI_SYSTEM.md) - Advanced AI
2. **Read:** [Building Systems](24_building_systems.md) - Base building
3. **Example:** Create custom zombie spawner
4. **Example:** Create dynamic loot system

---

## 💡 Key Takeaways

1. **Procedural Generation = Dynamic Content**
   - Random numbers (Math.RandomFloat01())
   - Algorithms (weighted tables, tiers)
   - Dynamic (changes each playthrough)

2. **Common Uses:**
   - Loot spawning
   - Zombie spawning
   - Building placement
   - Event generation

3. **Performance:**
   - Don't generate every frame
   - Limit entity count
   - Use object pooling

4. **Best Practices:**
   - Use weights for loot tables
   - Scale difficulty by location
   - Validate positions (don't spawn underground)

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** [AI Behavior Trees](36_ai_behavior_trees.md)
