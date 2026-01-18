# CF RPC System Example

**Status:** ✅ Working Example
**Complexity:** Intermediate
**Time to Implement:** 1-2 hours
**Related:** [CF RPC Reference](../../reference/CF_RPC_SYSTEM.md) | [CF RPC Topic](../../topics/31_cf_rpc.md)

---

## ⚡ 30-Second Summary

Complete working example of CF (Community Framework) RPC system. Demonstrates client-server communication with string-based namespace routing, automatic singleplayer/multiplayer handling, and built-in conflict prevention.

---

## 🎯 Features

### Core Functionality
- ✅ **Client → Server RPC** - Send requests from client to server
- ✅ **Server → Client RPC** - Send responses from server to client
- ✅ **Parameter Passing** - Send single and multiple parameters
- ✅ **Array Parameters** - Send arrays of data
- ✅ **Broadcasting** - Send to all players or specific player
- ✅ **Execution Types** - Server/Client/Both execution modes
- ✅ **Error Handling** - Validation and error checking
- ✅ **Debug Logging** - Print statements for troubleshooting

### Examples Included
1. **Simple Echo** - Basic request/response
2. **Player Data** - Request player information from server
3. **Chat System** - Broadcast messages to all players
4. **Object Sync** - Synchronize object state
5. **Array Transfer** - Send arrays of data

---

## 📁 File Structure

```
cf_rpc/
├── README.md                        ← This file
├── config.cpp                       ← Mod definition
└── scripts/
    ├── 3_Game/
    │   ├── CF_RPCManager.c          ← CF RPC wrapper (simplified)
    │   ├── RPC_Echo.c               ← Simple echo example
    │   ├── RPC_PlayerData.c         ← Player data example
    │   ├── RPC_Chat.c               ← Chat system example
    │   └── RPC_ObjectSync.c         ← Object sync example
    └── 4_World/
        └── Classes/
            └── CF_RPCExample.c      ← Integration with PlayerBase
```

---

## 💡 Installation

### Step 1: Dependency - Community Framework

**IMPORTANT:** This example requires Community Framework to be installed!

1. Subscribe to Community Framework: https://steamcommunity.com/sharedfiles/filedetails/?id=1559212036
2. Add to server `server.cfg`:
   ```
   @CommunityFramework
   @CF_RPC_Example
   ```

### Step 2: Copy Files

Copy the `cf_rpc` folder to your DayZ server:

```
DayZ_Server/@CF_RPC_Example/
├── config.cpp
└── scripts/
```

### Step 3: Configure

**Edit `config.cpp`:**
- Change mod name if needed
- Add required dependencies

### Step 4: Test

1. Start server
2. Join as client
3. Press chat key (`/`)
4. Type: `/rpc_test echo Hello World`
5. Check server log and client notification

---

## 🎮 Usage

### Example 1: Simple Echo

**Client sends:**
```c
GetRPCManager().SendRPC("CF_RPC_Example", "Echo",
    new Param1<string>("Hello Server"),
    true,
    player.GetIdentity());
```

**Server receives and responds:**
```c
void Echo(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param1<string> data;
    if (!ctx.Read(data))
        return;

    string message = data.param1;

    // Echo back to client
    GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveEcho",
        new Param1<string>("Echo: " + message),
        true,
        sender);
}
```

**Client receives:**
```c
void ReceiveEcho(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Client)
        return;

    Param1<string> data;
    if (!ctx.Read(data))
        return;

    string message = data.param1;

    // Display
    NotificationSystem.Notify(this, message);
}
```

---

### Example 2: Request Player Data

**Client requests data:**
```c
GetRPCManager().SendRPC("CF_RPC_Example", "RequestPlayerData",
    new Param1<string>("balance"),
    true,
    player.GetIdentity());
```

**Server processes:**
```c
void RequestPlayerData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param1<string> data;
    if (!ctx.Read(data))
        return;

    string requestType = data.param1;
    string playerID = sender.GetPlainId();

    int response;

    if (requestType == "balance")
    {
        response = GetPlayerBalance(playerID);
    }
    else if (requestType == "level")
    {
        response = GetPlayerLevel(playerID);
    }

    // Send response
    GetRPCManager().SendRPC("CF_RPC_Example", "ReceivePlayerData",
        new Param2<string, int>(requestType, response),
        true,
        sender);
}
```

