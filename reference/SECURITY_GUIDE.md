# Security Guide

**Status:** ✅ Complete Reference
**Target:** Multiplayer mod developers
**Goal:** Prevent exploits, cheating, and data corruption

---

## ⚡ 30-Second Summary

Security best practices for DayZ multiplayer mods. Covers input validation, server-side authority, anti-cheat patterns, and data integrity.

---

## 🛡️ Core Security Principles

### Rule #1: Never Trust the Client

**Always validate on server:**
- RPC parameters
- Player actions
- Data modifications
- State changes

### Rule #2: Server Authority

**Critical operations MUST be server-side:**
- Economy/currency transactions
- XP/level changes
- Inventory spawns
- Building construction
- Reputation changes

### Rule #3: Validate Everything

**Check:**
- Parameter types
- Value ranges
- Permissions
- Cooldowns
- Distance limits

---

## 🔍 Input Validation

### RPC Parameter Validation

#### ❌ VULNERABLE: No validation

```c
modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if (rpc_type == SOME_RPC)
        {
            Param1<string> param;
            ctx.Read(param);

            // NO VALIDATION - EXPLOITABLE!
            GiveItem(param.param1);
        }
    }
}
```

#### ✅ SECURE: Full validation

```c
modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if (rpc_type == SOME_RPC)
        {
            // Validate sender
            if (sender.GetId() != GetIdentity().GetId())
            {
                Print("[Security] RPC from wrong sender!");
                return;
            }

            Param1<string> param;
            if (!ctx.Read(param))
            {
                Print("[Security] Invalid RPC parameter!");
                return;
            }

            // Validate item type
            if (!IsValidItemType(param.param1))
            {
                Print("[Security] Invalid item type: " + param.param1);
                return;
            }

            // Validate permissions
            if (!HasPermission(param.param1))
            {
                Print("[Security] No permission for: " + param.param1);
                return;
            }

            GiveItem(param.param1);
        }
    }

    bool IsValidItemType(string type)
    {
        // Whitelist approach
        array<string> validItems = {"AK74", "M4A1", "Bandage"};

        foreach (string valid : validItems)
        {
            if (valid == type)
                return true;
        }

        return false;
    }
}
```

---

### Value Range Validation

#### ❌ VULNERABLE: No range check

```c
void SetPlayerHealth(PlayerBase player, float health)
{
    // No validation - can set to 999999!
    player.GetStatHealth().Set(health);
}
```

#### ✅ SECURE: Range validation

```c
void SetPlayerHealth(PlayerBase player, float health)
{
    // Validate range
    if (health < 0 || health > 100)
    {
        Print("[Security] Invalid health value: " + health);
        return;
    }

    player.GetStatHealth().Set(health);
}
```

---

### Distance Validation

#### ❌ VULNERABLE: No distance check

```c
void TeleportPlayer(PlayerBase player, vector pos)
{
    // No validation - teleport anywhere!
    player.SetPosition(pos);
}
```

#### ✅ SECURE: Distance validation

```c
void TeleportPlayer(PlayerBase player, vector pos)
{
    float distance = vector.Distance(player.GetPosition(), pos);

    // Max teleport distance: 1000m
    if (distance > 1000)
    {
        Print("[Security] Teleport too far: " + distance + "m");
        return;
    }

    player.SetPosition(pos);
}
```

---

## 🔒 Server-Side Authority

### Economy Transactions

#### ❌ VULNERABLE: Client-side

```c
// Client script
void BuyItem(PlayerBase player, string item)
{
    int cost = GetItemCost(item);
    int balance = GetPlayerBalance(player); // From client - FAKEABLE!

    if (balance >= cost)
    {
        GiveItem(player, item);
    }
}
```

#### ✅ SECURE: Server-side

```c
// Server script
void BuyItem(PlayerBase player, string item)
{
    // Validate on server
    int cost = GetItemCost(item);
    int balance = GetPlayerBalanceServer(player); // From server - REAL

    if (balance >= cost)
    {
        DeductCurrency(player, cost);
        GiveItem(player, item);

        Print("[Economy] " + player.GetIdentity().GetName() + " bought " + item);
    }
    else
    {
        NotificationSystem.Notify(player, "Cannot afford " + item);
    }
}
```

