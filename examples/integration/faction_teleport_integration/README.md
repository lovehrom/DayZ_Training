# Faction + Teleport Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** faction_system + teleportation

---

## ⚡ 30-Second Summary

Integration example showing faction-specific teleportation. Each faction has unique teleport locations, and teleportation costs reputation points.

---

## 🎯 Features

### Integration Mechanics
- ✅ **Faction-Specific Teleports** - Different locations per faction
- ✅ **Reputation Cost** - Teleporting costs reputation
- ✅ **Faction Bases** - Safe zones for each faction
- ✅ **Cost System** - 10 reputation per teleport
- ✅ **Cooldown** - 5 minutes between teleports

---

## 📁 File Structure

```
faction_teleport_integration/
├── README.md                    ← This file
├── config.cpp                   ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── FactionTeleportManager.c  ← Manager
            ├── FactionTeleportNode.c      ← Location
            └── PlayerBase/
                └── playerbase_faction_teleport.c ← Integration
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- `faction_system/` - Tracks player faction and reputation
- `teleportation/` - Teleports player to locations

**Integration:**
1. Player joins faction → Gets access to faction teleports
2. Player earns reputation → Can afford teleportation
3. Player uses teleport → Reputation deducted
4. Faction-specific bases → Only that faction can teleport there

### Faction Teleport Locations

**Blu Faction:**
- Cherno (Main base)
- Electro (Outpost)
- NWAF (Forward base)

**Rossi Faction:**
- Balota (Main base)
- Zelenogorsk (Outpost)
- NEAF (Forward base)

**Cost:** 10 reputation per teleport

---

## 💡 Implementation

### 1. FactionTeleportNode.c

Faction-specific teleport location:

```c
class FactionTeleportNode
{
    string Name;              // "Cherno Base"
    string Faction;           // "Blu"
    vector Position;          // Position
    int ReputationCost;       // 10

    // Check if player can use
    bool CanUse(PlayerBase player)
    {
        FactionManager fm = FactionManager.GetInstance();

        // Check faction
        if (fm.GetFaction(player) != Faction)
            return false;

        // Check reputation
        int rep = fm.GetReputation(player, Faction);
        if (rep < ReputationCost)
            return false;

        // Check cooldown
        if (IsOnCooldown(player))
            return false;

        return true;
    }
}
```

### 2. FactionTeleportManager.c

Manager with faction teleports:

```c
class FactionTeleportManager
{
    ref array<ref FactionTeleportNode> m_Locations;

    void LoadLocations()
    {
        // Load from JSON or define in code
        m_Locations = new array<ref FactionTeleportNode>;

        // Blu faction locations
        m_Locations.Insert(new FactionTeleportNode("Cherno", "Blu", "6500 0 2500", 10));
        m_Locations.Insert(new FactionTeleportNode("Electro", "Blu", "5000 0 2500", 10));

        // Rossi faction locations
        m_Locations.Insert(new FactionTeleportNode("Balota", "Rossi", "6000 0 2500", 10));
    }

    // Teleport player
    bool Teleport(PlayerBase player, string locationName)
    {
        string faction = FactionManager.GetInstance().GetFaction(player);

        foreach (FactionTeleportNode node : m_Locations)
        {
            if (node.Name == locationName && node.Faction == faction)
            {
                if (node.CanUse(player))
                {
                    // Deduct reputation
                    FactionManager.GetInstance().AddReputation(player, faction, -10);

                    // Teleport
                    player.SetPosition(node.Position);

                    // Start cooldown
                    StartCooldown(player);

                    return true;
                }
            }
        }

        return false;
    }
}
```

### 3. PlayerBase Integration

```c
modded class PlayerBase
{
    ref FactionTeleportManager m_TeleportManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Create teleport manager
        m_TeleportManager = new FactionTeleportManager();
        m_TeleportManager.LoadLocations();
    }

    // Chat command: /ftp <location>
    bool ChatCommand_FactionTeleport(string locationName)
    {
        string faction = FactionManager.GetInstance().GetFaction(this);

        if (faction == "")
        {
            NotificationSystem.Notify(this, "You must join a faction first!");
            return false;
        }

        if (m_TeleportManager.Teleport(this, locationName))
        {
            NotificationSystem.Notify(this, "Teleported to " + locationName);
            return true;
        }
        else
        {
            NotificationSystem.Notify(this, "Cannot teleport to " + locationName);
            return false;
        }
    }
}
```

---

## 📝 Usage Examples

### Example 1: Define Faction Bases

```c
void LoadFactionBases()
{
    // Blu faction bases
    AddFactionBase("Blu", "Cherno", Vector(6500, 0, 2500));
    AddFactionBase("Blu", "Electro", Vector(5000, 0, 2500));

    // Rossi faction bases
    AddFactionBase("Rossi", "Balota", Vector(6000, 0, 2500));
    AddFactionBase("Rossi", "Zeleno", Vector(3000, 0, 3000));
}
```

### Example 2: Teleport with Cost

```c
void OnPlayerTeleport(PlayerBase player, string baseName)
{
    int cost = 10;

    if (HasReputation(player, cost))
    {
        // Deduct reputation
        DeductReputation(player, cost);

        // Teleport
        TeleportToBase(player, baseName);

        Print("[FactionTP] " + player.GetIdentity().GetName() + " teleported to " + baseName);
    }
    else
    {
        NotificationSystem.Notify(player, "Not enough reputation! Need " + cost);
    }
}
```

### Example 3: Reputation Requirements

```c
class FactionTeleportNode
{
    int MinReputation = 0;

    bool CanTeleport(PlayerBase player)
    {
        int rep = GetReputation(player, Faction);

        if (rep < MinReputation)
        {
            NotificationSystem.Notify(player, "Need " + MinReputation + " reputation");
            return false;
        }

        return true;
    }
}
```

---

## 🔗 Related Examples

→ [Faction System](../faction_system/) - Individual faction mechanics
→ [Teleportation](../teleportation/) - Individual teleport mechanics
→ [Integration: XP+Storage](../xp_storage_integration/) - Another integration

---

## ⚠️ Important Notes

### Reputation System
- **Cost:** 10 reputation per teleport
- **Minimum:** -50 reputation (hostile) cannot teleport
- **Cooldown:** 5 minutes between teleports

### Faction Bases
Each faction has exclusive access to their bases:
- **Blu:** Cherno, Electro, NWAF
- **Rossi:** Balota, Zelenogorsk, NEAF
- **Verde:** Berezino, Solnichniy
- **Gialli:** Novy, Stary, Novodmitrovsk
- **Hive:** Devil's Castle, Green Mountain

### Chat Commands
- `/join <faction>` - Join faction
- `/ftp <location>` - Faction teleport
- `/rep` - Check reputation

---

*Last updated: 2025-01-18*
*Combines: faction_system + teleportation*
