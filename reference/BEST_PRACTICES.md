# Best Practices Guide

**Status:** ✅ Complete Reference
**Target:** All mod developers
**Goal:** Code quality, maintainability, collaboration

---

## ⚡ 30-Second Summary

Best practices for DayZ mod development. Covers code organization, naming conventions, error handling, version control, and mod dependencies.

---

## 📁 Code Organization

### File Structure

DayZ mods follow this standard structure:

```
MyMod/
├── config.cpp                   ← Mod definition
├── mod.cpp                      ← Workshop metadata
├── meta.cpp                     ← Version info
├── logo.paa                     ← Mod icon (256x256)
├── README.md                    ← Documentation
└── scripts/
    ├── 1_Game/                  ← Core game overrides (rarely used)
    ├── 2_Script/                ← Utility scripts
    ├── 3_Game/                  ← Game classes (managers, config)
    ├── 4_World/                 ← World classes (items, buildings)
    ├── 5_Mission/               ← Mission/GUI scripts
    ├── 6_Gui/                   ← GUI layouts
    └── 7_User/                  ← User-defined scripts
```

---

### Directory Usage Guidelines

| Directory | Purpose | Examples |
|-----------|---------|----------|
| **1_Game/** | Core engine overrides (AVOID) | PlayerBase, Man |
| **2_Script/** | Utility functions | MathHelper, StringUtils |
| **3_Game/** | Game logic managers | FactionManager, XPSystem |
| **4_World/** | World entities | Items, Buildings, NPCs |
| **5_Mission/** | Mission scripts | Mission callbacks, GUI |
| **6_Gui/** | GUI layouts | .layout files |
| **7_User/** | Custom scripts | Your custom code |

**Best Practice:** Put most code in `3_Game/`, `4_World/`, `7_User/`

---

### File Naming

#### Class Files

```
✅ GOOD: FactionManager.c
✅ GOOD: playerbase_faction.c (modded class)
❌ BAD: factionmanager.c (lowercase)
❌ BAD: FactionManager_Class.c (redundant)
```

#### Organization

```
scripts/
├── 3_Game/
│   ├── FactionSystem/
│   │   ├── FactionManager.c
│   │   ├── FactionData.c
│   │   └── FactionConstants.c
│   ├── XPSystem/
│   │   ├── SkillManager.c
│   │   └── SimpleSkill.c
│   └── StorageSystem/
│       ├── StorageManager.c
│       └── StoredItem.c
└── 4_World/
    └── Classes/
        ├── FactionPlayer.c
        └── PlayerBase/
            └── playerbase_faction.c
```

---

## 📝 Naming Conventions

### Classes

```
✅ GOOD: class FactionManager
✅ GOOD: class PlayerBaseFaction extends PlayerBase
❌ BAD: class factionManager (lowercase start)
❌ BAD: class Faction_Manager (underscore)
```

**Rule:** PascalCase, descriptive names

---

### Variables

```
✅ GOOD: int playerCount;
✅ GOOD: protected float m_UpdateTimer;  // member prefix
✅ GOOD: static ref Singleton m_Instance;  // static singleton
❌ BAD: int PlayerCount; (variable, not class)
❌ BAD: int updatetimer; (lowercase)
```

**Rules:**
- camelCase for variables
- `m_` prefix for member variables
- `s_` prefix for static variables

---

### Functions

```
✅ GOOD: void UpdatePlayerData()
✅ GOOD: bool CanPerformAction()
❌ BAD: void update_player_data() (snake_case)
❌ BAD: void UpdatePlayerData() returns bool (inconsistent)
```

**Rule:** PascalCase, verb-first for actions

---

### Constants

```
✅ GOOD: const int MAX_PLAYERS = 60;
✅ GOOD: protected const float UPDATE_INTERVAL = 1.0;
❌ BAD: const int max_players = 60; (lowercase)
❌ BAD: const int MAXPLAYERS = 60; (no underscore)
```

**Rule:** UPPER_SNAKE_CASE

---

### Events / Callbacks

```
✅ GOOD: override void OnConnect()
✅ GOOD: void OnPlayerKilled()
✅ GOOD: override void EEKilled(Object killer)
❌ BAD: override void OnPlayerConnect() (non-standard)
```

**Rule:** Use standard DayZ event naming

---

## 🔧 Error Handling

### Always Check Null

```c
✅ GOOD:
void ProcessItem(ItemBase item)
{
    if (!item)
    {
        Print("[Error] Item is null!");
        return;
    }

    item.Process();
}

❌ BAD:
void ProcessItem(ItemBase item)
{
    item.Process(); // Crashes if null!
}
```

---

### Validate Parameters

```c
✅ GOOD:
void AddItemToInventory(PlayerBase player, string itemType, int count)
{
    // Validate player
    if (!player)
    {
        Print("[Error] Player is null!");
        return;
    }

    // Validate item type
    if (itemType == "")
    {
        Print("[Error] Item type is empty!");
        return;
    }

    // Validate count
    if (count <= 0 || count > 100)
    {
        Print("[Error] Invalid count: " + count);
        return;
    }

    // Safe to proceed
    player.GetInventory().CreateInInventory(itemType);
}

❌ BAD:
void AddItemToInventory(PlayerBase player, string itemType, int count)
{
    // No validation - can crash!
    player.GetInventory().CreateInInventory(itemType);
}
```

---

### Graceful Degradation

```c
✅ GOOD:
bool LoadPlayerData(string steam64ID)
{
    string filePath = "$profile:players/" + steam64ID + ".json";

    if (!FileExist(filePath))
    {
        Print("[Warning] File not found: " + filePath);
        return false; // Don't crash, just fail gracefully
    }

    JsonSaveManager json = new JsonSaveManager();

    if (!json.LoadFromFile(filePath, m_PlayerData))
    {
        Print("[Error] Failed to load: " + filePath);
        return false;
    }

    return true;
}

❌ BAD:
void LoadPlayerData(string steam64ID)
{
    string filePath = "$profile:players/" + steam64ID + ".json";

    // Crashes if file doesn't exist!
    JsonSaveManager json = new JsonSaveManager();
    json.LoadFromFile(filePath, m_PlayerData);
}
```

---

### Error Logging

```c
class ErrorLogger
{
    static void LogError(string context, string error)
    {
        string log = "[ERROR] " + context + " - " + error;
        Print(log);

        // Write to error log
        FileHandle file = OpenFile("$profile:errors.log", FileMode.APPEND);
        if (file != 0)
        {
            FPrintln(file, log);
            CloseFile(file);
        }
    }

    static void LogWarning(string context, string warning)
    {
        string log = "[WARNING] " + context + " - " + warning;
        Print(log);
    }
}

// Usage
void ProcessPlayer(PlayerBase player)
{
    if (!player)
    {
        ErrorLogger.LogError("ProcessPlayer", "Player is null");
        return;
    }
}
```

---

## 🎯 Version Control (Git)

### Commit Messages

```
✅ GOOD: "Add faction system with reputation tracking"
✅ GOOD: "Fix: XP save corruption bug"
✅ GOOD: "Update: Improve storage performance"

❌ BAD: "update"
❌ BAD: "fix bug"
❌ BAD: "changes"
```

**Format:** `[Type] Brief description`

Types: `Add`, `Fix`, `Update`, `Remove`, `Refactor`

---

### .gitignore for DayZ Mods

```gitignore
# Build outputs
*.pbo
@*/
*.bidump
*.ebo
*.log
*.rpt