---

### XP System

#### ❌ VULNERABLE: Client requests XP

```c
// Client sends RPC
void AddXP(PlayerBase player, int amount)
{
    // Client can request ANY amount!
    GetSkillManager().AddXP(amount);
}
```

#### ✅ SECURE: Server validates

```c
// Server validates XP
override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
{
    if (rpc_type == ADD_XP_RPC)
    {
        Param1<int> param;
        ctx.Read(param);

        int amount = param.param1;

        // Validate amount
        if (amount < 0 || amount > 1000)
        {
            Print("[Security] Invalid XP amount: " + amount);
            return;
        }

        // Server-side validation complete
        GetSkillManager().AddXP(amount);
    }
}
```

---

### Building Construction

#### ❌ VULNERABLE: Client builds

```c
// Client sends build request
void Build(PlayerBase player, string buildingType, vector pos)
{
    // No validation - build anywhere!
    GetGame().CreateObject(buildingType, pos);
}
```

#### ✅ SECURE: Server validates

```c
// Server validates build
void Build(PlayerBase player, string buildingType, vector pos)
{
    // Validate distance
    float dist = vector.Distance(player.GetPosition(), pos);
    if (dist > 50)
    {
        NotificationSystem.Notify(player, "Too far to build!");
        return;
    }

    // Validate territory
    if (!CanBuildInTerritory(player, pos))
    {
        NotificationSystem.Notify(player, "Cannot build here!");
        return;
    }

    // Validate building type
    if (!IsValidBuildingType(buildingType))
    {
        Print("[Security] Invalid building: " + buildingType);
        return;
    }

    // Server-side validation complete
    GetGame().CreateObject(buildingType, pos);
}
```

---

## 🚫 Anti-Cheat Patterns

### Rate Limiting

```c
class RateLimiter
{
    protected map<string, float> m_LastActionTime;

    bool CanPerformAction(PlayerBase player, string action, float cooldown)
    {
        string playerID = player.GetIdentity().GetId();
        string key = playerID + "_" + action;

        float currentTime = GetGame().GetTickTime();
        float lastTime = m_LastActionTime.Get(key);

        if (currentTime - lastTime < cooldown)
        {
            Print("[AntiCheat] " + player.GetIdentity().GetName() + " rate limited: " + action);
            return false;
        }

        m_LastActionTime.Set(key, currentTime);
        return true;
    }
}

// Usage
void OnCraftRequest(PlayerBase player, string item)
{
    if (!m_RateLimiter.CanPerformAction(player, "craft", 1.0))
    {
        NotificationSystem.Notify(player, "Crafting on cooldown!");
        return;
    }

    CraftItem(player, item);
}
```

---

### Speed Hack Detection

```c
class SpeedHackDetector
{
    protected map<string, vector> m_LastPosition;
    protected map<string, float> m_LastCheckTime;

    void Update(PlayerBase player)
    {
        string playerID = player.GetIdentity().GetId();
        vector currentPos = player.GetPosition();
        float currentTime = GetGame().GetTickTime();

        if (!m_LastPosition.Contains(playerID))
        {
            m_LastPosition.Set(playerID, currentPos);
            m_LastCheckTime.Set(playerID, currentTime);
            return;
        }

        vector lastPos = m_LastPosition.Get(playerID);
        float lastTime = m_LastCheckTime.Get(playerID);

        float distance = vector.Distance(lastPos, currentPos);
        float deltaTime = currentTime - lastTime;

        // Max speed: 10 m/s
        float maxDistance = deltaTime * 10.0;

        if (distance > maxDistance)
        {
            Print("[AntiCheat] Speed hack detected: " + player.GetIdentity().GetName());
            Print("[AntiCheat] Distance: " + distance + "m in " + deltaTime + "s");

            // Punish or kick
            // OnPlayerCheat(player, "speed_hack");
        }

        m_LastPosition.Set(playerID, currentPos);
        m_LastCheckTime.Set(playerID, currentTime);
    }
}
```

---

### Item Dupe Prevention

