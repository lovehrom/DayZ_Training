# CF RPC System - Complete Technical Reference

**Status:** ✅ Complete Reference
**Source:** Community Framework (CF) v1.0 - Mod ID: 1559212036
**Complexity:** Advanced
**Related:** [CF RPC Topic Guide](../../topics/31_cf_rpc.md) | [CF RPC Example](../../examples/cf_rpc/)

---

## ⚡ 30-Second Summary

The CF (Community Framework) RPC System is a robust client-server communication framework used by 70%+ of DayZ mods. It provides unified RPC handling with singleplayer/multiplayer abstraction, execution type control (Server/Client/Both), and automatic conflict prevention through namespace isolation.

---

## 🎯 Key Concepts

### What is CF RPC?

**RPC (Remote Procedure Call)** allows client and server to call functions on each other across the network.

**Vanilla DayZ RPC Problems:**
- ❌ No singleplayer/multiplayer abstraction
- ❌ Manual execution type checking required
- ❌ Numeric RPC IDs (conflicts between mods)
- ❌ Difficult to debug

**CF RPC Solutions:**
- ✅ Automatic singleplayer/multiplayer handling
- ✅ Declarative execution types (Server/Client/Both)
- ✅ Namespace-based routing (string modName + funcName)
- ✅ Built-in conflict detection and warnings
- ✅ Centralized RPC management via singleton
- ✅ Debugging with trace logging

### Why Use CF RPC?

**Benefits:**
- ✅ **Universal Standard** - Used by TraderPlus, Auction System, Quest System, and most major mods
- ✅ **Singleplayer Compatible** - Works seamlessly in both singleplayer and multiplayer
- ✅ **Type Safety** - Compile-time function signature checking
- ✅ **Namespace Isolation** - No RPC ID conflicts between mods
- ✅ **Debugging** - Built-in trace logging system
- ✅ **Conflict Prevention** - Automatic warnings for unregistered RPCs

**Drawbacks:**
- ❌ Adds dependency on Community Framework mod
- ❌ Slight overhead from metadata (negligible in practice)
- ❌ Requires understanding of execution types

---

## 📚 Core Architecture

### 1. RPCManager - Singleton Pattern

The RPCManager is a singleton that manages all RPC registration and routing.

**Location:** `addons/scripts/JM/CF/Scripts/3_Game/CommunityFramework/RPC/RPCManager.c`

**Singleton Instance:**
```c
static ref RPCManager g_RPCManager;

static RPCManager GetRPCManager()
{
    if (!g_RPCManager)
    {
        g_RPCManager = new RPCManager();
    }
    return g_RPCManager;
}
```

**Internal Structure:**
```c
class RPCManager
{
    static const int FRAMEWORK_RPC_ID = 10042;  // Single ID for all CF RPCs

    // Nested map: modName -> funcName -> wrapper
    protected autoptr map<string, ref map<string, ref RPCMetaWrapper>> m_RPCActions;
}
```

**Key Methods:**

```c
// Register RPC handler
bool AddRPC(string modName, string funcName, Class instance, int singlePlayerExecType);

// Send RPC
void SendRPC(string modName, string funcName, Param params, bool guaranteed, PlayerIdentity sendToIdentity, Object sendToTarget);

// Handle incoming RPC (called automatically by game)
void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx);

// Remove RPC handler
void RemoveRPC(string modName, string funcName);
```

---

### 2. Execution Types

CF RPC defines execution types for singleplayer mode:

**Source:** `RPCManager.c:8-19`

```c
enum SingleplayerExecutionType
{
    Server = 0,  // Execute on server-side in singleplayer
    Client,      // Execute on client-side in singleplayer
    Both         // Execute on BOTH client and server in singleplayer
}

enum CallType
{
    Server = 0,  // RPC call is executing on server
    Client       // RPC call is executing on client
}
```

**Execution Logic:**

**Multiplayer Server:**
```c
// RPC arrives at server
if (GetGame().IsServer() && GetGame().IsMultiplayer())
{
    // Execute function if registered
    CallFunctionParams(instance, funcName, CallType.Server, ...);
}
```

**Multiplayer Client:**
```c
// RPC arrives at client
if (GetGame().IsClient() && GetGame().IsMultiplayer())
{
    // Execute function if registered
    CallFunctionParams(instance, funcName, CallType.Client, ...);
}
```