# IDE files
.vs/
.vscode/
.idea/
*.sln
*.suo

# OS files
.DS_Store
Thumbs.db

# Workshop
!mod.cpp
!meta.cpp
```

---

### Branching Strategy

```
main (production)
  ↓
develop (development)
  ↓
feature/faction-system (feature branch)
feature/xp-system
fix/storage-bug
```

**Workflow:**
1. Create branch from `develop`
2. Make changes
3. Test
4. Pull request to `develop`
5. Merge after review
6. Periodically merge `develop` to `main`

---

## 📦 Mod Dependencies

### Declaring Dependencies

**config.cpp:**
```cpp
class CfgPatches
{
    class MyMod
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;

        // Dependencies
        requiredAddons[] =
        {
            "DZ_Data",      // Vanilla
            "DZ_Scripts",   // Vanilla scripts
            "CommunityFramework",  // Custom mod
            "AnotherMod"
        };
    };
};
```

**Best Practice:** List ALL dependencies

---

### Version Constraints

```cpp
class CfgPatches
{
    class MyMod
    {
        // Minimum versions
        requiredAddons[] =
        {
            "DZ_Data",              // Any version
            "CommunityFramework"    // Any version
        };
    };
};
```

**Note:** DayZ doesn't support version constraints yet

---

### Compatibility Checks

```c
class CompatibilityChecker
{
    static void CheckCompatibility()
    {
        // Check for required mods
        if (!IsModLoaded("CommunityFramework"))
        {
            Print("[Compatibility] WARNING: CommunityFramework not loaded!");
        }

        // Check for incompatible mods
        if (IsModLoaded("IncompatibleMod"))
        {
            Print("[Compatibility] ERROR: IncompatibleMod detected!");
            Print("[Compatibility] Please remove IncompatibleMod to use MyMod");
        }
    }

    static bool IsModLoaded(string modName)
    {
        // Check if mod is loaded
        // (Implementation varies)
        return true;
    }
}
```

---

## 🔗 Integration Best Practices

### Using Modded Classes

```c
✅ GOOD:
modded class PlayerBase
{
    protected ref FactionManager m_FactionManager;

    override void OnConnect()
    {
        super.OnConnect(); // Always call super!

        m_FactionManager = new FactionManager();
    }
}

❌ BAD:
modded class PlayerBase
{
    override void OnConnect()
    {
        // Missing super.OnConnect() - breaks other mods!
        m_FactionManager = new FactionManager();
    }
}
```

**Rule:** Always call `super.MethodName()` in overrides

---

### Avoiding Conflicts

```c
✅ GOOD: Use unique prefixes
class MyMod_FactionManager
{
    // Unique prefix prevents conflicts
}

✅ GOOD: Use namespaces
namespace MyMod
{
    class FactionManager
    {
        // Namespace prevents conflicts
    }
}

