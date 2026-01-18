# Integration Patterns - Complete Reference

**Status:** ✅ Complete Reference
**Complexity:** Advanced
**Related:** [System Architecture](../../topics/01_architecture.md) | [Best Practices](BEST_PRACTICES.md)

---

## ⚡ 30-Second Summary

Integration Patterns show how to COMBINE multiple DayZ systems together to create complex features. This is critical for advanced modding - most real-world features require 2-5 systems working together.

---

## 🎯 Why Integration Matters

**Single System Examples:**
- ✅ ATM System (banking only)
- ✅ Quest System (quests only)
- ✅ Trader System (trading only)

**Real-World Features Need Integration:**
- 💰 **Economy** = ATM + Trading + Banking + Storage + Central Economy
- 🎯 **RPG Server** = Quests + XP + Factions + Teleportation + Notifications
- 🏠 **Base Building** = Building + Permissions + Storage + Safe Zones + AI
- 🚗 **Vehicle System** = Vehicles + Locking + Storage + Keys + Traders

**Without Integration:**
- ❌ Systems exist in isolation
- ❌ No data sharing between systems
- ❌ Limited functionality
- ❌ Poor user experience

**With Integration:**
- ✅ Systems work together
- ✅ Shared data and events
- ✅ Complex features
- ✅ Seamless UX

---

## 🔧 Pattern 1: Event-Driven Integration

### Concept

Systems communicate through EVENTS without direct coupling.

### Implementation

```c
// Event bus for cross-system communication
class EventBus
{
    protected ref map<string, ref array<ref EventHandler>> m_Listeners;

    void Register(string eventName, EventHandler handler)
    {
        if (!m_Listeners.Contains(eventName))
        {
            m_Listeners.Set(eventName, new array<ref EventHandler>);
        }
        m_Listeners.Get(eventName).Insert(handler);
    }

    void Trigger(string eventName, map<string, string> data)
    {
        if (!m_Listeners.Contains(eventName))
            return;

        foreach (EventHandler handler : m_Listeners.Get(eventName))
        {
            handler.Handle(data);
        }
    }
}

// Base event handler
class EventHandler
{
    void Handle(map<string, string> data) {}
}

// Usage: XP System listens for kill events
class KillEventHandler: EventHandler
{
    override void Handle(map<string, string> data)
    {
        string killerID = data.Get("killerID");
        string victimID = data.Get("victimID");

        // Award XP
        GetXPManager().AddXP(killerID, 100, "Kill");
    }
}

// Usage: Quest System listens for kill events
class QuestKillEventHandler: EventHandler
{
    override void Handle(map<string, string> data)
    {
        string killerID = data.Get("killerID");
        string victimID = data.Get("victimID");

        // Update quest objectives
        GetQuestManager().UpdateObjective(killerID, "kill", 1);
    }
}

// Register handlers
void InitEventHandlers()
{
    EventBus bus = GetEventBus();

    bus.Register("OnPlayerKilled", new KillEventHandler());
    bus.Register("OnPlayerKilled", new QuestKillEventHandler());
}

// Trigger event
void OnPlayerKilled(PlayerBase killer, PlayerBase victim)
{
    map<string, string> data = new map<string, string>;
    data.Set("killerID", killer.GetIdentity().GetPlainId());
    data.Set("victimID", victim.GetIdentity().GetPlainId());

    GetEventBus().Trigger("OnPlayerKilled", data);
}
```