**Singleplayer (Special Case - Both):**
```c
// Singleplayer uses Both execution
if (GetGame().IsServer() && !GetGame().IsMultiplayer())
{
    if (execType == SingleplayerExecutionType.Server || execType == SingleplayerExecutionType.Both)
    {
        CallFunctionParams(instance, funcName, CallType.Server, ...);  // Server-side logic
    }
}

if (GetGame().IsClient() && GetGame().IsMultiplayer())  // False in singleplayer
{
    // Not executed
}

// BUT: For singleplayer, there's special handling
if (GetGame().IsServer() && !GetGame().IsMultiplayer())
{
    if (execType == SingleplayerExecutionType.Client || execType == SingleplayerExecutionType.Both)
    {
        CallFunctionParams(instance, funcName, CallType.Client, ...);  // Client-side logic
    }
}
```

**Critical Understanding:** In singleplayer with `Both` execution, the function runs **twice**:
1. Once with `CallType.Server` (server-side logic)
2. Once with `CallType.Client` (client-side logic)

---

### 3. RPCMetaWrapper - Metadata Container

Wraps RPC function with execution metadata:

**Source:** `RPCManager.c:21-43`

```c
class RPCMetaWrapper
{
    protected Class m_Instance;        // Class instance containing function
    protected int m_SPExecType;        // SingleplayerExecutionType

    void RPCMetaWrapper(Class instance, int spExecType)
    {
        m_Instance = instance;
        m_SPExecType = spExecType;
    }

    Class GetInstance()
    {
        return m_Instance;
    }

    int GetSPExecutionType()
    {
        return m_SPExecType;
    }
}
```

---

## 🔧 Implementation Guide

### Step 1: Define RPC Functions

**RPC Function Signature (MUST match exactly):**

```c
void FunctionName(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // type - Whether called on server or client
    // ctx - Input parameters (deserialize with ctx.Read())
    // sender - Who sent the RPC (NULL if server initiated)
    // target - Target entity (player, object, etc.)
}
```

**Example: Server-side RPC**

```c
modded class CustomMission
{
    void RequestPlayerData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on server
        if (type != CallType.Server)
            return;

        // Validate sender
        if (!sender)
        {
            Print("[Security] RPC called without valid sender!");
            return;
        }

        // Read parameters
        Param1<string> data;
        if (!ctx.Read(data))
        {
            Print("[RPC] Failed to read parameter!");
            return;
        }

        string requestType = data.param1;

        // Process request...
        string playerID = sender.GetPlainId();
        int balance = GetPlayerBalance(playerID);

        // Send response back to client
        GetRPCManager().SendRPC("MyMod", "ReceivePlayerData",
            new Param1<int>(balance),
            true,  // Guaranteed delivery
            sender);
    }
}
```

**Example: Client-side RPC**

```c
modded class CustomMission
{
    void ReceivePlayerData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on client
        if (type != CallType.Client)
            return;

        // Read parameters
        Param1<int> data;
        if (!ctx.Read(data))
            return;

        int balance = data.param1;

        // Update UI
        UpdateBalanceDisplay(balance);
    }
}
```

---

### Step 2: Register RPC Handlers

**In `OnMissionStart()` or `init.c`:**

```c
modded class CustomMission
{
    override void OnMissionStart()
    {
        super.OnMissionStart();

        // Register server-side RPC
        GetRPCManager().AddRPC("MyMod", "RequestPlayerData", this,
            SingleplayerExecutionType.Server);

        // Register client-side RPC
        GetRPCManager().AddRPC("MyMod", "ReceivePlayerData", this,
            SingleplayerExecutionType.Client);

        // Register both (executes twice in singleplayer)
        GetRPCManager().AddRPC("MyMod", "LogEvent", this,
            SingleplayerExecutionType.Both);

        Print("[RPC] Registered MyMod RPC handlers");
    }
}
```

**Registration Parameters:**
1. `modName` - Unique namespace (prevents conflicts between mods)
2. `funcName` - Function name (must match exactly, case-sensitive)
3. `instance` - Class instance containing the function (use `this`)
4. `singlePlayerExecType` - Execution mode (Server/Client/Both)

**Critical Rules:**
- ✅ Use unique `modName` (e.g., "MyMod_SystemName" instead of "Trading")
- ✅ `funcName` must match function name exactly
- ✅ Choose correct `SingleplayerExecutionType` for your use case

