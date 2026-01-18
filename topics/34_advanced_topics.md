# Advanced Topics - Complete Overview

**Status:** ✅ Advanced Guide
**Difficulty:** Advanced
**Related:** [Integration Patterns](../reference/INTEGRATION_PATTERNS.md) | [Best Practices](../reference/BEST_PRACTICES.md)

---

## ⚡ 30-Second Summary

Advanced Topics cover cutting-edge DayZ modding techniques that combine multiple systems. These are the techniques used by the most popular and complex mods.

---

## 🎯 What You'll Learn

### Category 1: System Integration
- **Event-Driven Architecture** - Cross-system communication
- **Service Locator Pattern** - Dependency management
- **RPC Bridges** - Client-server coordination
- **Shared Storage** - Unified persistence

### Category 2: Performance Optimization
- **Entity Pooling** - Reduce memory allocation
- **Timer Management** - Optimize update loops
- **Network Optimization** - Reduce bandwidth
- **JSON Optimization** - Faster serialization

### Category 3: Security
- **Anti-Cheat Patterns** - Prevent exploits
- **Input Validation** - Server-side authority
- **Rate Limiting** - Prevent spam
- **Data Integrity** - Checksums and validation

### Category 4: Advanced Features
- **Procedural Generation** - Dynamic content
- **AI Behavior Trees** - Complex AI
- **Dynamic UI** - Responsive interfaces
- **Save/Load Systems** - Complex persistence

---

## 📁 Topics Overview

### Completed Topics (Basic → Advanced)

**Beginner (Topics 1-10):**
1. ✅ Architecture
2. ✅ Config
3. ✅ EnScript Basics
4. ✅ Compiling
5. ✅ Classes & Objects
6. ✅ Crafting
7. ✅ Items
8. ✅ Actions
9. ✅ Events
10. ✅ Debugging

**Intermediate (Topics 11-25):**
11. ✅ Advanced Modding
12. ✅ XP Systems
13. ✅ Teleportation
14. ✅ Storage
15. ✅ Zombie AI
16. ✅ GUI Systems
17. ✅ Mission System
18. ✅ Sound System
19. ✅ Vehicle System
20. ✅ Faction Systems
21. ✅ Loot Boxes
22. ✅ Time Events
23. ✅ Notifications
24. ✅ Building Systems
25. ✅ GUI Crafting

**Advanced (Topics 26-33):**
26. ✅ Deployment
27. ✅ Trader Systems
28. ✅ Quest Systems
29. ✅ Auction Systems
30. ✅ ATM Systems
31. ✅ CF RPC
32. ✅ Mail Systems
33. ✅ Central Economy

**Expert (Topics 34-36):**
34. ✅ **Advanced Topics** (this file)
35. ⚠️ **Procedural Generation** (TODO)
36. ⚠️ **AI Behavior Trees** (TODO)

---

## 🔧 Topic 1: Event-Driven Architecture

