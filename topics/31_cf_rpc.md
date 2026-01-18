# CF RPC System for Beginners

**Status:** ✅ Beginner Guide
**Difficulty:** Intermediate
**Time to Learn:** 1-2 hours
**Related:** [CF RPC Reference](../../reference/CF_RPC_SYSTEM.md) | [RPC Example](../../examples/cf_rpc/)

---

## ⚡ 30-Second Summary

CF (Community Framework) RPC is a communication system that lets client and server talk to each other. Instead of using numeric IDs like vanilla DayZ, CF RPC uses string names (like "MyMod_Function") which makes it much easier to use and prevents conflicts between mods.

---

## 🎯 What You'll Learn

1. **What is RPC?** - Understanding client-server communication
2. **Why use CF RPC?** - Benefits over vanilla DayZ RPC
3. **Basic Setup** - Creating your first RPC
4. **Sending Data** - Client → Server and Server → Client
5. **Common Issues** - Troubleshooting problems

---

## 💡 What is RPC?

**RPC (Remote Procedure Call)** = Calling a function on another computer

**Real-world analogy:**
- You're at home (client)
- You call your friend (server)
- You ask: "What's your bank balance?"
- Your friend checks and tells you: "1000 coins"

In DayZ:
- Client asks server: "What's my player's position?"
- Server checks and responds: "Position is (1000, 0, 5000)"

---

## 🔧 Why Use CF RPC Instead of Vanilla?

### Vanilla DayZ RPC Problems:

```c
// ❌ Vanilla: Confusing and error-prone
#define MY_RPC_ID 12345  // Random number, might conflict!

GetGame().RPC(playerIdentity, MY_RPC_ID, params, ...);

void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
{
    if (rpc_type == MY_RPC_ID)  // Which RPC is this?
    {
        // Handle...
    }
}
```

**Problems:**
- ❌ Numeric IDs are hard to remember
- ❌ Different mods might use same ID (conflicts!)
- ❌ No singleplayer/multiplayer handling
- ❌ Difficult to debug

### CF RPC Solutions:

```c
// ✅ CF RPC: Clear and safe
GetRPCManager().AddRPC("MyMod", "GetBalance", this, SingleplayerExecutionType.Server);

GetRPCManager().SendRPC("MyMod", "GetBalance", new Param1<string>("player123"), true, sender);

void GetBalance(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // Clear function name, no ID confusion!
}
```

**Benefits:**
- ✅ String names ("MyMod_GetBalance") are easy to remember
- ✅ No conflicts (each mod has unique namespace)
- ✅ Works in both singleplayer and multiplayer
- ✅ Easy debugging with clear warnings

---

## 🚀 Basic RPC Setup

### Step 1: Define RPC Function

**All RPC functions MUST have this signature:**

```c
void FunctionName(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // type = Is this running on server or client?
    // ctx = Input parameters sent from other side
    // sender = Who sent this RPC?
    // target = Which object is this for?
}
```

**Example: Server-side function**

```c
void RequestBalance(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // Only run on server
    if (type != CallType.Server)
        return;

    // Get player's ID
    string playerID = sender.GetPlainId();

    // Get balance from database
    int balance = GetPlayerBalance(playerID);

    // Send balance back to client
    GetRPCManager().SendRPC("MyMod", "ReceiveBalance",
        new Param1<int>(balance),
        true,
        sender);
}
```

**Example: Client-side function**

```c
void ReceiveBalance(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // Only run on client
    if (type != CallType.Client)
        return;

    // Read balance from parameters
    Param1<int> data;
    if (!ctx.Read(data))
        return;

    int balance = data.param1;

    // Display on screen
    Print("Your balance: " + balance.ToString());
}
```

---

### Step 2: Register RPC Handlers

**In `init.c` or `OnMissionStart()`:**

```c
modded class CustomMission
{
    override void OnMissionStart()
    {
        super.OnMissionStart();

        // Register server RPC
        GetRPCManager().AddRPC("MyMod", "RequestBalance", this,
            SingleplayerExecutionType.Server);

        // Register client RPC
        GetRPCManager().AddRPC("MyMod", "ReceiveBalance", this,
            SingleplayerExecutionType.Client);

        Print("[MyMod] RPC handlers registered!");
    }
}
```

**What This Does:**
- Tells CF RPC: "When someone calls 'MyMod::RequestBalance', run this function"
- `SingleplayerExecutionType.Server` = Run on server in singleplayer
- `SingleplayerExecutionType.Client` = Run on client in multiplayer

---

### Step 3: Send RPC

**From Client to Server:**

```c
// In client-side code
PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

// Ask server for balance
GetRPCManager().SendRPC(
    "MyMod",              // Namespace (must match registration)
    "RequestBalance",     // Function name (must match registration)
    new Param1<string>("player123"),  // Parameters (optional)
    true,                 // Guaranteed delivery (true = reliable)
    player.GetIdentity()  // Send to this player
);
```