---

### Step 3: Send RPC

**From Client to Server:**

```c
// In client-side code
PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

GetRPCManager().SendRPC(
    "MyMod",                    // modName (must match registration)
    "RequestPlayerData",        // funcName (must match registration)
    new Param1<string>("balance"),  // Parameters
    true,                        // Guaranteed delivery (reliable)
    player.GetIdentity()         // Send to this player's identity
);
```

**From Server to Client:**

```c
// In server-side code
GetRPCManager().SendRPC(
    "MyMod",
    "ReceivePlayerData",
    new Param1<int>(1000),       // Balance value
    true,                        // Guaranteed delivery
    player.GetIdentity()         // Send to specific player
);
```

**Broadcast to All Players:**

```c
// Send to all connected players
array<Man> players = new array<Man>;
GetGame().GetPlayers(players);

foreach (Man man : players)
{
    PlayerBase player = PlayerBase.Cast(man);
    if (player)
    {
        GetRPCManager().SendRPC(
            "MyMod",
            "ReceivePlayerData",
            new Param1<int>(data),
            true,
            player.GetIdentity()
        );
    }
}
```

**Send to Specific Object:**

```c
// Send RPC to specific entity (e.g., vehicle, tent)
Object targetObject = tent;

GetRPCManager().SendRPC(
    "MyMod",
    "ObjectUpdate",
    new Param1<int>(health),
    true,
    NULL,            // No specific player (broadcast)
    targetObject     // Target object
);
```

---

## 📋 Parameter Types

CF RPC supports all vanilla DayZ `Param` types:

### Built-in Param Types

```c
// Single value
Param1<bool>        // Boolean
Param1<int>         // Integer
Param1<float>       // Float
Param1<string>      // String
Param1<vector>      // Vector (position, direction)

// Multiple values
Param2<int, string>           // Two values
Param3<int, string, float>    // Three values
Param4<int, string, float, bool>  // Four values

// Arrays
Param1<array<int>>            // Integer array
Param1<array<string>>         // String array
Param1<array<vector>>         // Vector array

// Custom classes (must be serializable)
Param1<ref BankingAccount>    // Custom class with JSON serialization
```

### Sending Parameters

```c
// Single parameter
GetRPCManager().SendRPC("MyMod", "Function",
    new Param1<int>(42), ...);

// Two parameters
GetRPCManager().SendRPC("MyMod", "Function",
    new Param2<int, string>(42, "Hello"), ...);

// Three parameters
GetRPCManager().SendRPC("MyMod", "Function",
    new Param3<int, string, float>(42, "Hello", 3.14), ...);

// Four parameters
GetRPCManager().SendRPC("MyMod", "Function",
    new Param4<int, string, float, bool>(42, "Hello", 3.14, true), ...);

// Array parameter
array<int> values = {1, 2, 3, 4, 5};
GetRPCManager().SendRPC("MyMod", "Function",
    new Param1<array<int>>(values), ...);
```

### Receiving Parameters

```c
void MyFunction(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // Read single parameter
    Param1<int> data;
    if (ctx.Read(data))
    {
        int value = data.param1;
        Print("Received: " + value.ToString());
    }
    else
    {
        Print("Failed to read parameter!");
        return;
    }

    // Read multiple parameters
    Param2<int, string> data2;
    if (ctx.Read(data2))
    {
        int value1 = data2.param1;
        string value2 = data2.param2;
    }
}
```

---

## 🔄 Execution Flow

### Multiplayer: Client → Server

```
1. Client: GetRPCManager().SendRPC("MyMod", "Func", params, true, playerIdentity)
   ↓
2. Network: Serialize to bytes + transmit over network
   ↓
3. Server: Game calls RPCManager.OnRPC(sender, target, rpc_type, ctx)
   ↓
4. RPCManager: Read metadata Param2<string, string> (modName="MyMod", funcName="Func")
   ↓
5. RPCManager: Lookup in m_RPCActions["MyMod"]["Func"]
   ↓
6. RPCManager: Get RPCMetaWrapper (instance + execType)
   ↓
7. RPCManager: Check GetGame().IsServer() && IsMultiplayer() → TRUE
   ↓
8. RPCManager: CallFunctionParams(instance, "Func", CallType.Server, ctx, sender, target)
   ↓
9. Your Function: Executes with CallType.Server
```

