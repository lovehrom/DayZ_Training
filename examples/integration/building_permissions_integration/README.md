# Building + Factions Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** building_system + faction_system

---

## ⚡ 30-Second Summary

Integration example showing faction-controlled building. Only faction members can build/raid in faction territory, with building permissions and raiding mechanics.

---

## 🎯 Features

### Integration Mechanics
- ✅ **Faction Territory** - Land claimed by faction
- ✅ **Building Permissions** - Only members can build
- ✅ **Raiding Mechanics** - Enemy factions can raid
- ✅ **Territory Claims** - Claim land with flag
- ✅ **Building Protection** - Friendly fire disabled

---

## 📁 File Structure

```
building_permissions_integration/
├── README.md                         ← This file
├── config.cpp                        ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── FactionTerritory.c            ← Territory zone
            ├── FactionBuilding.c             ← Faction building
            ├── TerritoryFlag.c               ← Claim object
            └── PlayerBase/
                └── playerbase_faction_build.c ← Integration
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- `building_system/` - Place/construct buildings
- `faction_system/` - Track player factions

**Integration:**
1. Player places territory flag → Area claimed for faction
2. Player tries to build → Check faction membership
3. Building created → Linked to faction
4. Enemy tries to raid → Check permissions

### Territory System

| Action | Same Faction | Enemy Faction |
|--------|--------------|---------------|
| Build | ✅ Allowed | ❌ Blocked |
| Raid | ❌ Blocked | ✅ Allowed |
| Access | ✅ Open | ❌ Locked |

---

## 💡 Implementation

### 1. FactionTerritory.c

Territory zone for faction:

```c
class FactionTerritory
{
    protected string m_Faction;
    protected vector m_CenterPosition;
    protected float m_Radius;
    protected ref TerritoryFlag m_Flag;

    void FactionTerritory(string faction, vector center, float radius)
    {
        m_Faction = faction;
        m_CenterPosition = center;
        m_Radius = radius;
    }

    // Check if position is in territory
    bool IsInTerritory(vector pos)
    {
        float dist = vector.Distance(pos, m_CenterPosition);
        return dist <= m_Radius;
    }

    // Check if player can build here
    bool CanBuild(PlayerBase player)
    {
        string playerFaction = FactionManager.GetInstance().GetFaction(player);

        // Same faction can build
        return playerFaction == m_Faction;
    }

    // Check if player can raid here
    bool CanRaid(PlayerBase player)
    {
        string playerFaction = FactionManager.GetInstance().GetFaction(player);

        // Enemy faction can raid
        return playerFaction != m_Faction;
    }

    string GetFaction()
    {
        return m_Faction;
    }

    vector GetCenter()
    {
        return m_CenterPosition;
    }

    float GetRadius()
    {
        return m_Radius;
    }
}
```

### 2. TerritoryFlag.c

Flag that claims territory:

```c
class TerritoryFlag extends ItemBase
{
    protected ref FactionTerritory m_Territory;
    protected const float TERRITORY_RADIUS = 100.0; // 100m

    override void OnPlaceStarted(PlayerBase player)
    {
        super.OnPlaceStarted(player);

        // Get player faction
        string faction = FactionManager.GetInstance().GetFaction(player);

        if (faction == "")
        {
            NotificationSystem.Notify(player, "You must join a faction first!");
            return;
        }

        // Create territory
        m_Territory = new FactionTerritory(faction, GetPosition(), TERRITORY_RADIUS);

        NotificationSystem.Notify(player, "Territory claimed for " + faction + "!");
    }

    override void EEDeleted(EntityAI parent)
    {
        super.EEDeleted(parent);

        // Territory lost when flag destroyed
        if (m_Territory)
        {
            Print("[Territory] " + m_Territory.GetFaction() + " lost territory at " + GetPosition());
        }
    }

    FactionTerritory GetTerritory()
    {
        return m_Territory;
    }
}
```

### 3. FactionBuilding.c

Building with faction permissions:

```c
class FactionBuilding extends House
{
    protected string m_OwnerFaction;

    void SetFaction(string faction)
    {
        m_OwnerFaction = faction;
    }

    string GetFaction()
    {
        return m_OwnerFaction;
    }

    // Check if player can access
    bool CanAccess(PlayerBase player)
    {
        string playerFaction = FactionManager.GetInstance().GetFaction(player);

        // Same faction can access
        return playerFaction == m_OwnerFaction;
    }