---

### Example 3: Chat System

**Client sends message:**
```c
GetRPCManager().SendRPC("CF_RPC_Example", "ChatMessage",
    new Param1<string>("Hello everyone!"),
    true,
    player.GetIdentity());
```

**Server broadcasts:**
```c
void ChatMessage(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param1<string> data;
    if (!ctx.Read(data))
        return;

    string message = data.param1;
    string playerName = sender.GetName();

    // Broadcast to all players
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    foreach (Man man : players)
    {
        PlayerBase player = PlayerBase.Cast(man);
        if (player)
        {
            GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveChat",
                new Param2<string, string>(playerName, message),
                true,
                player.GetIdentity());
        }
    }
}
```

---

### Example 4: Array Transfer

**Send array from client:**
```c
array<int> numbers = {1, 2, 3, 4, 5};

GetRPCManager().SendRPC("CF_RPC_Example", "ProcessArray",
    new Param1<array<int>>(numbers),
    true,
    player.GetIdentity());
```

**Server processes:**
```c
void ProcessArray(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param1<array<int>> data;
    if (!ctx.Read(data))
        return;

    array<int> numbers = data.param1;

    int sum = 0;
    foreach (int num : numbers)
    {
        sum += num;
    }

    // Send result back
    GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveArrayResult",
        new Param1<int>(sum),
        true,
        sender);
}
```

---

## 🔧 Customization

### Change Namespace

**Edit config.cpp:**
```cpp
class CfgPatches
{
    class MyCustomRPC
    {
        // ...
    };
};
```

**Update all RPC calls:**
```c
// Old
GetRPCManager().SendRPC("CF_RPC_Example", "Function", ...);

// New
GetRPCManager().SendRPC("MyCustomRPC", "Function", ...);
```

---

### Add New RPC

**1. Define function:**
```c
void MyNewRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    // Your code here...
}
```

**2. Register in OnMissionStart():**
```c
GetRPCManager().AddRPC("CF_RPC_Example", "MyNewRPC", this,
    SingleplayerExecutionType.Server);
```

**3. Send RPC:**
```c
GetRPCManager().SendRPC("CF_RPC_Example", "MyNewRPC",
    new Param1<int>(42),
    true,
    player.GetIdentity());
```

---

## ⚠️ Troubleshooting

### Issue 1: RPC Not Executing

**Check:**
1. Function name matches registration (case-sensitive)
2. RPC is registered in OnMissionStart()
3. Correct namespace (modName)
4. CallType check matches execution type

**Debug:**
```c
bool registered = GetRPCManager().AddRPC("CF_RPC_Example", "MyRPC", this, ...);
Print("RPC registered: " + registered.ToString());
```

---

### Issue 2: Parameters Not Received

**Check:**
1. Param types match (send/receive)
2. ctx.Read() returns true
3. Check ctx.Read() return value

**Debug:**
```c
Param1<int> data;
if (!ctx.Read(data))
{
    Print("Failed to read parameter!");
    return;
}
Print("Received: " + data.param1.ToString());
```

---

### Issue 3: Singleplayer Double Execution

**Check:**
1. SingleplayerExecutionType is correct
2. Not using `Both` unless needed

**Solution:**
```c
// Use Server for server-side logic
GetRPCManager().AddRPC("CF_RPC_Example", "MyRPC", this,
    SingleplayerExecutionType.Server);

// Check CallType in function
void MyRPC(CallType type, ...)
{
    if (type != CallType.Server)
        return;
}
```

---

## 🔗 Integration Examples

### With Banking System

```c
// Request balance
void RequestBalance(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    string playerID = sender.GetPlainId();
    int balance = BankingManager.GetBalance(playerID);

    GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveBalance",
        new Param1<int>(balance),
        true,
        sender);
}
```

### With Quest System

```c
// Complete quest
void CompleteQuest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param1<int> questID;
    if (!ctx.Read(questID))
        return;

    QuestManager.CompleteQuest(sender, questID.param1);

    // Send rewards
    GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveReward",
        new Param2<int, int>(1000, 50),  // XP, reputation
        true,
        sender);
}
```

### With Trading System