### Multiplayer: Server → Client

```
1. Server: GetRPCManager().SendRPC("MyMod", "Func", params, true, targetPlayerIdentity)
   ↓
2. Network: Serialize + transmit to specific client
   ↓
3. Client: Game calls RPCManager.OnRPC(sender, target, rpc_type, ctx)
   ↓
4. RPCManager: Read metadata (modName="MyMod", funcName="Func")
   ↓
5. RPCManager: Lookup in m_RPCActions["MyMod"]["Func"]
   ↓
6. RPCManager: Check GetGame().IsClient() && IsMultiplayer() → TRUE
   ↓
7. RPCManager: CallFunctionParams(instance, "Func", CallType.Client, ctx, sender, target)
   ↓
8. Your Function: Executes with CallType.Client
```

### Singleplayer: Special Case (Both Execution)

```
1. Singleplayer: GetRPCManager().SendRPC("MyMod", "Func", params, true, NULL)
   ↓
2. RPCManager: Detects !IsMultiplayer() → TRUE
   ↓
3. RPCManager: Adds params TWICE to sendData array (for Both execution)
   ↓
4. RPC sent locally (no network)
   ↓
5. RPCManager.OnRPC() called
   ↓
6. RPCManager: Reads metadata (modName="MyMod", funcName="Func")
   ↓
7. RPCManager: Lookup wrapper with SingleplayerExecutionType.Both
   ↓
8. RPCManager: First execution (Server-side)
   - Check: GetGame().IsServer() && !IsMultiplayer() → TRUE
   - CallFunctionParams(instance, "Func", CallType.Server, ctx, sender, target)
   ↓
9. RPCManager: Second execution (Client-side)
   - Check: GetGame().IsServer() && !IsMultiplayer() && execType == Both → TRUE
   - CallFunctionParams(instance, "Func", CallType.Client, ctx, sender, target)
   ↓
10. Result: Function executed TWICE in singleplayer!
```

---

## 🛡️ Security Best Practices

### 1. Server-Side Authority (CRITICAL)

**ALWAYS validate critical operations on the server!**

```c
// ❌ BAD: Client trusts player input
void BuyItem(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Client)
        return;

    Param1<int> itemCost;
    ctx.Read(itemCost);

    GiveItem(player);  // EXPLOIT: Player can buy for free!
}

// ✅ GOOD: Server validates everything
void BuyItem(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;  // Only execute on server

    Param1<int> itemCost;
    if (!ctx.Read(itemCost))
        return;

    // Server validates player has enough money
    if (GetPlayerBalance(sender) < itemCost.param1)
    {
        Print("[Anti-Cheat] Player " + sender.GetPlainId() + " attempted to buy without sufficient funds!");
        return;
    }

    DeductMoney(sender, itemCost.param1);
    GiveItem(player);
}
```

---

### 2. Input Validation

**Always validate all parameters:**

```c
void DepositMoney(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param1<int> amount;
    if (!ctx.Read(amount))
        return;

    // Validate amount is positive
    if (amount.param1 <= 0)
    {
        Print("[Security] Player " + sender.GetPlainId() + " attempted negative deposit: " + amount.param1);
        return;
    }

    // Validate amount is reasonable
    if (amount.param1 > 1000000)
    {
        Print("[Security] Player " + sender.GetPlainId() + " attempted excessive deposit: " + amount.param1);
        return;
    }

    // Process deposit...
}
```

---

### 3. Sender Validation

**Check if sender is valid (on server):**

```c
void ServerFunction(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    // Server should always have valid sender
    if (!sender)
    {
        Print("[Security] RPC called without valid sender!");
        return;
    }

    string playerID = sender.GetPlainId();
    // Proceed...
}
```

---

### 4. Rate Limiting

**Prevent RPC spam:**