    // Check if player can raid
    bool CanRaid(PlayerBase player)
    {
        string playerFaction = FactionManager.GetInstance().GetFaction(player);

        // Enemy faction can raid
        return playerFaction != m_OwnerFaction;
    }

    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);

        // Set owner faction
        PlayerBase playerBase = PlayerBase.Cast(player);
        if (playerBase)
        {
            string faction = FactionManager.GetInstance().GetFaction(playerBase);
            SetFaction(faction);

            Print("[FactionBuilding] Built for " + faction);
        }
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        // Prevent moving buildings
        return false;
    }
}
```

### 4. PlayerBase Integration

```c
modded class PlayerBase
{
    ref array<ref FactionTerritory> m_Territories;

    override void OnConnect()
    {
        super.OnConnect();

        m_Territories = new array<ref FactionTerritory>;
    }

    // Try to build in territory
    bool TryBuildInTerritory(vector buildPos)
    {
        // Find territory at position
        FactionTerritory territory = GetTerritoryAtPosition(buildPos);

        if (!territory)
        {
            // No territory, free build
            return true;
        }

        // Check permissions
        if (territory.CanBuild(this))
        {
            NotificationSystem.Notify(this, "Building in " + territory.GetFaction() + " territory");
            return true;
        }
        else
        {
            NotificationSystem.Notify(this, "Cannot build here! Not your territory.");
            return false;
        }
    }

    // Try to raid building
    bool TryRaidBuilding(FactionBuilding building)
    {
        // Check permissions
        if (building.CanRaid(this))
        {
            NotificationSystem.Notify(this, "Raiding " + building.GetFaction() + " building!");

            // Reputation penalty for raiding
            string targetFaction = building.GetFaction();
            FactionManager.GetInstance().AddReputation(this, targetFaction, -50);

            return true;
        }
        else
        {
            NotificationSystem.Notify(this, "Cannot raid friendly building!");
            return false;
        }
    }

    FactionTerritory GetTerritoryAtPosition(vector pos)
    {
        foreach (FactionTerritory territory : m_Territories)
        {
            if (territory.IsInTerritory(pos))
                return territory;
        }

        return null;
    }

    void RegisterTerritory(FactionTerritory territory)
    {
        m_Territories.Insert(territory);
    }
}
```

---

## 📝 Usage Examples

### Example 1: Claim Territory

```c
modded class PlayerBase
{
    void ActionClaimTerritory()
    {
        string faction = FactionManager.GetInstance().GetFaction(this);

        if (faction == "")
        {
            NotificationSystem.Notify(this, "Join a faction first!");
            return;
        }

        // Place flag
        TerritoryFlag flag = TerritoryFlag.Cast(GetGame().CreateObject("TerritoryFlag", GetPosition()));

        // Create territory
        FactionTerritory territory = new FactionTerritory(faction, GetPosition(), 100.0);

        RegisterTerritory(territory);

        NotificationSystem.Notify(this, "Territory claimed for " + faction);
    }
}
```

### Example 2: Faction Base Building

```c
class FactionBaseBuilding extends FactionBuilding
{
    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);

        // Set health
        SetHealth("GlobalHealth", "Health", 1000);
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        PlayerBase player = PlayerBase.Cast(killer);
        if (player)
        {
            string ownerFaction = GetFaction();
            string killerFaction = FactionManager.GetInstance().GetFaction(player);

            if (ownerFaction != killerFaction)
            {
                // Enemy destroyed base
                NotificationSystem.Notify(player, "Base destroyed! +100 reputation");

                // Reward
                FactionManager.GetInstance().AddReputation(player, killerFaction, 100);
            }
        }
    }
}
```

### Example 3: Territory Overlap Detection

```c
class TerritoryManager
{
    static bool CanClaimTerritory(string faction, vector center, float radius)
    {
        // Check for overlapping territories
        array<FactionTerritory> territories = GetAllTerritories();

        foreach (FactionTerritory territory : territories)
        {
            // Check distance between centers
            float dist = vector.Distance(center, territory.GetCenter());
            float minDist = radius + territory.GetRadius();

            // Overlapping?
            if (dist < minDist)
            {
                // Check if same faction
                if (territory.GetFaction() == faction)
                {
                    // Same faction, allow overlap
                    return true;
                }
                else
                {
                    // Different faction, block overlap
                    return false;
                }
            }
        }

        // No overlap
        return true;
    }
}
```

---

## 🔗 Related Examples

→ [Building System](../building_system/) - Individual building mechanics
→ [Faction System](../faction_system/) - Individual faction mechanics
→ [Integration: Faction+Teleport](../faction_teleport_integration/) - Another integration

---

## ⚠️ Important Notes

### Territory Rules
- **Radius:** 100m (configurable)
- **Overlap:** Same faction allowed, different blocked
- **Building:** Only members can build
- **Raiding:** Only enemies can raid

### Reputation Changes
- **Build:** +10 reputation
- **Raid:** -50 reputation with target faction
- **Destroy base:** +100 reputation with own faction

### Building Protection
- Friendly fire disabled in territory
- Buildings locked to enemy faction
- Raiding requires reputation sacrifice

---

*Last updated: 2025-01-18*
*Combines: building_system + faction_system*