```c
class DupePrevention
{
    protected map<string, string> m_ItemSerials;

    void OnItemCreated(ItemBase item)
    {
        // Assign unique serial
        string serial = GenerateSerial();
        item.SetVariableString("Serial", serial);
        m_ItemSerials.Set(serial, item.GetType());
    }

    bool CanTransferItem(ItemBase item, PlayerBase from, PlayerBase to)
    {
        string serial = item.GetVariableString("Serial");

        // Check if serial exists
        if (!m_ItemSerials.Contains(serial))
        {
            Print("[AntiCheat] Duplicate item detected: " + serial);
            return false;
        }

        // Verify item type matches serial
        if (m_ItemSerials.Get(serial) != item.GetType())
        {
            Print("[AntiCheat] Item type mismatch: " + serial);
            return false;
        }

        return true;
    }

    void OnItemDeleted(ItemBase item)
    {
        string serial = item.GetVariableString("Serial");
        m_ItemSerials.Remove(serial);
    }
}
```

---

## 🔐 Data Integrity

### JSON Validation

```c
class SecureJSONLoader
{
    static bool LoadValidate(string filePath, out map<string, string> data)
    {
        if (!FileExist(filePath))
        {
            Print("[Security] File not found: " + filePath);
            return false;
        }

        // Load JSON
        JsonSaveManager json = new JsonSaveManager();

        if (!json.LoadFromFile(filePath, data))
        {
            Print("[Security] Invalid JSON: " + filePath);
            return false;
        }

        // Validate structure
        if (!ValidateDataStructure(data))
        {
            Print("[Security] Invalid data structure: " + filePath);
            return false;
        }

        return true;
    }

    static bool ValidateDataStructure(map<string, string> data)
    {
        // Check for required fields
        if (!data.Contains("version"))
            return false;

        if (!data.Contains("checksum"))
            return false;

        // Verify checksum
        string expectedChecksum = CalculateChecksum(data);
        string actualChecksum = data.Get("checksum");

        if (expectedChecksum != actualChecksum)
        {
            Print("[Security] Checksum mismatch!");
            return false;
        }

        return true;
    }
}
```

---

### Save File Corruption Prevention

```c
class SecureSaveManager
{
    static bool SaveData(string filePath, map data)
    {
        // Create backup
        if (FileExist(filePath))
        {
            string backupPath = filePath + ".bak";
            CopyFile(filePath, backupPath);
        }

        // Add checksum
        data.Set("checksum", CalculateChecksum(data));

        // Save to temp file first
        string tempPath = filePath + ".tmp";
        JsonSaveManager json = new JsonSaveManager();

        if (!json.SaveToFile(tempPath, data))
        {
            Print("[Security] Failed to save: " + tempPath);
            return false;
        }

        // Verify temp file
        if (!VerifyFile(tempPath))
        {
            Print("[Security] Verification failed: " + tempPath);
            DeleteFile(tempPath);
            return false;
        }

        // Replace original
        if (!RenameFile(tempPath, filePath))
        {
            Print("[Security] Failed to replace: " + filePath);
            return false;
        }

        return true;
    }

    static bool LoadData(string filePath, out map data)
    {
        // Check if file exists
        if (!FileExist(filePath))
        {
            Print("[Security] File not found: " + filePath);

            // Try backup
            string backupPath = filePath + ".bak";
            if (FileExist(backupPath))
            {
                Print("[Security] Loading backup: " + backupPath);
                return LoadData(backupPath, data);
            }

            return false;
        }

        // Load and verify
        JsonSaveManager json = new JsonSaveManager();

        if (!json.LoadFromFile(filePath, data))
        {
            Print("[Security] Load failed: " + filePath);

            // Try backup
            string backupPath = filePath + ".bak";
            if (FileExist(backupPath))
            {
                return LoadData(backupPath, data);
            }

            return false;
        }

        // Verify checksum
        if (!VerifyChecksum(data))
        {
            Print("[Security] Checksum invalid: " + filePath);

            // Try backup
            string backupPath = filePath + ".bak";
            if (FileExist(backupPath))
            {
                return LoadData(backupPath, data);
            }

            return false;
        }

        return true;
    }
}
```