```c
class RPCRateLimiter
{
    private map<string, int> m_LastCallTime;
    private int m_CooldownMs;

    void RPCRateLimiter(int cooldownMs)
    {
        m_LastCallTime = new map<string, int>;
        m_CooldownMs = cooldownMs;
    }

    bool CanCall(string playerID)
    {
        int currentTime = GetGame().GetTickTime();

        if (m_LastCallTime.Contains(playerID))
        {
            int lastCall = m_LastCallTime.Get(playerID);
            if (currentTime - lastCall < m_CooldownMs)
            {
                return false;  // Cooldown not expired
            }
        }

        m_LastCallTime.Set(playerID, currentTime);
        return true;
    }
}

// Usage
ref RPCRateLimiter m_RateLimiter = new RPCRateLimiter(1000);  // 1 second cooldown

void FrequentFunction(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    if (!m_RateLimiter.CanCall(sender.GetPlainId()))
    {
        GetRPCManager().SendRPC("MyMod", "ReceiveNotification",
            new Param1<string>("Slow down! Too many requests."),
            true,
            sender);
        return;
    }

    // Process...
}
```

---

## 🐛 Common Issues & Troubleshooting

### Issue 1: RPC Not Executing

**Symptoms:** RPC sent but function never called.

**Causes:**
1. Function name mismatch (case-sensitive)
2. Not registered
3. Wrong execution type
4. Wrong modName

**Solution:**

```c
// Check registration - modName and funcName must match EXACTLY
GetRPCManager().AddRPC("MyMod", "FunctionName", this,
    SingleplayerExecutionType.Server);

// Function name must match EXACTLY (case-sensitive)
void FunctionName(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // Must have this exact signature
}
```

**Debug:**

```c
override void OnMissionStart()
{
    super.OnMissionStart();

    // Check if registration succeeded
    bool registered = GetRPCManager().AddRPC("MyMod", "FunctionName", this,
        SingleplayerExecutionType.Server);

    if (registered)
    {
        Print("[RPC] ✓ Registered: MyMod::FunctionName");
    }
    else
    {
        Print("[RPC] ✗ FAILED to register: MyMod::FunctionName");
    }
}
```

**CF RPC Warning (if not found):**
```
[CF] <senderPlainId> tried sending <MyMod>::<FunctionName> which does not seem to exist!
```

---

### Issue 2: Parameters Not Received

**Symptoms:** Function called but parameters are default values.

**Causes:**
1. Wrong Param type
2. Not reading from ctx
3. Type mismatch
4. Failed ctx.Read()

**Solution:**

```c
// ❌ BAD: Wrong Param type
SendRPC("MyMod", "Func", new Param1<int>(42), ...);
void Func(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    Param1<string> data;  // WRONG: Expects string, but sent int!
    ctx.Read(data);
    // data.param1 is empty/invalid
}

// ✅ GOOD: Matching types
SendRPC("MyMod", "Func", new Param1<int>(42), ...);
void Func(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    Param1<int> data;  // CORRECT: Expects int
    if (!ctx.Read(data))  // ALWAYS check return value!
    {
        Print("[RPC] Failed to read parameter!");
        return;
    }
    int value = data.param1;
    Print("[RPC] Received: " + value.ToString());
}
```

---

### Issue 3: Function Executing Multiple Times

**Symptoms:** Function runs twice in singleplayer.

**Cause:** `SingleplayerExecutionType.Both` causes double execution.

**Solution:**

```c
// ❌ BAD: Causes double execution in singleplayer
GetRPCManager().AddRPC("MyMod", "LogEvent", this,
    SingleplayerExecutionType.Both);  // Executes TWICE in singleplayer!

void LogEvent(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    Print("This prints TWICE in singleplayer!");
}

// ✅ GOOD: Single execution in singleplayer
GetRPCManager().AddRPC("MyMod", "LogEvent", this,
    SingleplayerExecutionType.Server);  // Executes ONCE in singleplayer (as server)

void LogEvent(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type == CallType.Server)
    {
        Print("This prints ONCE in singleplayer!");
    }
}
```

**When to Use `Both`:**
- Only when you need BOTH client-side and server-side logic
- Example: Visual effect (client) + server validation (server)
- Example: UI update (client) + database save (server)

```c
// Example where Both is appropriate
void PlayerLevelUp(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type == CallType.Server)
    {
        // Server-side: Save to database
        SavePlayerData(sender);
    }
    else if (type == CallType.Client)
    {
        // Client-side: Show visual effects
        ShowLevelUpEffect();
        PlaySound("levelup_sound");
    }
}
```

---

### Issue 4: RPC Conflict Between Mods

**Symptoms:** RPC not executing or wrong mod's function called.

**Cause:** Same `modName` used by multiple mods.