❌ BAD: Generic names
class Manager  // Conflicts with other mods!
{
}
```

---

### Event Order

When multiple mods override same class:

```
Execution order:
1. Vanilla class
2. Mod A's override
3. Mod B's override
4. Mod C's override
```

**Best Practice:** Document dependencies in README

---

## 📚 Documentation

### File Headers

```c
/**
 * FactionManager.c
 *
 * Purpose: Manage player factions and reputation
 *
 * Features:
 * - 5 factions (Blu, Rossi, Verde, Gialli, Hive)
 * - Reputation tracking
 * - Faction relations
 *
 * Dependencies:
 * - StorageManager
 *
 * Author: YourName
 * Created: 2025-01-18
 * Last updated: 2025-01-18
 */
class FactionManager
{
    // ...
}
```

---

### Function Documentation

```c
/**
 * Add reputation to player for faction
 *
 * @param player - Player to add reputation to
 * @param faction - Faction to add reputation for
 * @param amount - Amount to add (can be negative)
 * @return true if successful, false if failed
 *
 * Example:
 * AddReputation(player, "Blu", 20);  // Add 20 rep
 * AddReputation(player, "Blu", -50); // Remove 50 rep
 */
bool AddReputation(PlayerBase player, string faction, int amount)
{
    // Implementation...
}
```

---

### README Template

```markdown
# My Mod

## Features
- Feature 1
- Feature 2

## Installation
1. Download mod
2. Extract to `@MyMod/`
3. Add to server launch parameters
4. Connect

## Dependencies
- DayZ (vanilla)
- Community Framework (optional)

## Configuration
Edit `config.cpp` to customize

## Troubleshooting
### Issue: Mod doesn't load
**Solution:** Check dependencies

## Changelog
### 1.0.0 (2025-01-18)
- Initial release

## Credits
- Author: YourName
- Contributors: ...

## License
MIT License
```

---

## 🎓 Code Style Guide

### Indentation

```
✅ GOOD: Tabs (DayZ standard)
class MyClass
{
	void MyMethod()
	{
		if (condition)
		{
			DoSomething();
		}
	}
}

❌ BAD: Spaces (inconsistent)
class MyClass
{
    void MyMethod()
    {
        if (condition)
        {
            DoSomething();
        }
    }
}
```

**Standard:** Tabs (DayZ convention)

---

### Braces

```
✅ GOOD: Allman style (DayZ standard)
class MyClass
{
	void MyMethod()
	{
		if (condition)
		{
			DoSomething();
		}
	}
}

❌ BAD: K&R style (inconsistent)
class MyClass {
	void MyMethod() {
		if (condition) {
			DoSomething();
		}
	}
}
```

**Standard:** Allman (braces on new line)

---

### Spacing

```
✅ GOOD:
void MyMethod(int param1, string param2)
{
	int value = param1 + 5;

	if (value > 10)
	{
		DoSomething();
	}
}

❌ BAD:
void MyMethod(int param1,string param2)// No spacing
{
	int value=param1+5;// No spacing

	if(value>10)// No spacing
	{
		DoSomething();
	}
}
```

**Standard:** Space after commas, around operators

---

## 🔍 Code Review Checklist

### Functionality
- [ ] Code works as intended
- [ ] No crashes or errors
- [ ] Edge cases handled
- [ ] Performance acceptable

### Security
- [ ] Input validation
- [ ] Server-side authority
- [ ] No exploitable bugs
- [ ] Data integrity checks

### Quality
- [ ] Naming conventions followed
- [ ] Code is readable
- [ ] Comments added where needed
- [ ] No hardcoded values

### Documentation
- [ ] README updated
- [ ] Function documentation added
- [ ] Examples provided
- [ ] Breaking changes noted

---

## 🚀 Advanced Patterns

### Singleton Pattern

```c
class MyManager
{
    protected static ref MyManager m_Instance;

    static MyManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new MyManager();
        }

        return m_Instance;
    }

    void MyManager()
    {
        // Initialize
    }
}
```

---

### Factory Pattern

```c
class ItemFactory
{
    static EntityAI CreateItem(string type, vector pos)
    {
        switch (type)
        {
            case "weapon":
                return CreateWeapon(pos);

            case "food":
                return CreateFood(pos);

            default:
                return null;
        }
    }
}
```

---

### Observer Pattern

```c
class EventManager
{
    protected ref array<ref EventHandler> m_Handlers;

    void RegisterEvent(EventHandler handler)
    {
        m_Handlers.Insert(handler);
    }

    void TriggerEvent(string eventType)
    {
        foreach (EventHandler handler : m_Handlers)
        {
            handler.OnEvent(eventType);
        }
    }
}
```

---

## 🔗 Related Topics

→ [Config](../topics/02_config.md) - Mod configuration
→ [Performance](../reference/PERFORMANCE_GUIDE.md) - Optimization
→ [Security](../reference/SECURITY_GUIDE.md) - Anti-cheat
→ [Deployment](../topics/26_deployment.md) - Publishing

---

*Last updated: 2025-01-18*
*Target: All mod developers*
