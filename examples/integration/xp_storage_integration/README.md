# XP + Storage Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** xp_system + storage

---

## ⚡ 30-Second Summary

Integration example showing how to persist XP/skills across servers using storage system. Players keep their levels when switching servers.

---

## 🎯 Features

### Integration Mechanics
- ✅ **XP Persistence** - Skills save cross-server
- ✅ **Auto-Sync** - Load on connect, save on disconnect
- ✅ **JSON Storage** - Steam64 ID-based
- ✅ **Multiple Skills** - Combat, Crafting, Survival
- ✅ **Level Bonuses** - Restored after server switch

---

## 📁 File Structure

```
xp_storage_integration/
├── README.md                    ← This file
├── config.cpp                   ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── PersistentSkill.c      ← Skill with storage
            ├── PersistentSkillManager.c ← Manager with save/load
            └── PlayerBase/
                └── playerbase_xp_storage.c ← Integration
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- `xp_system/` - Tracks XP, levels up skills
- `storage/` - Saves data to JSON

**Integration:**
1. Player connects → Load XP from JSON
2. Player gains XP → Update manager
3. Player disconnects → Save XP to JSON
4. Player joins different server → XP loaded from same JSON

### Data Flow

```
Server A                          Server B
   |                                 |
Player connects                    Player connects
   |                                 |
Load XP from JSON                 Load XP from JSON  ← Same file!
   |                                 |
Player gains XP                   Player gains XP
   |                                 |
Save XP to JSON                   Save XP to JSON
   ↓                                 ↓
$profile:XPStorage/           $profile:XPStorage/
  player_steamid.json              player_steamid.json
```

---

## 💡 Implementation

### 1. PersistentSkill.c

XP Skill with storage capability:

```c
class PersistentSkill
{
    string SkillName;
    int CurrentXP;
    int Level;
    int MaxLevel;

    // Serialize to JSON
    string ToJSON()
    {
        return string.Format("{\"xp\":%1,\"level\":%2}", CurrentXP, Level);
    }

    // Load from JSON
    static PersistentSkill FromJSON(string json)
    {
        // Parse JSON and return skill
        // Implementation details in actual file
    }
}
```

### 2. PersistentSkillManager.c

Manager with save/load:

```c
class PersistentSkillManager
{
    ref map<string, ref PersistentSkill> m_Skills;

    // Save to storage system
    void SaveToStorage(PlayerBase player)
    {
        string steamId = player.GetIdentity().GetId();
        string json = SerializeSkills();

        // Use storage system
        StorageManager.Save(steamId, json);
    }

    // Load from storage system
    void LoadFromStorage(PlayerBase player)
    {
        string steamId = player.GetIdentity().GetId();

        // Use storage system
        string json = StorageManager.Load(steamId);

        if (json != "")
        {
            DeserializeSkills(json);
        }
    }
}
```

### 3. PlayerBase Integration

```c
modded class PlayerBase
{
    ref PersistentSkillManager m_XPManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Create manager
        m_XPManager = new PersistentSkillManager();

        // Load XP from storage
        m_XPManager.LoadFromStorage(this);

        Print("[XP+Storage] Loaded XP for player: " + GetIdentity().GetName());
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save XP to storage
        if (m_XPManager)
        {
            m_XPManager.SaveToStorage(this);
        }

        Print("[XP+Storage] Saved XP for player: " + GetIdentity().GetName());
    }

    // Add XP with auto-save
    void AddPersistentXP(string skillName, int amount)
    {
        if (m_XPManager)
        {
            m_XPManager.AddXP(skillName, amount);

            // Auto-save every level up
            if (m_XPManager.JustLeveledUp())
            {
                m_XPManager.SaveToStorage(this);
            }
        }
    }
}
```

---

## 📝 Usage Examples

### Example 1: Zombie Kill Grants XP

```c
modded class PlayerBase
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        PlayerBase killerPlayer = PlayerBase.Cast(killer);

        if (killerPlayer && killerPlayer == this)
        {
            // Give XP that persists across servers
            AddPersistentXP("Combat", 10);
        }
    }
}
```

### Example 2: Crafting Grants XP

```c
modded class CraftingManager
{
    override void CraftItem(string recipe)
    {
        super.CraftItem(recipe);

        // Give crafting XP
        PlayerBase player = GetPlayer();
        player.AddPersistentXP("Crafting", 50);
    }
}
```

### Example 3: Reading Book Grants XP

```c
class ReadableBook extends ItemBook
{
    override void Read(PlayerBase player)
    {
        super.Read(player);

        // Give XP that persists
        player.AddPersistentXP("Survival", 1500);
    }
}
```

---

## 🔗 Related Examples

→ [XP System](../xp_system/) - Individual XP mechanics
→ [Storage System](../storage/) - Individual storage mechanics
→ [Integration: Faction+Teleport](../faction_teleport_integration/) - Another integration

---

## ⚠️ Important Notes

### Storage Location
XP data stored in: `$profile:XPStorage/`

**File naming:** `{steam64_id}.json`

### Cross-Server Requirements
- Both servers must have this mod
- Same storage path (`$profile:XPStorage/`)
- Same mod version

### Auto-Save Triggers
- Player disconnects (always)
- Level up (configurable)
- Every 5 minutes (configurable)

---

*Last updated: 2025-01-18*
*Combines: xp_system + storage*
