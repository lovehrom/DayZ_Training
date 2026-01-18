# Networking in DayZ Modding - Complete Guide

**Status:** ✅ Intermediate Guide
**Difficulty:** Intermediate
**Time to Learn:** 60 minutes
**Related:** [RPC Systems](../reference/RPC_NETWORKING.md) | [CF RPC](31_cf_rpc.md) | [Security](../reference/SECURITY_GUIDE.md)

---

## ⚡ 30-Second Summary

Networking in DayZ is about **client-server communication**. Clients (players) send requests to the server, the server validates and executes them, then sends responses back. Understanding RPC (Remote Procedure Call) is critical for multiplayer mods.

---

## 🎯 What You'll Learn

1. **Client-Server Model** - How multiplayer works
2. **RPC (Remote Procedure Call)** - Client-server communication
3. **RPC Types** - Server, Client, Both
4. **Parameters** - Sending data between client and server
5. **Security** - Validating client requests
6. **Common Patterns** - Real-world examples

---

## 💡 Client-Server Architecture

### Basic Model

```
┌─────────────────┐         ┌─────────────────┐
│   CLIENT        │         │   SERVER        │
│  (Player PC)    │         │  (Host Machine)  │
├─────────────────┤         ├─────────────────┤
│                 │         │                 │
│  • UI Display   │         │  • Authority    │
│  • Input        │   ◄──►  │  • Validation   │
│  • Prediction   │   RPC   │  • World State  │
│  • Effects      │         │  • Save/Load    │
│                 │         │                 │
└─────────────────┘         └─────────────────┘
```

### Key Concepts