**Reference:** [Integration Patterns](../reference/INTEGRATION_PATTERNS.md#pattern-1-event-driven-integration)

**Concept:** Systems communicate through events without direct coupling.

**Use Cases:**
- XP System listens for kill events
- Quest System listens for collection events
- Notification System listens for achievement events
- Multiple systems respond to one event

**Example:**
```c
// One kill event → Multiple systems respond
void OnPlayerKilled(PlayerBase killer, PlayerBase victim)
{
    map<string, string> data = new map<string, string>;
    data.Set("killerID", killer.GetID());
    data.Set("victimID", victim.GetID());

    // Trigger event
    GetEventBus().Trigger("OnPlayerKilled", data);
    // → XP System adds XP
    // → Quest System updates objectives
    // → Faction System updates reputation
    // → Notification System notifies
}
```

**Benefits:**
- ✅ Loose coupling
- ✅ Easy to extend
- ✅ One event → Multiple actions

---

## 🔧 Topic 2: Shared Storage Integration

**Reference:** [Integration Patterns](../reference/INTEGRATION_PATTERNS.md#pattern-2-shared-storage-integration)

**Concept:** Multiple systems store data in ONE JSON file for cross-server persistence.

**Use Cases:**
- Player XP persists across servers
- Bank balance shared across servers
- Quest progress saved globally
- Faction membership persistent

**Example:**
```c
// Single JSON file contains ALL player data
{
  "Players": {
    "player123": {
      "Level": 10,
      "CurrentXP": 5000,
      "BankBalance": 10000,
      "FactionID": "Traders",
      "CompletedQuests": ["quest1", "quest2"],
      "LastPosition": [1234, 100, 5678]
    }
  }
}
```

**Benefits:**
- ✅ Cross-server persistence
- ✅ Atomic updates
- ✅ Single backup point

---

## 🔧 Topic 3: Performance Optimization

**Reference:** [Performance Guide](../reference/PERFORMANCE_GUIDE.md)

**Key Techniques:**

### 3.1 Entity Pooling
```c
// Reuse entities instead of creating/destroying
class EntityPool
{
    protected array<Entity> m_Pool;

    Entity GetEntity()
    {
        if (m_Pool.Count() > 0)
            return m_Pool.Remove(0);  // Reuse
        else
            return Entity.Cast(GetGame().CreateObject("MyEntity", "0 0 0"));  // Create new
    }

    void ReturnEntity(Entity entity)
    {
        entity.SetPosition("0 0 0");  // Reset
        m_Pool.Insert(entity);
    }
}
```

### 3.2 Timer Management
```c
// BAD: Create timer every frame
void Update()
{
    new Timer();  // Memory leak!
}

// GOOD: Reuse timer
protected Timer m_UpdateTimer;

void Update()
{
    if (!m_UpdateTimer)
        m_UpdateTimer = new Timer();

    m_UpdateTimer.Run(1, this, "Update", NULL, true);
}
```

### 3.3 Network Optimization
```c
// BAD: Send RPC every second
void Update()
{
    GetRPCManager().SendRPC("MyMod", "Sync", data, true);  // Spam!
}

// GOOD: Send on change
void OnValueChanged()
{
    GetRPCManager().SendRPC("MyMod", "Sync", data, true);  // Only when changed
}
```

---

## 🔧 Topic 4: Security Best Practices

**Reference:** [Security Guide](../reference/SECURITY_GUIDE.md)

**Key Principles:**

### 4.1 Never Trust Client
```c
// BAD: Trust client value
void AddXP(string playerID, int amount)
{
    m_XP[playerID] += amount;  // Client sends amount = 999999!
}

// GOOD: Validate on server
void AddXP(string playerID, int amount)
{
    if (amount > 1000)  // Max reasonable amount
        return;

    if (!GetGame().IsServer())  // Must be server
        return;

    m_XP[playerID] += amount;
}
```

### 4.2 Server-Side Authority
```c
// All critical operations on server
void Withdraw(PlayerBase player, int amount)
{
    if (!GetGame().IsServer())
    {
        GetRPCManager().SendRPC("Bank", "Withdraw", new Param1<int>(amount), true);
        return;
    }

    // Server-side validation
    if (amount > m_Balance[player.GetID()])
        return;

    m_Balance[player.GetID()] -= amount;
}
```

### 4.3 Rate Limiting
```c
class RateLimiter
{
    protected map<string, int> m_LastActionTime;

    bool CanAct(string playerID, int cooldownSeconds)
    {
        int now = GetGame().GetTickTime();

        if (m_LastActionTime.Contains(playerID))
        {
            if (now - m_LastActionTime[playerID] < cooldownSeconds)
                return false;  // Cooldown active
        }

        m_LastActionTime.Set(playerID, now);
        return true;
    }
}
```

---

## 🔧 Topic 5: Procedural Generation

**Concept:** Generate content dynamically instead of static placement.

**Use Cases:**
- Random loot placement
- Dynamic zombie spawns
- Procedural buildings
- Random quest locations

**Example:**
```c
class ProceduralLootSpawner
{
    void SpawnLootInArea(vector center, float radius, int count)
    {
        for (int i = 0; i < count; i++)
        {
            // Random position
            float angle = Math.RandomFloat01() * Math.PI2;
            float distance = Math.RandomFloat01() * radius;
            vector pos = center + Vector(Math.Cos(angle) * distance, 0, Math.Sin(angle) * distance);

            // Random loot
            array<string> lootTypes = {"AK74", "M4A1", "Mosin", "SKS"};
            string lootType = lootTypes[Math.RandomInt(0, lootTypes.Count())];

            // Spawn
            GetGame().CreateObject(lootType, pos);
        }
    }
}
```

**Benefits:**
- ✅ Dynamic content
- ✅ Replayability
- ✅ Reduced storage

---

## 🔧 Topic 6: AI Behavior Trees

**Concept:** Complex AI decisions through tree structures.

**Use Cases:**
- Advanced zombie behavior
- NPC decision making
- Boss fight mechanics
- Companion AI

**Example:**
```c
class BehaviorNode
{
    bool Execute(AIAgent agent) { return false; }
}

class SequenceNode: BehaviorNode
{
    protected ref array<ref BehaviorNode> m_Children;

    override bool Execute(AIAgent agent)
    {
        foreach (BehaviorNode child : m_Children)
        {
            if (!child.Execute(agent))
                return false;  // Fail fast
        }
        return true;  // All children succeeded
    }
}

class SelectorNode: BehaviorNode
{
    protected ref array<ref BehaviorNode> m_Children;

    override bool Execute(AIAgent agent)
    {
        foreach (BehaviorNode child : m_Children)
        {
            if (child.Execute(agent))
                return true;  // Success
        }
        return false;  // All children failed
    }
}

// Usage: Zombie AI
BehaviorNode zombieAI = new SelectorNode();
zombieAI.m_Children.Insert(new AttackNode());  // Try attack
zombieAI.m_Children.Insert(new ChaseNode());   // Try chase
zombieAI.m_Children.Insert(new WanderNode());  // Try wander
```

---

## 🔧 Topic 7: Dynamic UI Systems

**Reference:** [UI Framework](../reference/UI_FRAMEWORK.md)

**Concept:** UI that updates based on game state.

**Use Cases:**
- Health bars
- XP progress
- Quest trackers
- Inventory grids

**Example:**
```c
class DynamicHealthBar: UIScriptedMenu
{
    protected Widget m_HealthBar;
    protected TextWidget m_HealthText;

    override void Update()
    {
        PlayerBase player = GetGame().GetPlayer();
        float health = player.GetHealth("GlobalHealth", "Health");

        // Update bar width
        m_HealthBar.SetSize(health * 200, 20);

        // Update text
        m_HealthText.SetText(health.ToString() + "%");
    }
}
```

---

## 🔧 Topic 8: Advanced Save/Load

**Reference:** [Storage System](../reference/STORAGE_SYSTEMS.md)

**Concept:** Complex persistence with versioning, migration, validation.

**Use Cases:**
- Large databases
- Version compatibility
- Data migration
- Corruption recovery

**Example:**
```c
class AdvancedStorage
{
    const int VERSION = 2;

    void Save()
    {
        string json = JsonSerializeToString(m_Data);
        json = "{\"Version\":" + VERSION + ",\"Data\":" + json + "}";

        // Add checksum
        string checksum = CalculateChecksum(json);
        json = "{\"Checksum\":\"" + checksum + "\",\"Content\":" + json + "}";

        File file = OpenFile("$profile:data.json", FileMode.WRITE);
        file.WriteString(json);
        file.Close();
    }

    bool Load()
    {
        string json;
        File file = OpenFile("$profile:data.json", FileMode.READ);
        file.ReadString(json);
        file.Close();

        // Verify checksum
        if (!VerifyChecksum(json))
        {
            Print("[Storage] Checksum failed! Data corrupted.");
            return false;
        }

        // Parse JSON
        map data = JsonDeserialize(json);

        // Check version
        int version = data.Get("Version");
        if (version < VERSION)
        {
            MigrateData(version, VERSION);
        }

        return true;
    }

    void MigrateData(int fromVersion, int toVersion)
    {
        if (fromVersion == 1 && toVersion == 2)
        {
            // Add new field
            foreach (PlayerData data : m_Data.Players)
            {
                data.NewField = "default";
            }
        }
    }
}
```

---

## 📋 Advanced Topic Checklist

### System Integration
- ✅ Event-Driven Architecture
- ✅ Shared Storage
- ✅ RPC Bridges
- ✅ Service Locator
- ✅ Chain of Responsibility

### Performance
- ✅ Entity Pooling
- ✅ Timer Management
- ✅ Network Optimization
- ✅ JSON Optimization
- ✅ Memory Management

### Security
- ✅ Input Validation
- ✅ Server-Side Authority
- ✅ Rate Limiting
- ✅ Data Integrity
- ✅ Anti-Cheat

### Advanced Features
- ✅ Procedural Generation
- ⚠️ AI Behavior Trees (basic coverage)
- ✅ Dynamic UI
- ✅ Advanced Save/Load
- ⚠️ Multiplayer Sync (covered in CF_RPC)

---

## 🎯 Learning Path

### Step 1: Master Integration Patterns
**Time:** 2-3 days
**Files:**
- [Integration Patterns](../reference/INTEGRATION_PATTERNS.md)
- [Integration Examples](../examples/integration/)

**Goal:** Understand how to combine systems

### Step 2: Optimize Performance
**Time:** 2-3 days
**Files:**
- [Performance Guide](../reference/PERFORMANCE_GUIDE.md)
- [Best Practices](../reference/BEST_PRACTICES.md)

**Goal:** Write efficient code

### Step 3: Implement Security
**Time:** 2-3 days
**Files:**
- [Security Guide](../reference/SECURITY_GUIDE.md)
- [Best Practices](../reference/BEST_PRACTICES.md)

**Goal:** Prevent exploits

### Step 4: Build Complex Features
**Time:** 5-7 days
**Projects:**
- Economy System (ATM + Banking + Trading + Storage)
- RPG System (Quests + XP + Factions + Notifications)
- Base Building (Building + Permissions + Storage + Safe Zones)

**Goal:** Create production-ready mods

---

## 📝 Quick Reference

### Integration Pattern Selection

| Pattern | Use Case | File |
|---------|----------|------|
| Event-Driven | Broadcast events | [INTEGRATION_PATTERNS.md#Pattern-1](../reference/INTEGRATION_PATTERNS.md#pattern-1-event-driven-integration) |
| Shared Storage | Cross-server persistence | [INTEGRATION_PATTERNS.md#Pattern-2](../reference/INTEGRATION_PATTERNS.md#pattern-2-shared-storage-integration) |
| RPC Bridge | Client-server comms | [INTEGRATION_PATTERNS.md#Pattern-3](../reference/INTEGRATION_PATTERNS.md#pattern-3-rpc-bridge-integration) |
| Dependency Injection | Explicit dependencies | [INTEGRATION_PATTERNS.md#Pattern-4](../reference/INTEGRATION_PATTERNS.md#pattern-4-dependency-injection-integration) |
| Chain of Responsibility | Sequential processing | [INTEGRATION_PATTERNS.md#Pattern-5](../reference/INTEGRATION_PATTERNS.md#pattern-5-chain-of-responsibility-integration) |

### Performance Guidelines

| Issue | Solution | File |
|-------|----------|------|
| Memory leaks | Entity pooling, timer cleanup | [PERFORMANCE_GUIDE.md#Memory-Management](../reference/PERFORMANCE_GUIDE.md#memory-management) |
| Network lag | Reduce RPC frequency | [PERFORMANCE_GUIDE.md#Network-Optimization](../reference/PERFORMANCE_GUIDE.md#network-optimization) |
| Slow JSON | Optimize structure | [PERFORMANCE_GUIDE.md#JSON-Optimization](../reference/PERFORMANCE_GUIDE.md#json-optimization) |

### Security Checklist

| Threat | Protection | File |
|--------|-----------|------|
| Client spoofing | Server-side validation | [SECURITY_GUIDE.md#Input-Validation](../reference/SECURITY_GUIDE.md#input-validation) |
| RPC spam | Rate limiting | [SECURITY_GUIDE.md#Rate-Limiting](../reference/SECURITY_GUIDE.md#rate-limiting) |
| Data corruption | Checksums | [SECURITY_GUIDE.md#Data-Integrity](../reference/SECURITY_GUIDE.md#data-integrity) |

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** [Procedural Generation](35_procedural_generation.md) (TODO)