**CF RPC Warning:**
```
[CF] <player> tried sending <Trading>::<Buy> which does not seem to exist!
```

**Solution:**

```c
// ❌ BAD: Generic modName (conflicts!)
GetRPCManager().AddRPC("Trading", "Buy", this, ...);
GetRPCManager().AddRPC("Trading", "Sell", this, ...);
// Conflicts with any other trading mod using "Trading"

// ✅ GOOD: Unique modName with prefix
GetRPCManager().AddRPC("MyMod_Trading", "Buy", this, ...);
GetRPCManager().AddRPC("MyMod_Trading", "Sell", this, ...);
// Unique namespace, no conflicts
```

**Best Practice:**
```c
// Use: ModName_SystemName pattern
"TraderPlus_Banking"       // Banking system in TraderPlus mod
"AuctionSystem_Items"      // Item management in Auction mod
"MyMod_Inventory"          // Inventory system in MyMod
```

---

## 📊 Performance Considerations

### RPC Overhead

**Costs per RPC:**
- Network latency: ~50-200ms (multiplayer)
- Serialization/deserialization: ~0.1-1ms
- Function call overhead: ~0.01ms

**Total:** ~50-200ms per RPC (network is the bottleneck)

### Optimizations

**1. Batch Multiple Operations:**

```c
// ❌ BAD: Multiple RPCs (very slow)
for (int i = 0; i < 100; i++)
{
    GetRPCManager().SendRPC("MyMod", "UpdateItem", new Param1<int>(i), true, ident);
}
// Total time: 100 * 100ms = 10 seconds!

// ✅ GOOD: Single RPC with array
array<int> items = new array<int>;
for (int i = 0; i < 100; i++)
{
    items.Insert(i);
}
GetRPCManager().SendRPC("MyMod", "UpdateItems", new Param1<array<int>>(items), true, ident);
// Total time: ~100ms (100x faster!)
```

**2. Use Guaranteed=false for Non-Critical Updates:**

```c
// ❌ BAD: Guaranteed for position updates (unnecessary overhead)
GetRPCManager().SendRPC("MyMod", "PositionUpdate", params, true, ident);  // Reliable but slow

// ✅ GOOD: Unreliable for frequent updates (fast but may drop packets)
GetRPCManager().SendRPC("MyMod", "PositionUpdate", params, false, ident);  // Fast
// Lost packets will be corrected by next update
```

**When to use Guaranteed:**
- ✅ Critical transactions (money, items)
- ✅ One-time events (notifications)
- ✅ User-initiated actions (button clicks)

**When to use Unreliable:**
- ✅ Position updates (frequent)
- ✅ State updates (repeated often)
- ✅ Visual effects (non-critical)

**3. Limit RPC Frequency:**

```c
class RPCThrottler
{
    private map<string, int> m_LastRPC;
    private int m_MinIntervalMs;

    bool CanSend(string playerID)
    {
        int currentTime = GetGame().GetTickTime();
        int minInterval = m_MinIntervalMs;

        if (m_LastRPC.Contains(playerID))
        {
            int lastCall = m_LastRPC.Get(playerID);
            if (currentTime - lastCall < minInterval)
            {
                return false;  // Too soon
            }
        }

        m_LastRPC.Set(playerID, currentTime);
        return true;
    }
}

// Usage: Max 10 RPC per second per player
ref RPCThrottler m_Throttler = new RPCThrottler(100);  // 100ms between RPCs
```

---

## 🔗 Integration Examples

### Example 1: Banking System

```c
// Client requests balance
void RequestBalance(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    string playerID = sender.GetPlainId();
    int balance = BankingManager.GetBalance(playerID);

    // Send balance to client
    GetRPCManager().SendRPC("Banking", "ReceiveBalance",
        new Param1<int>(balance), true, sender);
}

// Client receives balance
void ReceiveBalance(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Client)
        return;

    Param1<int> data;
    if (!ctx.Read(data))
        return;

    int balance = data.param1;

    // Update UI
    UpdateBalanceDisplay(balance);
}
```

### Example 2: Chat System

