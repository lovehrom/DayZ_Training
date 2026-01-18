# Full Mod Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Expert
**Combines:** ALL systems (XP, Storage, Teleport, Factions, AI, Loot, Time, Vehicle, Building, Crafting, Economy, Notifications, Modifiers)

---

## ⚡ 30-Second Summary

Complete mod integration example showing how ALL mechanics work together. This is a full-featured DayZ mod with progression, persistence, social systems, and gameplay mechanics.

---

## 🎯 Features

### Integrated Systems
- ✅ **Faction System** - 5 factions with reputation
- ✅ **XP System** - 3 skill trees with persistence
- ✅ **Storage System** - Cross-server item storage
- ✅ **Teleportation** - Faction-specific bases
- ✅ **AI System** - NPC bandits with loot
- ✅ **Loot System** - Weighted random drops
- ✅ **Time System** - Day/night events
- ✅ **Vehicle System** - Packable vehicles with cargo
- ✅ **Building System** - Faction territories
- ✅ **Crafting System** - Economy-based crafting
- ✅ **Notification System** - Server-wide alerts
- ✅ **Modifier System** - Stacking effects

---

## 📁 File Structure

```
full_mod_integration/
├── README.md                              ← This file
├── config.cpp                             ← Mod configuration
└── scripts/
    ├── 3_Game/
    │   ├── FullModManager.c               ← Main manager
    │   ├── FactionSystem/                 ← Faction logic
    │   ├── XPSystem/                      ← XP logic
    │   ├── StorageSystem/                 ← Storage logic
    │   ├── TeleportSystem/                ← Teleport logic
    │   ├── AISystem/                      ← AI logic
    │   ├── LootSystem/                    ← Loot logic
    │   ├── TimeSystem/                    ← Time logic
    │   ├── VehicleSystem/                 ← Vehicle logic
    │   ├── BuildingSystem/                ← Building logic
    │   ├── CraftingSystem/                ← Crafting logic
    │   ├── EconomySystem/                 ← Economy logic
    │   ├── NotificationSystem/            → Notification logic
    │   └── ModifierSystem/                ← Modifier logic
    └── 4_World/
        └── Classes/
            └── PlayerBase/
                └── playerbase_full_mod.c  ← Player integration
```

---

## 🔧 Integration Concept

### How All Systems Work Together

```
Player Joins
    ↓
Load XP Progress (Storage)
    ↓
Load Faction Data (Storage)
    ↓
Load Economy Balance (Storage)
    ↓
Player Gameplay Loop
    ↓
Kill Zombie → XP + Loot → Reputation (Faction) → Notification
    ↓
Level Up → Modifiers → Notification
    ↓
Craft Items → Economy Cost → XP → Notification
    ↓
Build Base → Faction Territory → Storage
    ↓
Teleport → Reputation Cost → Notification
    ↓
Vehicle Pack → Cargo Storage
    ↓
Time Event → Spawns → AI → Loot
    ↓
Player Leaves → Save All Data (Storage)
```

### System Dependencies

| System | Depends On | Used By |
|--------|------------|---------|
| Storage | - | ALL |
| Notifications | - | ALL |
| XP | Storage | Modifiers, Achievements |
| Factions | Storage | Teleport, Building, Economy |
| Teleport | Factions, Storage | Economy |
| AI | Loot, Time | XP, Factions |
| Loot | - | AI, Crafting |
| Time | - | AI, Spawning |
| Vehicle | Storage | Economy |
| Building | Factions, Storage | Economy |
| Crafting | Economy | XP |
| Economy | Storage | Crafting, Teleport |
| Modifiers | XP | Combat, Movement |

---

## 💡 Implementation

### 1. FullModManager.c

Main manager coordinating all systems:

```c
class FullModManager
{
    // System managers
    protected ref FactionManager m_FactionManager;
    protected ref SkillManager m_SkillManager;
    protected ref StorageManager m_StorageManager;
    protected ref TeleportManager m_TeleportManager;
    protected ref AIManager m_AIManger;
    protected ref LootManager m_LootManager;
    protected ref TimeManager m_TimeManager;
    protected ref VehicleManager m_VehicleManager;
    protected ref BuildingManager m_BuildingManager;
    protected ref CraftingManager m_CraftingManager;
    protected ref EconomyManager m_EconomyManager;
    protected ref NotificationManager m_NotificationManager;
    protected ref ModifierManager m_ModifierManager;

    protected static ref FullModManager m_Instance;

    static FullModManager GetInstance()
    {
        if (!m_Instance)
            m_Instance = new FullModManager();

        return m_Instance;
    }

    void FullModManager()
    {
        // Initialize all managers
        m_FactionManager = new FactionManager();
        m_SkillManager = new SkillManager();
        m_StorageManager = new StorageManager();
        m_TeleportManager = new TeleportManager();
        m_AIManger = new AIManager();
        m_LootManager = new LootManager();
        m_TimeManager = new TimeManager();
        m_VehicleManager = new VehicleManager();
        m_BuildingManager = new BuildingManager();
        m_CraftingManager = new CraftingManager();
        m_EconomyManager = new EconomyManager();
        m_NotificationManager = new NotificationManager();
        m_ModifierManager = new ModifierManager();
    }

    // Load player data
    void LoadPlayerData(PlayerBase player)
    {
        string steam64ID = player.GetIdentity().GetId();

        // Load XP
        m_SkillManager.LoadPlayer(steam64ID);

        // Load faction
        m_FactionManager.LoadPlayer(steam64ID);

        // Load economy
        m_EconomyManager.LoadPlayer(steam64ID);

        // Load modifiers
        m_ModifierManager.LoadPlayer(steam64ID);

        Print("[FullMod] Loaded data for " + player.GetIdentity().GetName());
    }

    // Save player data
    void SavePlayerData(PlayerBase player)
    {
        string steam64ID = player.GetIdentity().GetId();

        // Save XP
        m_SkillManager.SavePlayer(steam64ID);

        // Save faction
        m_FactionManager.SavePlayer(steam64ID);

        // Save economy
        m_EconomyManager.SavePlayer(steam64ID);

        // Save modifiers
        m_ModifierManager.SavePlayer(steam64ID);

        Print("[FullMod] Saved data for " + player.GetIdentity().GetName());
    }

    // Getters for managers
    FactionManager GetFactionManager() { return m_FactionManager; }
    SkillManager GetSkillManager() { return m_SkillManager; }
    StorageManager GetStorageManager() { return m_StorageManager; }
    TeleportManager GetTeleportManager() { return m_TeleportManager; }
    AIManager GetAIManger() { return m_AIManger; }
    LootManager GetLootManager() { return m_LootManager; }
    TimeManager GetTimeManager() { return m_TimeManager; }
    VehicleManager GetVehicleManager() { return m_VehicleManager; }
    BuildingManager GetBuildingManager() { return m_BuildingManager; }
    CraftingManager GetCraftingManager() { return m_CraftingManager; }
    EconomyManager GetEconomyManager() { return m_EconomyManager; }
    NotificationManager GetNotificationManager() { return m_NotificationManager; }
    ModifierManager GetModifierManager() { return m_ModifierManager; }
}
```

### 2. PlayerBase Integration