**Client (Player's Computer):**
- Renders UI
- Handles keyboard/mouse input
- Shows visual effects
- **NO authority** (can't make decisions)

**Server (Host Machine):**
- **Has authority** (makes all decisions)
- Validates all actions
- Manages world state
- Saves/loads data

**Communication:**
- **RPC** = Remote Procedure Call
- Client sends request → Server validates → Server executes → Server responds

---

## 🔧 RPC (Remote Procedure Call)

### What is RPC?

**RPC** = Calling a function on another computer

**Example:**
```c
// Client wants to pick up item
// Instead of:
player.PickUpItem(item);  // Direct call (client-side only)

// We use:
GetRPCManager().SendRPC("MyMod", "PickUpItem", params, true, server);
// Server executes PickUpItem()
```

### Why RPC?

**Without RPC:**
```
Client: "I picked up the item!"
Server: "No you didn't, I didn't see it."
Result: Desync
```

**With RPC:**
```
Client: "Can I pick up this item?" (RPC request)
Server: "Let me check... Yes, here you go." (RPC response)
Result: Sync
```

---

## 📋 RPC Types

### 1. Server RPC

**Purpose:** Client calls function on server

**Use when:**
- Player wants to do something (pick up item, attack)
- Need server validation
- Need server authority

**Example:**
```c
// Client code
void RequestPickUpItem(ItemBase item)
{
    // Send RPC to server
    GetRPCManager().SendRPC("MyMod", "PickUpItem", new Param1<ItemBase>(item), true);
}

// Server code
void Server_PickUpItem(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    ItemBase item;
    if (!ctx.Read(item)) return;

    // Validate on server
    if (!CanPickUp(sender, item))
    {
        Print("[Security] Player tried to pick up invalid item!");
        return;
    }

    // Execute on server
    player.PickUpItem(item);
}
```

---

### 2. Client RPC

**Purpose:** Server calls function on client

**Use when:**
- Server wants to show UI
- Server wants to play sound
- Server wants to show notification

**Example:**
```c
// Server code
void NotifyPlayer(PlayerBase player, string message)
{
    // Send RPC to client
    GetRPCManager().SendRPC("MyMod", "ShowNotification", new Param1<string>(message), true, player.GetIdentity());
}

// Client code
void Client_ShowNotification(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    string message;
    if (!ctx.Read(message)) return;

    // Show notification on client
    NotificationSystem.Show(message);
}
```

---

### 3. Both RPC

**Purpose:** Call on BOTH client and server

**Use when:**
- Need to execute on both sides
- Synchronization needed

**Example:**
```c
// Server sends
GetRPCManager().SendRPC("MyMod", "PlaySound", params, true);

// Executed on BOTH client and server
void PlaySound()
{
    // Play sound on client
    // Play sound on server (for everyone nearby)
}
```

---

## 📦 Parameters

### Sending Data via RPC

**Param1** - Send 1 value:
```c
// Send
GetRPCManager().SendRPC("MyMod", "MyFunction", new Param1<int>(100), true);

// Receive
void MyFunction(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int value;
    if (!ctx.Read(value)) return;
    Print(value);  // 100
}
```

**Param2** - Send 2 values:
```c
// Send
GetRPCManager().SendRPC("MyMod", "MyFunction", new Param2<int, string>(100, "hello"), true);

// Receive
void MyFunction(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int value1;
    string value2;
    if (!ctx.Read(value1)) return;
    if (!ctx.Read(value2)) return;
    Print(value1);  // 100
    Print(value2);  // "hello"
}
```

**Param3+** - Send 3+ values:
```c
new Param3<int, string, float>(100, "hello", 1.5)
new Param4<int, string, float, bool>(100, "hello", 1.5, true)
// ... up to Param8
```

---

## 🔒 Security: Server-Side Validation

### ❌ BAD: Trust Client

```c
// Client-side
void AddXP(int amount)
{
    m_XP += amount;  // Client can send ANY amount!
}

// Problem: Cheat client sends AddXP(999999)
```

### ✅ GOOD: Server Authority

```c
// Client-side (request only)
void RequestAddXP(int amount)
{
    // Send request to server
    GetRPCManager().SendRPC("MyMod", "AddXP", new Param1<int>(amount), true);
}

// Server-side (validate and execute)
void Server_AddXP(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int amount;
    if (!ctx.Read(amount)) return;

    // Validate on server
    if (!GetGame().IsServer())
        return;

    if (amount < 0 || amount > 1000)
    {
        Print("[Security] Invalid XP amount: " + amount);
        return;
    }

    // Execute on server
    m_XP[sender.GetId()] += amount;
}
```

---

## 🎯 Common Patterns

### Pattern 1: Client Request, Server Execute

**Use when:** Player wants to do something

```c
// CLIENT: Request
void PickUpItem(ItemBase item)
{
    GetRPCManager().SendRPC("MyMod", "PickUp", new Param1<ItemBase>(item), true);
}

// SERVER: Execute
void Server_PickUp(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    ItemBase item;
    if (!ctx.Read(item)) return;

    // Validate
    if (!CanPickUp(sender, item))
        return;

    // Execute
    player.PickUpItem(item);
}
```

---

### Pattern 2: Server Notify Client

**Use when:** Server wants to show something

```c
// SERVER: Notify
void OnPlayerKilled(PlayerBase killer, PlayerBase victim)
{
    string message = "You killed " + victim.GetName();
    GetRPCManager().SendRPC("MyMod", "ShowKillMessage", new Param1<string>(message), true, killer.GetIdentity());
}

// CLIENT: Display
void Client_ShowKillMessage(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    string message;
    if (!ctx.Read(message)) return;

    NotificationSystem.Show(message);
}
```

---

### Pattern 3: Bi-Directional Communication

**Use when:** Need data from server

```c
// CLIENT: Request data
void RequestBankBalance()
{
    GetRPCManager().SendRPC("MyMod", "GetBalance", new Param1<string>(playerID), true);
}

// SERVER: Send data back
void Server_GetBalance(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    string playerID;
    if (!ctx.Read(playerID)) return;

    int balance = m_BankBalances.Get(playerID);

    // Send back to client
    GetRPCManager().SendRPC("MyMod", "UpdateBalanceUI", new Param1<int>(balance), true, sender);
}

// CLIENT: Update UI
void Client_UpdateBalanceUI(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int balance;
    if (!ctx.Read(balance)) return;

    BankUI.SetBalance(balance);
}
```

---

## 🛡️ Security Best Practices

### 1. NEVER Trust Client

**❌ BAD:**
```c
// Client sends: AddXP(999999)
void AddXP(int amount)
{
    m_XP += amount;  // EXPLOIT!
}
```

**✅ GOOD:**
```c
// Server validates
void Server_AddXP(int amount)
{
    if (amount > 100)  // Max 100 XP per request
        return;

    m_XP += amount;
}
```

---

### 2. Validate Everything

**❌ BAD:**
```c
void BuyItem(string itemClass)
{
    GiveItem(itemClass);  // No validation!
}
```

**✅ GOOD:**
```c
void Server_BuyItem(PlayerIdentity sender, string itemClass)
{
    // Validate player has money
    if (GetBalance(sender.GetId()) < GetPrice(itemClass))
        return;

    // Validate item exists
    if (!GetGame().IsKindOf(itemClass, "ItemBase"))
        return;

    // Execute
    GiveItem(sender.GetId(), itemClass);
}
```

---

### 3. Rate Limiting

**❌ BAD:**
```c
// Client spams: RequestAddXP() 1000 times/second
```

**✅ GOOD:**
```c
class RateLimiter
{
    map<string, int> m_LastActionTime;

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

void Server_AddXP(PlayerIdentity sender, int amount)
{
    if (!GetRateLimiter().CanAct(sender.GetId(), 1))  // 1 second cooldown
    {
        Print("[Security] Player spamming AddXP!");
        return;
    }

    m_XP[sender.GetId()] += amount;
}
```

---

## 🔧 Vanilla RPC vs CF RPC

### Vanilla RPC

**How it works:**
```c
// Register
GetRPCManager().AddRPC("MyMod", "MyFunction", this, SingleplayerExecutionType.Server);

// Send
GetRPCManager().SendRPC("MyMod", "MyFunction", params, true);
```

**Pros:**
- Built into DayZ
- Simple

**Cons:**
- Namespace conflicts
- No singleplayer support

---

### CF RPC (Community Framework)

**How it works:**
```c
// Register
GetRPCManager().AddRPC("MyMod_RPC", "MyFunction", this, SingleplayerExecutionType.Server);

// Send
GetRPCManager().SendRPC("MyMod_RPC", "MyFunction", params, true);
```

**Pros:**
- Namespace-based (no conflicts)
- Singleplayer support
- Used by 70%+ of mods

**Cons:**
- Requires CF dependency

**See:** [CF RPC Guide](31_cf_rpc.md) for details

---

## 📝 Quick Reference

### RPC Flow

```
1. Client: GetRPCManager().SendRPC(...)
2. Network: Transmit data
3. Server: Receive RPC
4. Server: Validate
5. Server: Execute
6. Server: SendRPC() back (optional)
7. Client: Receive response
```

### RPC Types

| Type | Direction | Use Case |
|------|-----------|----------|
| Server RPC | Client → Server | Player actions (pick up, attack) |
| Client RPC | Server → Client | Show UI, play sound |
| Both RPC | Server → Client+Server | Synchronization |

### Security Checklist

- ✅ Always validate on server
- ✅ Never trust client data
- ✅ Use rate limiting
- ✅ Check for exploits
- ✅ Log suspicious activity

---

## ⚠️ Common Mistakes

### ❌ Mistake 1: Client Authority

```c
// WRONG: Client gives itself XP
void AddXP(int amount) { m_XP += amount; }

// CORRECT: Server validates
void RequestAddXP(int amount) { SendRPC(...); }
void Server_AddXP(int amount) { if (amount <= 100) m_XP += amount; }
```

### ❌ Mistake 2: No Validation

```c
// WRONG: No validation
void BuyItem(string item) { GiveItem(item); }

// CORRECT: Validate everything
void Server_BuyItem(string item)
{
    if (HasMoney(player) && ItemExists(item))
        GiveItem(item);
}
```

### ❌ Mistake 3: Forgetting to Read Parameters

```c
// WRONG: Not reading parameters
void MyFunction(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    // Forgot to read!
}

// CORRECT: Always read
void MyFunction(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int value;
    if (!ctx.Read(value)) return;  // Always check return value
}
```

---

## 🎓 Next Steps

**After learning networking:**
1. **Read:** [RPC Networking Reference](../reference/RPC_NETWORKING.md) - Complete RPC reference
2. **Read:** [CF RPC Guide](31_cf_rpc.md) - Community Framework RPC
3. **Read:** [Security Guide](../reference/SECURITY_GUIDE.md) - Anti-cheat patterns
4. **Example:** [examples/cf_rpc/](../examples/cf_rpc/) - Working RPC examples

---

## 💡 Key Takeaways

1. **Client-Server Model:**
   - Client = UI/Input (NO authority)
   - Server = Authority/Validation

2. **RPC = Communication:**
   - Client sends request → Server validates → Server executes → Server responds

3. **Security:**
   - NEVER trust client
   - ALWAYS validate on server
   - Use rate limiting

4. **Patterns:**
   - Client Request → Server Execute
   - Server Notify → Client Display
   - Bi-Directional for data exchange

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** [CF RPC System](31_cf_rpc.md)