```c
// Server receives chat message
void ChatMessage(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param2<string, string> data;  // (channel, message)
    if (!ctx.Read(data))
        return;

    string channel = data.param1;
    string message = data.param2;

    // Broadcast to all players
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    foreach (Man man : players)
    {
        PlayerBase player = PlayerBase.Cast(man);
        if (player)
        {
            GetRPCManager().SendRPC("Chat", "ReceiveChat",
                new Param3<string, string, string>(sender.GetName(), channel, message),
                true,
                player.GetIdentity());
        }
    }
}

// Client displays chat message
void ReceiveChat(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Client)
        return;

    Param3<string, string, string> data;  // (playerName, channel, message)
    if (!ctx.Read(data))
        return;

    string playerName = data.param1;
    string channel = data.param2;
    string message = data.param3;

    // Display in chat box
    ChatBox.AddMessage(channel, playerName + ": " + message);
}
```

### Example 3: Object Synchronization

```c
// Server sends vehicle data to client
void SyncVehicle(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Car vehicle = Car.Cast(target);
    if (!vehicle)
        return;

    float fuel = vehicle.GetFuel();
    float health = vehicle.GetHealth();

    GetRPCManager().SendRPC("Vehicles", "VehicleUpdate",
        new Param2<float, float>(fuel, health),
        true,
        sender);
}

// Client updates vehicle
void VehicleUpdate(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Client)
        return;

    Param2<float, float> data;
    if (!ctx.Read(data))
        return;

    float fuel = data.param1;
    float health = data.param2;

    // Update local vehicle
    Car vehicle = Car.Cast(target);
    if (vehicle)
    {
        vehicle.SetFuel(fuel);
        vehicle.SetHealth(health);
    }
}
```

---

## 📚 Advanced Topics

### 1. Trace Logging

CF RPC includes built-in trace logging:

```c
#ifdef CF_TRACE_ENABLED
auto trace = CF_Trace_4(this).Add(modName).Add(funcName).Add(params).Add(guaranteed);
#endif
```

**Enable in CF config:**
```c
#define CF_TRACE_ENABLED
```

**Output:**
```
[CF_TRACE] RPCManager::SendRPC(modName="MyMod", funcName="Func", params=..., guaranteed=true)
[CF_TRACE] RPCManager::OnRPC(modName="MyMod", funcName="Func", sender="SteamID_123", target=...)
[CF_TRACE] FunctionName(CallType=Server, ctx=..., sender=..., target=...)
```

### 2. Custom Event Handlers

CF includes event handler system:

**Source:** `CF/Scripts/2_GameLib/CommunityFramework/EventHandler/CF_EventHandler.c`

```c
class CF_EventHandler
{
    void AddSubscriber(ScriptCaller caller);
    void Invoke(Class sender = NULL, CF_EventArgs args = NULL);
}

// Usage
ref CF_EventHandler m_OnPlayerConnected = new CF_EventHandler();

// Subscribe
m_OnPlayerConnected.AddSubscriber(OnPlayerConnectedHandler);

// Invoke
m_OnPlayerConnected.Invoke(this, new CF_EventArgs());
```

---

## 🎯 Best Practices

### DO ✅

- ✅ Use unique `modName` with prefix (e.g., "MyMod_SystemName")
- ✅ Validate all inputs on server
- ✅ Use `SingleplayerExecutionType.Server` for most server RPCs
- ✅ Check `ctx.Read()` return value
- ✅ Rate limit frequent RPCs
- ✅ Log RPC calls for debugging
- ✅ Use guaranteed=false for frequent updates

### DON'T ❌

- ❌ Use generic modNames ("RPC", "Network", "Trading")
- ❌ Trust client input without validation
- ❌ Use `SingleplayerExecutionType.Both` unless necessary
- ❌ Send large arrays (>1KB) in single RPC
- ❌ Spam RPCs in loops (batch instead)
- ❌ Forget to check `CallType` in RPC functions
- ❌ Use guaranteed=true for position updates

---

## 📝 Quick Reference

### Registration

```c
GetRPCManager().AddRPC("ModName", "FunctionName", this, SingleplayerExecutionType.Server);
```

### Sending

```c
GetRPCManager().SendRPC("ModName", "FunctionName", params, guaranteed, playerIdentity, targetObject);
```

### Receiving

```c
void FunctionName(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)  // or CallType.Client
        return;

    Param1<int> data;
    if (!ctx.Read(data))
        return;

    // Process...
}
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** See [CF RPC Topic Guide](../../topics/31_cf_rpc.md) for beginner tutorial