```c
modded class PlayerBase
{
    protected FullModManager m_ModManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Get mod manager
        m_ModManager = FullModManager.GetInstance();

        // Load player data
        m_ModManager.LoadPlayerData(this);

        // Welcome message
        string faction = m_ModManager.GetFactionManager().GetFaction(this);
        int level = m_ModManager.GetSkillManager().GetLevel("Survival");
        int currency = m_ModManager.GetEconomyManager().GetCurrency(GetIdentity().GetId());

        NotificationSystem.Notify(this, "Welcome to Full Mod!");
        NotificationSystem.Notify(this, "Faction: " + faction + " | Level: " + level + " | Coins: " + currency);
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save player data
        if (m_ModManager)
        {
            m_ModManager.SavePlayerData(this);
        }
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Check if killed by player
        PlayerBase player = PlayerBase.Cast(killer);
        if (player)
        {
            // Add XP
            player.m_ModManager.GetSkillManager().AddXP("Combat", 10);

            // Add reputation (if enemy faction)
            string victimFaction = m_ModManager.GetFactionManager().GetFaction(this);
            string killerFaction = m_ModManager.GetFactionManager().GetFaction(player);

            if (victimFaction != killerFaction)
            {
                player.m_ModManager.GetFactionManager().AddReputation(player, killerFaction, 20);

                NotificationSystem.Notify(player, "Killed enemy! +20 reputation");
            }

            // Drop loot
            m_ModManager.GetLootManager().DropLoot(this, player.GetPosition());

            // Check achievements
            player.m_ModManager.GetSkillManager().CheckAchievements(player);
        }
    }

    override void OnItemInHandsChanged(EntityAI old_item, EntityAI new_item)
    {
        super.OnItemInHandsChanged(old_item, new_item);

        // Update modifiers based on items
        if (new_item)
        {
            m_ModManager.GetModifierManager().OnItemEquipped(this, ItemBase.Cast(new_item));
        }
    }

    // Get mod manager
    FullModManager GetModManager()
    {
        return m_ModManager;
    }
}
```

### 3. Cross-System Interactions