**From Server to Client:**

```c
// In server-side code
PlayerBase player = GetPlayerBaseByPlayerID("player123");

// Send balance to client
GetRPCManager().SendRPC(
    "MyMod",
    "ReceiveBalance",
    new Param1<int>(1000),  // Balance value
    true,                    // Guaranteed delivery
    player.GetIdentity()     // Send to this player
);
```

---

## 📦 Sending Parameters

### Single Parameter

```c
// Send
GetRPCManager().SendRPC("MyMod", "Function",
    new Param1<int>(42),
    true,
    sender);

// Receive
void Function(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    Param1<int> data;
    if (!ctx.Read(data))
        return;

    int value = data.param1;  // value = 42
}
```

### Multiple Parameters

```c
// Send
GetRPCManager().SendRPC("MyMod", "Function",
    new Param3<int, string, float>(42, "Hello", 3.14),
    true,
    sender);

// Receive
void Function(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    Param3<int, string, float> data;
    if (!ctx.Read(data))
        return;

    int value1 = data.param1;      // 42
    string value2 = data.param2;   // "Hello"
    float value3 = data.param3;    // 3.14
}
```

### Arrays

```c
// Send
array<int> numbers = {1, 2, 3, 4, 5};
GetRPCManager().SendRPC("MyMod", "Function",
    new Param1<array<int>>(numbers),
    true,
    sender);

// Receive
void Function(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    Param1<array<int>> data;
    if (!ctx.Read(data))
        return;

    array<int> numbers = data.param1;  // {1, 2, 3, 4, 5}
}
```

---

## 🔄 Complete Example: Simple Chat System

### Server-Side: Receive and Broadcast

```c
modded class CustomMission
{
    override void OnMissionStart()
    {
        super.OnMissionStart();

        // Register chat RPC
        GetRPCManager().AddRPC("MyChat", "SendMessage", this,
            SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("MyChat", "ReceiveMessage", this,
            SingleplayerExecutionType.Client);
    }

    // Server receives message from client
    void SendMessage(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server)
            return;

        // Read message
        Param1<string> data;
        if (!ctx.Read(data))
            return;

        string message = data.param1;
        string playerName = sender.GetName();

        Print("[Server] " + playerName + ": " + message);

        // Broadcast to all players
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player)
            {
                GetRPCManager().SendRPC("MyChat", "ReceiveMessage",
                    new Param2<string, string>(playerName, message),
                    true,
                    player.GetIdentity());
            }
        }
    }

    // Client receives message from server
    void ReceiveMessage(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
            return;

        Param2<string, string> data;
        if (!ctx.Read(data))
            return;

        string playerName = data.param1;
        string message = data.param2;

        // Display on screen
        NotificationSystem.Notify(this, playerName + ": " + message);
    }
}

// Client sends message
modded class PlayerBase
{
    void SendChatMessage(string message)
    {
        GetRPCManager().SendRPC("MyChat", "SendMessage",
            new Param1<string>(message),
            true,
            GetIdentity());
    }
}
```

**Usage:**
```c
// In client script
player.SendChatMessage("Hello everyone!");
// Output on all clients: "PlayerName: Hello everyone!"
```

---

## ⚠️ Common Issues & Troubleshooting

### Issue 1: RPC Not Executing

**Symptoms:** You send RPC but function never runs.

**Cause:** Function name mismatch or not registered.

**Solution:**

```c
// ❌ WRONG: Names don't match!
GetRPCManager().AddRPC("MyMod", "GetBalance", this, ...);
void GetPlayerBalance(CallType type, ...)  // Different name!

// ✅ CORRECT: Names match exactly
GetRPCManager().AddRPC("MyMod", "GetBalance", this, ...);
void GetBalance(CallType type, ...)  // Same name!
```

**CF RPC Warning:**
```
[CF] player123 tried sending <MyMod>::<GetBalance> which does not seem to exist!
```

**Debug:**
```c
bool registered = GetRPCManager().AddRPC("MyMod", "GetBalance", this, ...);
if (registered)
    Print("✓ RPC registered");
else
    Print("✗ RPC registration failed");
```

---

### Issue 2: Parameters Are Empty

**Symptoms:** Function runs but parameters are default values.

**Cause:** Wrong Param type or didn't read from ctx.

**Solution:**

```c
// ❌ WRONG: Type mismatch
SendRPC("MyMod", "Func", new Param1<int>(42), ...);
void Func(CallType type, ParamsReadContext ctx, ...)
{
    Param1<string> data;  // Wrong type!
    ctx.Read(data);
    // data.param1 is empty!
}

// ✅ CORRECT: Matching types
SendRPC("MyMod", "Func", new Param1<int>(42), ...);
void Func(CallType type, ParamsReadContext ctx, ...)
{
    Param1<int> data;  // Correct type
    if (!ctx.Read(data))  // Always check return value!
    {
        Print("Failed to read parameter");
        return;
    }
    int value = data.param1;  // value = 42
}
```