### Benefits
- ✅ Loose coupling (systems don't know about each other)
- ✅ Easy to add new listeners
- ✅ One event can trigger multiple actions
- ✅ Testable in isolation

---

## 🔧 Pattern 2: Shared Storage Integration

### Concept

Multiple systems store data in the SAME JSON file for cross-server persistence.

### Implementation

```c
// Unified storage manager
class UnifiedStorageManager
{
    protected ref UnifiedData m_Data;

    // Load unified data
    void Load()
    {
        string jsonPath = "$profile:UnifiedStorage/data.json";

        if (FileExist(jsonPath))
        {
            string jsonContent;
            File file = OpenFile(jsonPath, FileMode.READ);
            file.ReadString(jsonContent);
            file.Close();

            m_Data = UnifiedData.Deserialize(jsonContent);
        }
        else
        {
            m_Data = new UnifiedData();
        }
    }

    // Save unified data
    void Save()
    {
        string jsonPath = "$profile:UnifiedStorage/data.json";

        string jsonContent = m_Data.Serialize();
        File file = OpenFile(jsonPath, FileMode.WRITE);
        file.WriteString(jsonContent);
        file.Close();
    }

    // Get player data (contains ALL systems)
    UnifiedPlayerData GetPlayerData(string playerID)
    {
        if (!m_Data.Players.Contains(playerID))
        {
            m_Data.Players.Set(playerID, new UnifiedPlayerData);
        }
        return m_Data.Players.Get(playerID);
    }
}

// Unified data structure
class UnifiedData
{
    ref map<string, ref UnifiedPlayerData> Players;

    void UnifiedData()
    {
        Players = new map<string, ref UnifiedPlayerData>;
    }

    string Serialize()
    {
        return JsonSerializeToString(this);
    }

    static UnifiedData Deserialize(string json)
    {
        return (UnifiedData)JsonDeserialize(json, UnifiedData);
    }
}

// Player data contains ALL systems
class UnifiedPlayerData
{
    // XP System
    int Level;
    int CurrentXP;
    int TotalXP;

    // Banking System
    int BankBalance;

    // Quest System
    ref array<string> CompletedQuests;
    ref array<string> ActiveQuests;

    // Faction System
    string FactionID;
    int FactionRank;

    // Position (for teleportation)
    vector LastPosition;

    void UnifiedPlayerData()
    {
        CompletedQuests = new array<string>;
        ActiveQuests = new array<string>;
    }
}

// Usage: XP System saves to unified storage
class XPSystemIntegrated
{
    void AddXP(string playerID, int amount)
    {
        UnifiedStorageManager storage = GetUnifiedStorage();
        UnifiedPlayerData data = storage.GetPlayerData(playerID);

        data.CurrentXP += amount;
        data.TotalXP += amount;

        // Check level up
        if (data.CurrentXP >= GetXPForLevel(data.Level + 1))
        {
            data.Level++;
            // Trigger level up event
            GetEventBus().Trigger("OnLevelUp", playerID, data.Level);
        }

        storage.Save();
    }
}

// Usage: Banking System saves to unified storage
class BankingSystemIntegrated
{
    void Deposit(string playerID, int amount)
    {
        UnifiedStorageManager storage = GetUnifiedStorage();
        UnifiedPlayerData data = storage.GetPlayerData(playerID);

        data.BankBalance += amount;

        storage.Save();
    }
}
```

### Benefits
- ✅ Single JSON file for all systems
- ✅ Cross-server persistence (XP, bank, quests, faction)
- ✅ Atomic updates (all data saves together)
- ✅ Easy backup/restore

---

## 🔧 Pattern 3: RPC Bridge Integration

### Concept

Use CF RPC to communicate between client and server across multiple systems.

### Implementation

```c
// Unified RPC namespace
const string UNIFIED_RPC_NS = "UnifiedSystem";

// Unified RPC handler
class UnifiedRPCHandler
{
    void InitRPC()
    {
        CF_RPCManager rpc = GetRPCManager();

        // Register all RPC handlers
        rpc.AddRPC(UNIFIED_RPC_NS, "RequestData", this, SingleplayerExecutionType.Server);
        rpc.AddRPC(UNIFIED_RPC_NS, "UpdateData", this, SingleplayerExecutionType.Server);
        rpc.AddRPC(UNIFIED_RPC_NS, "SyncData", this, SingleplayerExecutionType.Client);
    }

    // Client requests data
    void RequestData(PlayerIdentity sender, Object target, ParamsReadContext ctx)
    {
        string playerID = sender.GetPlainId();
        string dataType;

        if (!ctx.Read(dataType))
            return;

        UnifiedStorageManager storage = GetUnifiedStorage();
        UnifiedPlayerData data = storage.GetPlayerData(playerID);

        // Send requested data back to client
        auto params = new Param2<string, UnifiedPlayerData>(dataType, data);
        GetRPCManager().SendRPC(UNIFIED_RPC_NS, "SyncData", params, true, sender);
    }

    // Server receives data update from client
    void UpdateData(PlayerIdentity sender, Object target, ParamsReadContext ctx)
    {
        string playerID = sender.GetPlainId();
        UnifiedPlayerData data;

        if (!ctx.Read(data))
            return;

        // Validate on server (security!)
        if (!ValidateDataUpdate(playerID, data))
        {
            Print("[Security] Invalid data update from " + playerID);
            return;
        }

        // Save to storage
        UnifiedStorageManager storage = GetUnifiedStorage();
        storage.GetPlayerData(playerID).Merge(data);
        storage.Save();
    }

    // Client receives data sync from server
    void SyncData(PlayerIdentity sender, Object target, ParamsReadContext ctx)
    {
        string dataType;
        UnifiedPlayerData data;

        if (!ctx.Read(dataType) || !ctx.Read(data))
            return;

        // Update local UI
        if (dataType == "XP")
        {
            GetXPUI().Update(data.Level, data.CurrentXP);
        }
        else if (dataType == "Bank")
        {
            GetBankUI().Update(data.BankBalance);
        }
        else if (dataType == "Quest")
        {
            GetQuestUI().Update(data.ActiveQuests, data.CompletedQuests);
        }
    }

    bool ValidateDataUpdate(string playerID, UnifiedPlayerData data)
    {
        // Server-side validation
        UnifiedStorageManager storage = GetUnifiedStorage();
        UnifiedPlayerData currentData = storage.GetPlayerData(playerID);

        // Example: Can't decrease level
        if (data.Level < currentData.Level)
            return false;

        // Example: Can't add unrealistic XP
        int xpDiff = data.TotalXP - currentData.TotalXP;
        if (xpDiff > 1000)  // Max 1000 XP per update
            return false;

        // Example: Can't add money from nothing
        int moneyDiff = data.BankBalance - currentData.BankBalance;
        if (moneyDiff > 10000)  // Max 10000 per update
            return false;

        return true;
    }
}

// Usage: Client requests XP data
void RequestXPData()
{
    PlayerIdentity identity = GetGame().GetPlayer().GetIdentity();

    auto params = new Param1<string>("XP");
    GetRPCManager().SendRPC(UNIFIED_RPC_NS, "RequestData", params, true, identity);
}

// Usage: Client updates bank balance
void UpdateBankBalance(int newBalance)
{
    PlayerIdentity identity = GetGame().GetPlayer().GetIdentity();

    auto params = new Param1<int>(newBalance);
    GetRPCManager().SendRPC(UNIFIED_RPC_NS, "UpdateData", params, true, identity);
}
```

### Benefits
- ✅ Secure (server-side validation)
- ✅ Unified communication
- ✅ Client-server sync
- ✅ Anti-cheat protection

---

## 🔧 Pattern 4: Dependency Injection Integration

### Concept

Systems inject dependencies into each other for tight coupling when needed.

### Implementation

```c
// Service locator
class ServiceLocator
{
    protected ref map<string, Class> m_Services;

    void Register(string name, Class service)
    {
        m_Services.Set(name, service);
    }

    Class GetService(string name)
    {
        return m_Services.Get(name);
    }
}

// System that needs other systems
class QuestSystemIntegrated
{
    protected XPSystem m_XPSystem;
    protected BankingSystem m_BankingSystem;
    protected NotificationSystem m_NotificationSystem;

    void QuestSystemIntegrated()
    {
        // Inject dependencies
        m_XPSystem = (XPSystem)GetServiceLocator().GetService("XP");
        m_BankingSystem = (BankingSystem)GetServiceLocator().GetService("Banking");
        m_NotificationSystem = (NotificationSystem)GetServiceLocator().GetService("Notification");
    }

    void CompleteQuest(string playerID, string questID)
    {
        Quest quest = GetQuest(questID);

        // Award XP through XP system
        m_XPSystem.AddXP(playerID, quest.XPReward);

        // Award money through banking system
        m_BankingSystem.Deposit(playerID, quest.MoneyReward);

        // Send notification
        m_NotificationSystem.Notify(playerID, "Quest completed: " + quest.Name);
    }
}

// Register services
void InitServices()
{
    ServiceLocator locator = GetServiceLocator();

    locator.Register("XP", new XPSystem());
    locator.Register("Banking", new BankingSystem());
    locator.Register("Notification", new NotificationSystem());
    locator.Register("Quest", new QuestSystemIntegrated());
}
```

### Benefits
- ✅ Explicit dependencies
- ✅ Easy testing (can inject mocks)
- ✅ Centralized service management
- ✅ Type-safe access

---

## 🔧 Pattern 5: Chain of Responsibility Integration

### Concept

Request passes through chain of systems until one handles it.

### Implementation

```c
// Chain handler base
class ChainHandler
{
    protected ChainHandler m_Next;

    void SetNext(ChainHandler next)
    {
        m_Next = next;
    }

    bool Handle(PlayerBase player, string action, map<string, string> params)
    {
        bool handled = Process(player, action, params);

        if (!handled && m_Next)
        {
            return m_Next.Handle(player, action, params);
        }

        return handled;
    }

    bool Process(PlayerBase player, string action, map<string, string> params)
    {
        return false;  // Override in subclasses
    }
}

// Handler 1: Banking system handles money actions
class BankingChainHandler: ChainHandler
{
    override bool Process(PlayerBase player, string action, map<string, string> params)
    {
        if (action == "Withdraw")
        {
            int amount = params.Get("amount").ToInt();
            GetBankingSystem().Withdraw(player, amount);
            return true;
        }
        else if (action == "Deposit")
        {
            int amount = params.Get("amount").ToInt();
            GetBankingSystem().Deposit(player, amount);
            return true;
        }

        return false;
    }
}

// Handler 2: Quest system handles quest actions
class QuestChainHandler: ChainHandler
{
    override bool Process(PlayerBase player, string action, map<string, string> params)
    {
        if (action == "AcceptQuest")
        {
            string questID = params.Get("questID");
            GetQuestSystem().AcceptQuest(player, questID);
            return true;
        }
        else if (action == "CompleteQuest")
        {
            string questID = params.Get("questID");
            GetQuestSystem().CompleteQuest(player, questID);
            return true;
        }

        return false;
    }
}

// Handler 3: Teleport system handles teleport actions
class TeleportChainHandler: ChainHandler
{
    override bool Process(PlayerBase player, string action, map<string, string> params)
    {
        if (action == "Teleport")
        {
            string location = params.Get("location");
            GetTeleportSystem().Teleport(player, location);
            return true;
        }

        return false;
    }
}

// Build chain
void InitChain()
{
    ChainHandler banking = new BankingChainHandler();
    ChainHandler quest = new QuestChainHandler();
    ChainHandler teleport = new TeleportChainHandler();

    banking.SetNext(quest);
    quest.SetNext(teleport);

    // Use chain
    map<string, string> params = new map<string, string>;
    params.Set("amount", "100");

    banking.Handle(player, "Deposit", params);  // Handled by banking
    banking.Handle(player, "AcceptQuest", params);  // Passed to quest handler
    banking.Handle(player, "Teleport", params);  // Passed to teleport handler
}
```

### Benefits
- ✅ Flexible handling
- ✅ Easy to add new handlers
- ✅ Decoupled systems
- ✅ Order of processing matters

---

## 📋 Common Integration Scenarios

### Scenario 1: Player Death (5 Systems Integration)

**Systems involved:**
1. XP System - Lose XP on death
2. Banking System - Pay death penalty
3. Quest System - Fail active quests
4. Storage System - Save death location
5. Notification System - Notify player

```c
void OnPlayerDeath(PlayerBase player)
{
    string playerID = player.GetIdentity().GetPlainId();

    // 1. XP System - Lose 10% XP
    GetXPSystem().RemoveXPPercent(playerID, 10);

    // 2. Banking System - Pay 500 currency penalty
    GetBankingSystem().Withdraw(playerID, 500);

    // 3. Quest System - Fail non-optional quests
    GetQuestSystem().FailQuestsOnDeath(playerID);

    // 4. Storage System - Save death location
    GetStorageSystem().SaveDeathLocation(playerID, player.GetPosition());

    // 5. Notification System
    GetNotificationSystem().Notify(playerID, "You died! Lost 10% XP and 500 currency.");
}
```

### Scenario 2: Player Join Server (4 Systems Integration)

**Systems involved:**
1. Storage System - Load player data
2. XP System - Restore level/XP
3. Banking System - Restore balance
4. Notification System - Welcome message

```c
void OnPlayerConnect(PlayerBase player)
{
    string playerID = player.GetIdentity().GetPlainId();

    // 1. Storage System - Load unified data
    UnifiedStorageManager storage = GetUnifiedStorage();
    UnifiedPlayerData data = storage.GetPlayerData(playerID);

    // 2. XP System - Restore level/XP
    GetXPSystem().SetXP(playerID, data.Level, data.CurrentXP);

    // 3. Banking System - Restore balance
    GetBankingSystem().SetBalance(playerID, data.BankBalance);

    // 4. Notification System - Welcome back
    int daysSinceLogin = GetDaysSinceLogin(playerID);
    GetNotificationSystem().Notify(playerID, "Welcome back! Level: " + data.Level + ", Bank: " + data.BankBalance);

    if (daysSinceLogin > 7)
    {
        // Bonus for returning players
        GetBankingSystem().Deposit(playerID, 1000);
        GetNotificationSystem().Notify(playerID, "Welcome back bonus: +1000 currency!");
    }
}
```

### Scenario 3: Complete Quest (6 Systems Integration)

**Systems involved:**
1. Quest System - Mark quest complete
2. XP System - Award XP
3. Banking System - Award money
4. Notification System - Notify player
5. Storage System - Save progress
6. Faction System - Award faction reputation

```c
void OnQuestComplete(PlayerBase player, Quest quest)
{
    string playerID = player.GetIdentity().GetPlainId();

    // 1. Quest System
    GetQuestSystem().MarkComplete(playerID, quest.ID);

    // 2. XP System
    GetXPSystem().AddXP(playerID, quest.XPReward, "Quest: " + quest.Name);

    // 3. Banking System
    GetBankingSystem().Deposit(playerID, quest.MoneyReward);

    // 4. Notification System
    string message = "Quest Complete: " + quest.Name;
    message += "\n+XP: " + quest.XPReward;
    message += "\n+Money: " + quest.MoneyReward;
    GetNotificationSystem().Notify(playerID, message);

    // 5. Storage System - Auto-save
    GetUnifiedStorage().Save();

    // 6. Faction System - Award reputation
    if (quest.FactionID != "")
    {
        GetFactionSystem().AddReputation(playerID, quest.FactionID, quest.ReputationReward);
    }
}
```

### Scenario 4: ATM Deposit (3 Systems Integration)

**Systems involved:**
1. Banking System - Process deposit
2. Notification System - Confirm transaction
3. Storage System - Save new balance

```c
void OnATMDeposit(PlayerBase player, int amount)
{
    string playerID = player.GetIdentity().GetPlainId();

    // 1. Banking System
    bool success = GetBankingSystem().Deposit(playerID, amount);

    if (success)
    {
        int newBalance = GetBankingSystem().GetBalance(playerID);

        // 2. Notification System
        GetNotificationSystem().Notify(playerID, "Deposited: " + amount + "\nNew Balance: " + newBalance);

        // 3. Storage System - Save
        GetUnifiedStorage().Save();
    }
    else
    {
        GetNotificationSystem().Notify(playerID, "Deposit failed: Insufficient funds");
    }
}
```

---

## 🛡️ Security Considerations

### 1. Validate All Inputs

```c
// BAD: Trust client data
void AddXP(string playerID, int amount)
{
    m_XP[playerID] += amount;  // Client can send amount = 999999!
}

// GOOD: Validate on server
void AddXP(string playerID, int amount)
{
    // Validate amount
    if (amount < 0 || amount > 1000)
    {
        Print("[Security] Invalid XP amount: " + amount);
        return;
    }

    // Check source (must be from server-side event)
    if (!GetGame().IsServer())
    {
        Print("[Security] Client tried to add XP directly!");
        return;
    }

    m_XP[playerID] += amount;
}
```

### 2. Use Server-Side Authority

```c
// All critical operations MUST be server-side
void WithdrawMoney(PlayerBase player, int amount)
{
    if (!GetGame().IsServer())
    {
        // Client requests, Server executes
        GetRPCManager().SendRPC("Banking", "Withdraw", new Param1<int>(amount), true);
        return;
    }

    // Server-side validation
    if (amount > m_Balance[player.GetID()])
    {
        GetNotificationSystem().Notify(player, "Insufficient funds");
        return;
    }

    m_Balance[player.GetID()] -= amount;
}
```

### 3. Rate Limiting

```c
class RateLimiter
{
    protected map<string, int> m_LastActionTime;

    bool CanAct(string playerID, int cooldownSeconds)
    {
        int now = GetGame().GetTickTime();

        if (m_LastActionTime.Contains(playerID))
        {
            int lastTime = m_LastActionTime.Get(playerID);
            if (now - lastTime < cooldownSeconds)
            {
                return false;  // Cooldown active
            }
        }

        m_LastActionTime.Set(playerID, now);
        return true;
    }
}

// Usage
void Teleport(PlayerBase player, vector location)
{
    RateLimiter limiter = GetRateLimiter();

    if (!limiter.CanAct(player.GetID(), 60))  // 60 second cooldown
    {
        GetNotificationSystem().Notify(player, "Teleport cooldown active");
        return;
    }

    player.SetPosition(location);
}
```

---

## 📝 Quick Reference

### Integration Pattern Selection

| Pattern | Use Case | Coupling | Complexity |
|---------|----------|----------|------------|
| Event-Driven | Broadcast events | Loose | Low |
| Shared Storage | Cross-server persistence | Tight | Medium |
| RPC Bridge | Client-server comms | Loose | Medium |
| Dependency Injection | Explicit dependencies | Tight | Low |
| Chain of Responsibility | Sequential processing | Loose | High |

### Common Integrations

| Feature | Systems Required |
|---------|------------------|
| Economy | ATM + Banking + Trading + Storage + Central Economy |
| RPG Server | Quests + XP + Factions + Notifications + Teleportation |
| Base Building | Building + Permissions + Storage + Safe Zones + AI |
| Vehicle System | Vehicles + Locking + Storage + Keys + Traders |
| PvP Server | Factions + Safe Zones + Loot + Teleportation + Leaderboards |

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** [Advanced Topics](../../topics/34_advanced_topics.md)
