# DayZ Modding Architecture - Complete Guide

**Status:** ✅ Beginner Guide
**Difficulty:** Beginner
**Time to Learn:** 45 minutes
**Related:** [EnScript Basics](03_enscript_basics.md) | [Config](02_config.md)

---

## ⚡ 30-Second Summary

DayZ modding follows a **layered architecture** with 3 main layers: Game (logic), World (entities), Mission (player interaction). Understanding this architecture is critical for creating efficient mods.

---

## 🎯 What You'll Learn

1. **DayZ Architecture Overview** - How the game is structured
2. **3-Game/4-World/5-Mission System** - File organization
3. **Class Hierarchy** - EntityAI → ItemBase → PlayerBase
4. **Execution Flow** - How code runs from startup to gameplay
5. **Client-Server Model** - Multiplayer architecture
6. **Mod Structure** - How to organize your mod files

---

## 💡 Architecture Overview

### The 3-Layer System

DayZ uses a **3-layer architecture** for code organization:

```
┌─────────────────────────────────────┐
│  3_Game/     (Game Logic)           │  ← Managers, Systems, Core logic
├─────────────────────────────────────┤
│  4_World/    (World Entities)       │  ← Items, NPCs, Buildings, Vehicles
├─────────────────────────────────────┤
│  5_Mission/  (Mission & Player)     │  ← UI, Player interaction, Game modes
└─────────────────────────────────────┘
```

**Why 3 layers?**
- **Separation of concerns** - Each layer has a specific job
- **Performance** - Game logic separate from world entities
- **Maintainability** - Easy to find where code belongs

---

## 🔧 Layer 1: 3_Game/ (Game Logic)

**Purpose:** Core game systems and managers

**What goes here:**
- **Managers** - XP system, Banking system, Quest manager
- **System Classes** - Centralized logic
- **Configuration** - Global settings
- **Utilities** - Helper functions

**Examples:**
```
scripts/3_Game/
├── DayZGame.c              ← Main game class
├── GameConstants.c         ← Global constants
├── Systems/
│   ├── XPManager.c         ← XP system manager
│   ├── BankingManager.c    ← Banking system manager
│   └── QuestManager.c      ← Quest system manager
└── Helpers/
    ├── JsonHelper.c        ← JSON utilities
    └── MathHelper.c        ← Math utilities
```

**Key Classes:**
- `DayZGame` - Main game controller
- `GameConstants` - Global configuration
- Custom managers (XP, Banking, etc.)

**When to use 3_Game/:**
- ✅ Creating system managers (XP, Banking, etc.)
- ✅ Global configuration
- ✅ Utility functions
- ❌ NOT for individual entities (use 4_World/)
- ❌ NOT for UI (use 5_Mission/)

---

## 🔧 Layer 2: 4_World/ (World Entities)

**Purpose:** Game entities that exist in the world

**What goes here:**
- **Items** - Weapons, food, clothing, tools
- **NPCs** - Zombies, animals, traders
- **Buildings** - Base buildings, structures
- **Vehicles** - Cars, boats, helicopters
- **Containers** - Tents, chests, storage

**Examples:**
```
scripts/4_World/
├── Classes/
│   ├── User/
│   │   ├── WeaponBase.c/     ← Custom weapons
│   │   ├── Edible_Base.c/    ← Custom food
│   │   └── Clothing_Base.c/  ← Custom clothes
│   ├── Man.c/                ← Player modifications
│   └── ZombieBase.c/         ← Custom zombies
└── Entities/
    ├── BuildingBase.c/       ← Custom buildings
    └── CarScript.c/          ← Custom vehicles
```

**Key Classes:**
- `ItemBase` - Base class for all items
- `Man` - Player class
- `ZombieBase` - Zombie class
- `BuildingBase` - Building class
- `CarScript` - Vehicle class

**When to use 4_World/:**
- ✅ Creating custom items
- ✅ Creating custom NPCs
- ✅ Creating custom buildings/vehicles
- ❌ NOT for system managers (use 3_Game/)
- ❌ NOT for UI (use 5_Mission/)

---

## 🔧 Layer 3: 5_Mission/ (Mission & Player)

**Purpose:** Player interaction and UI

**What goes here:**
- **UI Menus** - All user interfaces
- **Player Actions** - Custom actions
- **Mission Controllers** - Game modes
- **Input Handling** - Keyboard/mouse input

**Examples:**
```
scripts/5_Mission/
├── MissionGameplay.c        ← Main mission controller
├── Menus/
│   ├── TradingMenu.c        ← Trader UI
│   ├── BankingMenu.c        ← ATM UI
│   └── QuestJournal.c       ← Quest UI
└── Inputs/
    └── CustomActions.c      ← Custom player actions
```

**Key Classes:**
- `MissionGameplay` - Main mission controller
- `UIScriptedMenu` - Base class for UI
- `ActionBase` - Base class for actions