```c
// Buy item
void BuyItem(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param2<string, int> data;  // (itemClassName, quantity)
    if (!ctx.Read(data))
        return;

    string item = data.param1;
    int qty = data.param2;

    // Validate player has enough money
    if (!TradingManager.CanAfford(sender, item, qty))
    {
        GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveError",
            new Param1<string>("Insufficient funds"),
            true,
            sender);
        return;
    }

    // Complete purchase
    TradingManager.BuyItem(sender, item, qty);

    // Send confirmation
    GetRPCManager().SendRPC("CF_RPC_Example", "ReceivePurchase",
        new Param2<string, int>(item, qty),
        true,
        sender);
}
```

---

## 📊 Performance Notes

### RPC Overhead

- **Network latency:** 50-200ms per RPC (multiplayer)
- **Serialization:** 0.1-1ms
- **Function call:** 0.01ms

**Best Practices:**
- ✅ Batch operations into single RPC
- ✅ Use guaranteed=false for frequent updates
- ✅ Rate limit client requests
- ❌ Don't send RPC in loops (batch instead)

### Optimization Example

```c
// ❌ BAD: 100 RPC calls
for (int i = 0; i < 100; i++)
{
    GetRPCManager().SendRPC("CF_RPC_Example", "Update",
        new Param1<int>(i),
        true,
        sender);
}
// Total: ~10 seconds

// ✅ GOOD: 1 RPC call with array
array<int> items = new array<int>;
for (int i = 0; i < 100; i++)
{
    items.Insert(i);
}
GetRPCManager().SendRPC("CF_RPC_Example", "UpdateBatch",
    new Param1<array<int>>(items),
    true,
    sender);
// Total: ~100ms (100x faster)
```

---

## 🛡️ Security Considerations

### Server-Side Validation

**ALWAYS validate on server:**

```c
void DepositMoney(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    Param1<int> amount;
    if (!ctx.Read(amount))
        return;

    // Validate
    if (amount.param1 <= 0)
    {
        Print("[Security] Negative deposit attempt");
        return;
    }

    if (amount.param1 > 1000000)
    {
        Print("[Security] Excessive deposit attempt");
        return;
    }

    // Process...
}
```

### Rate Limiting

```c
class RateLimiter
{
    private map<string, int> m_LastCall;

    bool CanCall(string playerID)
    {
        int currentTime = GetGame().GetTickTime();

        if (m_LastCall.Contains(playerID))
        {
            if (currentTime - m_LastCall[playerID] < 1000)  // 1 second cooldown
                return false;
        }

        m_LastCall[playerID] = currentTime;
        return true;
    }
}
```

---

## 📝 API Reference

### Registration

```c
bool AddRPC(
    string modName,              // Unique namespace
    string funcName,             // Function name
    Class instance,              // Class instance (use 'this')
    int singlePlayerExecType     // Server/Client/Both
)
```

### Sending

```c
void SendRPC(
    string modName,              // Must match registration
    string funcName,             // Must match registration
    Param params,                // Parameters (Param1, Param2, etc.)
    bool guaranteed,             // true=reliable, false=fast
    PlayerIdentity sendToIdentity,  // Target player (NULL=broadcast)
    Object sendToTarget          // Target object (optional)
)
```

### Receiving

```c
void FunctionName(
    CallType type,               // Server or Client
    ParamsReadContext ctx,       // Input parameters
    PlayerIdentity sender,       // Who sent RPC
    Object target                // Target entity
)
```

---

## 🎓 Next Steps

### Beginner (You are here)
- ✅ Basic RPC setup
- ✅ Simple echo example
- ✅ Parameter passing

### Intermediate
- [ ] Custom data structures
- [ ] Error handling
- [ ] Rate limiting
- [ ] Chat system

### Advanced
- [ ] Object synchronization
- [ ] Batch operations
- [ ] Security validation
- [ ] Performance optimization

**Recommended Reading:**
- [CF RPC Reference](../../reference/CF_RPC_SYSTEM.md) - Complete technical guide
- [CF RPC Topic](../../topics/31_cf_rpc.md) - Beginner tutorial
- [Security Guide](../../reference/SECURITY_GUIDE.md) - Anti-cheat patterns

---

**Last Updated:** 2025-01-18
**Status:** ✅ Working Example
**Author:** DayZ Training Project