---

### Issue 3: Function Runs Twice

**Symptoms:** In singleplayer, function executes 2 times.

**Cause:** `SingleplayerExecutionType.Both` causes double execution.

**Solution:**

```c
// ❌ WRONG: Causes double execution
GetRPCManager().AddRPC("MyMod", "LogEvent", this,
    SingleplayerExecutionType.Both);

void LogEvent(...)
{
    Print("This prints TWICE in singleplayer!");
}

// ✅ CORRECT: Single execution
GetRPCManager().AddRPC("MyMod", "LogEvent", this,
    SingleplayerExecutionType.Server);

void LogEvent(CallType type, ...)
{
    if (type == CallType.Server)
    {
        Print("This prints ONCE!");
    }
}
```

**When to use `Both`:**
- Only when you need BOTH server and client logic
- Example: Visual effect (client) + database save (server)

---

### Issue 4: RPC Conflict with Other Mod

**Symptoms:** Your RPC doesn't run or wrong mod's function runs.

**Cause:** Same `modName` as another mod.

**CF RPC Warning:**
```
[CF] player tried sending <Trading>::<Buy> which does not seem to exist!
```

**Solution:**

```c
// ❌ WRONG: Generic names (conflicts!)
GetRPCManager().AddRPC("Trading", "Buy", this, ...);
GetRPCManager().AddRPC("Trading", "Sell", this, ...);

// ✅ CORRECT: Unique names
GetRPCManager().AddRPC("MyMod_Trading", "Buy", this, ...);
GetRPCManager().AddRPC("MyMod_Trading", "Sell", this, ...);
```

**Rule:** Use `ModName_SystemName` pattern
- `"TraderPlus_Banking"` ✅
- `"MyMod_Inventory"` ✅
- `"Trading"` ❌ (too generic)

---

## 🎯 Execution Types Explained

### SingleplayerExecutionType.Server

```c
GetRPCManager().AddRPC("MyMod", "Func", this,
    SingleplayerExecutionType.Server);
```

**Behavior:**
- Multiplayer server: ✅ Runs
- Multiplayer client: ❌ Doesn't run
- Singleplayer: ✅ Runs once (as server)

**Use for:** Server-side logic (database, validation, etc.)

---

### SingleplayerExecutionType.Client

```c
GetRPCManager().AddRPC("MyMod", "Func", this,
    SingleplayerExecutionType.Client);
```

**Behavior:**
- Multiplayer server: ❌ Doesn't run
- Multiplayer client: ✅ Runs
- Singleplayer: ✅ Runs once (as client)

**Use for:** Client-side logic (UI, visual effects, etc.)

---

### SingleplayerExecutionType.Both

```c
GetRPCManager().AddRPC("MyMod", "Func", this,
    SingleplayerExecutionType.Both);
```

**Behavior:**
- Multiplayer server: ✅ Runs once (as server)
- Multiplayer client: ✅ Runs once (as client)
- Singleplayer: ✅ Runs TWICE (once as server, once as client)

**Use for:** When you need BOTH server and client logic

**Example:**

```c
void PlayerLevelUp(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type == CallType.Server)
    {
        // Server-side: Save to database
        SavePlayerLevel(sender);
    }
    else if (type == CallType.Client)
    {
        // Client-side: Show visual effects
        ShowLevelUpEffect();
        PlaySound("levelup");
    }
}
```

---

## 📝 Quick Reference

### Register RPC

```c
GetRPCManager().AddRPC("ModName", "FunctionName", this, SingleplayerExecutionType.Server);
```

### Send RPC

```c
GetRPCManager().SendRPC("ModName", "FunctionName", params, guaranteed, playerIdentity, targetObject);
```

### Receive RPC

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

## 🎓 Next Steps

### Beginner (You are here)
- ✅ Basic RPC setup
- ✅ Sending/receiving data
- ✅ Common issues

### Intermediate
- [ ] Learn CallType checks
- [ ] Use guaranteed vs unreliable
- [ ] Rate limiting
- [ ] Array parameters

### Advanced
- [ ] Custom class serialization
- [ ] Batch operations
- [ ] Security validation
- [ ] Performance optimization

**Recommended Reading:**
- [CF RPC Reference](../../reference/CF_RPC_SYSTEM.md) - Complete technical guide
- [RPC Networking](../../topics/09_networking.md) - Vanilla RPC comparison
- [Security Guide](../../reference/SECURITY_GUIDE.md) - Anti-cheat patterns

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete for Beginners
**Next:** See [CF RPC Reference](../../reference/CF_RPC_SYSTEM.md) for advanced features