```c
class CrossSystemInteractions
{
    // Example 1: Kill zombie → XP + Loot + Reputation + Notification
    static void OnZombieKilled(PlayerBase player, EntityAI zombie)
    {
        FullModManager manager = player.GetModManager();

        // Add XP
        manager.GetSkillManager().AddXP("Combat", 10);

        // Drop loot
        manager.GetLootManager().DropZombieLoot(zombie, zombie.GetPosition());

        // Add faction reputation
        string faction = manager.GetFactionManager().GetFaction(player);
        if (faction != "")
        {
            manager.GetFactionManager().AddReputation(player, faction, 5);
        }

        // Notify
        NotificationSystem.Notify(player, "Zombie killed! +10 XP");

        // Check achievements
        manager.GetSkillManager().CheckAchievement(player, "first_blood");
    }

    // Example 2: Craft item → Economy cost + XP + Notification
    static void OnItemCrafted(PlayerBase player, string itemName)
    {
        FullModManager manager = player.GetModManager();

        // Check currency
        int cost = manager.GetCraftingManager().GetRecipeCost(itemName);
        int balance = manager.GetEconomyManager().GetCurrency(player.GetIdentity().GetId());

        if (balance < cost)
        {
            NotificationSystem.Notify(player, "Cannot afford " + itemName);
            return;
        }

        // Deduct currency
        manager.GetEconomyManager().AddCurrency(player.GetIdentity().GetId(), -cost);

        // Add XP
        manager.GetSkillManager().AddXP("Crafting", 50);

        // Craft
        manager.GetCraftingManager().CraftItem(player, itemName);

        // Notify
        NotificationSystem.Notify(player, "Crafted " + itemName + " for " + cost + " coins");
    }

    // Example 3: Teleport → Reputation cost + Storage check + Notification
    static void OnPlayerTeleport(PlayerBase player, string location)
    {
        FullModManager manager = player.GetModManager();

        // Check faction
        string faction = manager.GetFactionManager().GetFaction(player);
        if (faction == "")
        {
            NotificationSystem.Notify(player, "Join a faction first!");
            return;
        }

        // Check reputation
        int reputation = manager.GetFactionManager().GetReputation(player, faction);
        int cost = 10;

        if (reputation < cost)
        {
            NotificationSystem.Notify(player, "Need " + cost + " reputation to teleport");
            return;
        }

        // Deduct reputation
        manager.GetFactionManager().AddReputation(player, faction, -cost);

        // Save current position for storage
        manager.GetStorageManager().SetLastPosition(player.GetIdentity().GetId(), player.GetPosition());

        // Teleport
        vector pos = manager.GetTeleportManager().GetFactionLocation(faction, location);
        player.SetPosition(pos);

        // Notify
        NotificationSystem.Notify(player, "Teleported to " + location + " (-" + cost + " rep)");

        // Broadcast
        manager.GetNotificationManager().Broadcast(player.GetIdentity().GetName() + " teleported to " + location);
    }

    // Example 4: Time event → Spawns + AI + Loot
    static void OnTimeEvent(float gameTime)
    {
        FullModManager manager = FullModManager.GetInstance();

        // Check if night
        bool isNight = gameTime >= 20.0 || gameTime < 6.0;

        if (isNight)
        {
            // Spawn night mobs
            manager.GetAIManger().SpawnNightMobs();

            // Spawn special loot
            manager.GetLootManager().SpawnNightLoot();
        }
        else
        {
            // Despawn night mobs
            manager.GetAIManger().DespawnNightMobs();
        }

        // Check dawn/dusk
        if (gameTime >= 6.0 && gameTime < 6.1)
        {
            // Dawn event
            manager.GetNotificationManager().Broadcast("Dawn has broken!");

            // Spawn dawn mobs
            manager.GetAIManger().SpawnDawnMobs();
        }

        if (gameTime >= 18.0 && gameTime < 18.1)
        {
            // Dusk event
            manager.GetNotificationManager().Broadcast("Dusk has fallen!");

            // Spawn dusk mobs
            manager.GetAIManger().SpawnDuskMobs();
        }
    }

    // Example 5: Build base → Faction territory + Storage + Notification
    static void OnBuildingBuilt(PlayerBase player, EntityAI building)
    {
        FullModManager manager = player.GetModManager();

        // Check faction
        string faction = manager.GetFactionManager().GetFaction(player);
        if (faction == "")
        {
            NotificationSystem.Notify(player, "Join a faction to build!");
            GetGame().ObjectDelete(building);
            return;
        }

        // Check territory
        vector pos = building.GetPosition();
        if (!manager.GetBuildingManager().CanBuildInTerritory(player, pos))
        {
            NotificationSystem.Notify(player, "Cannot build here!");
            GetGame().ObjectDelete(building);
            return;
        }

        // Set owner faction
        manager.GetBuildingManager().SetBuildingFaction(building, faction);

        // Save to storage
        manager.GetStorageManager().SaveBuilding(player.GetIdentity().GetId(), building);

        // Add XP
        manager.GetSkillManager().AddXP("Building", 100);

        // Notify
        NotificationSystem.Notify(player, "Built for " + faction + "! +100 XP");

        // Broadcast
        manager.GetNotificationManager().Broadcast(player.GetIdentity().GetName() + " built a structure for " + faction);
    }

    // Example 6: Vehicle pack → Cargo storage + Notification
    static void OnVehiclePacked(PlayerBase player, Car vehicle)
    {
        FullModManager manager = player.GetModManager();

        // Save cargo
        string vehicleID = manager.GetVehicleManager().GetVehicleID(vehicle);
        manager.GetStorageManager().SaveVehicleCargo(vehicleID, vehicle);

        // Delete vehicle
        GetGame().ObjectDelete(vehicle);

        // Give kit
        EntityAI kit = GetGame().CreateObject("CarKit", player.GetPosition());
        player.GetInventory().AddInInventory(kit);

        // Notify
        NotificationSystem.Notify(player, "Vehicle packed! Cargo saved.");
    }
}
```

### 4. Chat Commands