---

## 🎯 Permission Systems

### Faction Permissions

```c
class PermissionManager
{
    static bool CanPerformAction(PlayerBase player, string action)
    {
        string faction = FactionManager.GetInstance().GetFaction(player);

        // Check faction permissions
        switch (action)
        {
            case "build":
                return CanBuild(player, faction);

            case "raid":
                return CanRaid(player, faction);

            case "teleport":
                return CanTeleport(player, faction);

            default:
                return false;
        }
    }

    static bool CanBuild(PlayerBase player, string faction)
    {
        // Must be in faction
        if (faction == "")
            return false;

        // Check reputation
        int rep = FactionManager.GetInstance().GetReputation(player, faction);

        return rep >= 0; // Non-hostile
    }

    static bool CanRaid(PlayerBase player, string faction)
    {
        // Must be in faction
        if (faction == "")
            return false;

        // Check reputation
        int rep = FactionManager.GetInstance().GetReputation(player, faction);

        return rep >= 50; // Friendly
    }
}
```

---

### Admin System

```c
class AdminSystem
{
    protected array<string> m_Admins;

    void AdminSystem()
    {
        // Load from file
        LoadAdmins();
    }

    bool IsAdmin(PlayerBase player)
    {
        string steam64ID = player.GetIdentity().GetId();

        foreach (string admin : m_Admins)
        {
            if (admin == steam64ID)
                return true;
        }

        return false;
    }

    bool CanUseAdminCommand(PlayerBase player, string command)
    {
        if (!IsAdmin(player))
        {
            Print("[Security] Non-admin attempted admin command: " + command);
            Print("[Security] Player: " + player.GetIdentity().GetName());
            return false;
        }

        return true;
    }

    void LogAdminAction(PlayerBase player, string command)
    {
        string log = "[ADMIN] " + player.GetIdentity().GetName() + " used: " + command;
        Print(log);

        // Write to admin log
        FileHandle file = OpenFile("$profile:admin.log", FileMode.APPEND);
        if (file != 0)
        {
            FPrintln(file, log);
            CloseFile(file);
        }
    }
}
```

---

## 📊 Monitoring & Logging

### Security Event Logging

```c
class SecurityLogger
{
    static void LogSuspiciousActivity(PlayerBase player, string event)
    {
        string log = "[SECURITY] Player: " + player.GetIdentity().GetName();
        log += " | SteamID: " + player.GetIdentity().GetId();
        log += " | Event: " + event;
        log += " | Time: " + GetGame().GetTickTime();

        Print(log);

        // Write to security log
        FileHandle file = OpenFile("$profile:security.log", FileMode.APPEND);
        if (file != 0)
        {
            FPrintln(file, log);
            CloseFile(file);
        }
    }

    static void LogRPC(PlayerBase player, int rpcType, string source)
    {
        string log = "[RPC] Player: " + player.GetIdentity().GetName();
        log += " | Type: " + rpcType;
        log += " | Source: " + source;

        Print(log);
    }
}
```

---

### Cheat Detection Alerts

```c
class CheatDetectionSystem
{
    static void OnCheatDetected(PlayerBase player, string cheatType)
    {
        SecurityLogger.LogSuspiciousActivity(player, "Cheat: " + cheatType);

        // Notify admins
        BroadcastToAdmins("CHEAT DETECTED: " + player.GetIdentity().GetName() + " - " + cheatType);

        // Log for review
        LogCheat(player, cheatType);

        // Optional: Auto-kick
        // KickPlayer(player);
    }

    static void BroadcastToAdmins(string message)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);

            if (AdminSystem.GetInstance().IsAdmin(player))
            {
                NotificationSystem.Notify(player, message);
            }
        }
    }
}
```

---

## 🔗 Related Topics

→ [Network](../topics/08_network.md) - RPC communication
→ [Performance](../reference/PERFORMANCE_GUIDE.md) - Optimization
→ [Best Practices](../reference/BEST_PRACTICES.md) - Code standards

---

*Last updated: 2025-01-18*
*Target: Multiplayer mod developers*