**When to use 5_Mission/:**
- ✅ Creating UI menus
- ✅ Creating custom actions
- ✅ Mission-specific gameplay
- ❌ NOT for system managers (use 3_Game/)
- ❌ NOT for entities (use 4_World/)

---

## 📐 Class Hierarchy

### EntityAI Hierarchy

```
EntityAI (root entity class)
├── ItemBase (all items)
│   ├── WeaponBase (weapons)
│   ├── Edible_Base (food)
│   └── Clothing_Base (clothes)
├── Man (player)
├── ZombieBase (zombies)
├── AnimalBase (animals)
├── BuildingBase (buildings)
└── CarScript (vehicles)
```

**Key Points:**
- `EntityAI` is the **base class** for ALL entities
- `ItemBase` inherits from `EntityAI`
- `Man` (player) inherits from `EntityAI`
- Everything in the world is an `EntityAI`

---

## 🔄 Execution Flow

### Server Startup

```
1. DayZGame::OnLaunch()
   ↓
2. Init() scripts load
   ↓
3. Create Custom Servers
   ↓
4. MissionGameplay::OnMissionStart()
   ↓
5. World entities spawn
   ↓
6. Players connect
```

### Player Connection

```
1. Player connects
   ↓
2. PlayerIdentity created
   ↓
3. Man (player) created
   ↓
4. PlayerBase::OnConnect()
   ↓
5. Load player data (JSON)
   ↓
6. Player spawns in world
   ↓
7. Game loop starts
```

### Game Loop (Every Frame)

```
1. EOnFrame() called
   ↓
2. Update systems (managers)
   ↓
3. Update entities (items, NPCs)
   ↓
4. Update UI
   ↓
5. Repeat
```

---

## 🌐 Client-Server Model

### DayZ Multiplayer Architecture

```
┌─────────────────┐         ┌─────────────────┐
│   Client Side   │         │   Server Side   │
├─────────────────┤         ├─────────────────┤
│ • UI Display    │  ◄───►  │ • Game Logic    │
│ • Input         │   RPC   │ • Authority     │
│ • Prediction    │         │ • World State   │
│ • Effects       │         │ • Validation    │
└─────────────────┘         └─────────────────┘
```

**Key Concepts:**