```c
modded class PlayerBase
{
    // Chat command handler
    override bool ChatCommand(string command)
    {
        array<string> args = new array<string>;
        command.Split(" ", args);

        string cmd = args.Get(0);

        switch (cmd)
        {
            case "/join":
                // Join faction
                if (args.Count() > 1)
                {
                    string faction = args.Get(1);
                    m_ModManager.GetFactionManager().JoinFaction(this, faction);
                }
                return true;

            case "/rep":
                // Check reputation
                string myFaction = m_ModManager.GetFactionManager().GetFaction(this);
                int reputation = m_ModManager.GetFactionManager().GetReputation(this, myFaction);
                NotificationSystem.Notify(this, "Reputation with " + myFaction + ": " + reputation);
                return true;

            case "/tp":
                // Teleport
                if (args.Count() > 1)
                {
                    string location = args.Get(1);
                    CrossSystemInteractions::OnPlayerTeleport(this, location);
                }
                return true;

            case "/craft":
                // Craft item
                if (args.Count() > 1)
                {
                    string item = args.Get(1);
                    CrossSystemInteractions::OnItemCrafted(this, item);
                }
                return true;

            case "/money":
                // Check balance
                int balance = m_ModManager.GetEconomyManager().GetCurrency(GetIdentity().GetId());
                NotificationSystem.Notify(this, "Balance: " + balance + " coins");
                return true;

            case "/stats":
                // Show stats
                int level = m_ModManager.GetSkillManager().GetLevel("Survival");
                int xp = m_ModManager.GetSkillManager().GetXP("Survival");
                NotificationSystem.Notify(this, "Level: " + level + " | XP: " + xp);
                return true;

            case "/achievements":
                // Show achievements
                m_ModManager.GetSkillManager().ShowAchievements(this);
                return true;

            case "/modifiers":
                // Show active modifiers
                m_ModManager.GetModifierManager().ShowModifiers(this);
                return true;

            default:
                return super.ChatCommand(command);
        }
    }
}
```

---

## 📝 Usage Examples

### Example 1: Complete Player Session

```
1. Player joins → Load all data
2. Player kills zombie → +10 XP, loot, +5 rep
3. Player crafts AK74 → -100 coins, +50 XP
4. Player teleports → -10 rep, move to base
5. Player builds house → Territory check, +100 XP
6. Player leaves → Save all data
```

### Example 2: Faction War Event

```c
class FactionWarEvent
{
    static void StartWar(string faction1, string faction2)
    {
        FullModManager manager = FullModManager.GetInstance();

        // Set as enemies
        manager.GetFactionManager().SetRelation(faction1, faction2, "Enemy");

        // Broadcast
        manager.GetNotificationManager().Broadcast("WAR! " + faction1 + " vs " + faction2);

        // Bonus reputation for kills
        manager.GetFactionManager().SetWarMultiplier(faction1, faction2, 2.0);

        // Spawn special mobs
        manager.GetAIManger().SpawnWarMobs(faction1, faction2);
    }
}
```

### Example 3: Economy Event

```c
class EconomyEvent
{
    static void StartSale()
    {
        FullModManager manager = FullModManager.GetInstance();

        // Reduce all prices by 50%
        manager.GetCraftingManager().SetGlobalPriceMultiplier(0.5);

        // Broadcast
        manager.GetNotificationManager().Broadcast("SALE! All crafting 50% off!");

        // Schedule end
        GetGame().GetCallQueue().CallLater(EndSale, 300000); // 5 minutes
    }

    static void EndSale()
    {
        FullModManager manager = FullModManager.GetInstance();

        // Reset prices
        manager.GetCraftingManager().ResetPriceMultiplier();

        manager.GetNotificationManager().Broadcast("Sale ended!");
    }
}
```

---

## 🔗 Related Examples

→ [All Integration Examples](../integration/) - Individual integrations
→ [XP + Storage](../xp_storage_integration/) - Storage integration
→ [Faction + Teleport](../faction_teleport_integration/) - Faction integration

---

## ⚠️ Important Notes

### System Load Order
1. Storage (must be first)
2. Notifications (no dependencies)
3. Economy (no dependencies)
4. XP (depends on Storage)
5. Factions (depends on Storage)
6. Modifiers (depends on XP)
7. All others

### Performance Considerations
- **Save frequency:** Every 5 minutes (configurable)
- **Update frequency:** 10 ticks per second
- **Network traffic:** Minimize RPC calls
- **Memory usage:** ~10-50 MB per player

### Testing
- Test each system individually
- Test pairwise integrations
- Test full system load
- Stress test with 50+ players

---

*Last updated: 2025-01-18*
*Combines: ALL systems*