**Client (Player's computer):**
- Renders UI
- Handles input
- Shows visual effects
- **NO authority** (asks server for permission)

**Server (Host machine):**
- **Has authority** (makes decisions)
- Validates actions
- Manages world state
- Saves data

**Communication:**
- **RPC** (Remote Procedure Call) - Client talks to server
- Example: Client presses "F" to pick up item → Server validates → Server gives item

---

## 📁 Mod Structure

### Complete Mod Structure

```
@MyMod/
├── config.cpp                 ← Mod definition
├── scripts/
│   ├── 3_Game/
│   │   └── Systems/
│   │       └── MyManager.c    ← System manager
│   ├── 4_World/
│   │   └── Classes/
│   │       ├── User/
│   │       │   └── MyItem.c   ← Custom item
│   │       └── Man.c/
│   │           └── player.c   ← Player modifications
│   └── 5_Mission/
│       └── Menus/
│           └── MyMenu.c       ← Custom UI
└── data/
    └── config.json            ← Configuration
```

### config.cpp Structure

```cpp
class CfgPatches
{
    class MyMod
    {
        units[] = {"MyItem"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgVehicles
{
    class ItemBase;
    class MyItem: ItemBase
    {
        scope = 2;
        displayName = "My Custom Item";
        // ... item properties
    };
};
```

---

## 🔑 Key Architecture Principles

### 1. Separation of Concerns

**Each layer has ONE job:**
- **3_Game/** = Game logic ONLY
- **4_World/** = World entities ONLY
- **5_Mission/** = Player/UI ONLY

**Example:**
```c
// ❌ BAD: UI in 4_World/
scripts/4_World/Menus/MyMenu.c  // WRONG!

// ✅ GOOD: UI in 5_Mission/
scripts/5_Mission/Menus/MyMenu.c  // CORRECT!
```

---

### 2. Single Responsibility

**Each class has ONE responsibility:**

**❌ BAD:**
```c
class XPManager
{
    void AddXP() { }
    void CreateItem() { }  // WRONG! Not XP's job
    void ShowUI() { }      // WRONG! Not XP's job
};
```

**✅ GOOD:**
```c
class XPManager  // ONLY manages XP
{
    void AddXP() { }
};

class ItemFactory  // ONLY creates items
{
    void CreateItem() { }
};

class XPUI  // ONLY shows UI
{
    void ShowUI() { }
};
```

---

### 3. Server Authority

**Server makes ALL decisions:**

**❌ BAD:**
```c
// Client gives itself XP
void AddXP(int amount)
{
    m_XP += amount;  // Client cheating!
}
```

**✅ GOOD:**
```c
// Client requests XP
void RequestXP(int amount)
{
    // Send RPC to server
    GetRPCManager().SendRPC("MyMod", "AddXP", new Param1<int>(amount), true);
}

// Server validates and gives XP
void Server_AddXP(PlayerIdentity sender, int amount)
{
    if (GetGame().IsServer())
    {
        // Validate
        if (amount > 1000) return;  // Max 1000 XP

        // Give XP
        m_XP[sender.GetId()] += amount;
    }
}
```

---

### 4. Modularity

**Keep systems INDEPENDENT:**

**❌ BAD:**
```c
class XPManager
{
    void AddXP()
    {
        // Direct dependency on Banking
        GetBankingManager().Deposit(...);  // WRONG!
    }
};
```

**✅ GOOD:**
```c
class XPManager
{
    void AddXP()
    {
        // Use events (loose coupling)
        GetEventBus().Trigger("OnXPGained", data);
    }
};

class BankingManager
{
    void Init()
    {
        // Listen for events
        GetEventBus().Register("OnXPGained", this);
    }
}
```

---

## 🎯 When to Use Each Layer

### Decision Tree

```
What are you creating?
│
├─ System Manager (XP, Banking, Quests)?
│  └─► 3_Game/Systems/
│
├─ Entity (Item, NPC, Building, Vehicle)?
│  ├─► 4_World/Classes/User/     (if custom item/NPC)
│  ├─► 4_World/Classes/Man.c/     (if player mod)
│  └─► 4_World/Entities/         (if building/vehicle)
│
└─ UI Menu or Player Action?
   └─► 5_Mission/Menus/ or 5_Mission/Inputs/
```

### Examples

| What You Want | Where to Put It |
|---------------|----------------|
| XP System | `3_Game/Systems/XPManager.c` |
| Custom Sword | `4_World/Classes/User/Sword.c` |
| ATM UI | `5_Mission/Menus/ATMMenu.c` |
| Player Speed Boost | `4_World/Classes/Man.c/player.c` |
| Quest Manager | `3_Game/Systems/QuestManager.c` |
| Trader NPC | `4_World/Classes/User/TraderNPC.c` |

---

## 📝 Quick Reference

### File Organization

```
scripts/
├── 3_Game/          ← Managers, Systems, Core logic
│   └── Systems/
│       └── MyManager.c
├── 4_World/         ← Items, NPCs, Buildings, Vehicles
│   └── Classes/
│       ├── User/    ← Custom items/NPCs
│       └── Man.c/   ← Player modifications
└── 5_Mission/       ← UI, Actions, Mission
    └── Menus/
        └── MyMenu.c
```

### Class Hierarchy

```
EntityAI (root)
├── ItemBase (items)
├── Man (player)
├── ZombieBase (zombies)
├── BuildingBase (buildings)
└── CarScript (vehicles)
```

### Execution Flow

```
Startup → Init() → OnMissionStart() → OnConnect() → Game Loop
```

### Client-Server

```
Client (UI/Input) ←RPC→ Server (Authority/Validation)
```

---

## ⚠️ Common Mistakes

### ❌ Mistake 1: Wrong Directory

**Problem:** Putting code in wrong directory

**Example:**
```c
// ❌ WRONG: UI in 4_World/
scripts/4_World/Menus/MyMenu.c

// ✅ CORRECT: UI in 5_Mission/
scripts/5_Mission/Menus/MyMenu.c
```

### ❌ Mistake 2: Mixing Responsibilities

**Problem:** One class doing multiple jobs

**Example:**
```c
// ❌ WRONG: XP manager creating items
class XPManager
{
    void AddXP() { }
    void CreateItem() { }  // Not XP's job!
}

// ✅ CORRECT: Separate classes
class XPManager { void AddXP() { } }
class ItemFactory { void CreateItem() { } }
```

### ❌ Mistake 3: Client Authority

**Problem:** Client making decisions

**Example:**
```c
// ❌ WRONG: Client gives itself XP
void AddXP() { m_XP += 1000; }

// ✅ CORRECT: Server validates
void RequestXP() { SendRPC(...); }
void Server_AddXP() { if (GetGame().IsServer()) m_XP += 1000; }
```

---

## 🎓 Next Steps

**After learning architecture:**
1. **Read:** [Config Guide](02_config.md) - How to configure mods
2. **Read:** [EnScript Basics](03_enscript_basics.md) - DayZ scripting language
3. **Read:** [Classes & Objects](05_classes_objects.md) - OOP in DayZ
4. **Example:** [examples/xp_system/](../examples/xp_system/) - See architecture in action

---

## 💡 Key Takeaways

1. **3-Layer Architecture:**
   - `3_Game/` = Game logic (managers)
   - `4_World/` = World entities (items, NPCs)
   - `5_Mission/` = Player interaction (UI)

2. **Class Hierarchy:**
   - `EntityAI` is the base class
   - `ItemBase` → All items
   - `Man` → Player

3. **Server Authority:**
   - Server makes ALL decisions
   - Client requests via RPC
   - Server validates

4. **Separation of Concerns:**
   - Each layer has ONE job
   - Each class has ONE responsibility
   - Keep systems INDEPENDENT

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** [Networking Basics](09_networking.md)
