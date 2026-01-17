# API SIGNATURES - For AI

**Purpose:** All API signatures in one place
**For:** AI to know exact method signatures without reading files
**Last Updated:** 2025-01-17

---

## 🎯 XP SYSTEM (examples/xp_system/)

### SimpleSkill

#### void AddXP(int amount)
**File:** examples/xp_system/scripts/4_World/Classes/SimpleSkill.c:15
**Purpose:** Add experience to skill
**Parameters:**
- amount (int): XP to add
**Example:**
```c
skill.AddXP(10); // Add 10 XP
```
**Note:** Automatically calls CheckLevelUp()

---

#### void CheckLevelUp()
**File:** examples/xp_system/scripts/4_World/Classes/SimpleSkill.c:25
**Purpose:** Automatic level up check
**Called by:** AddXP()
**Logic:**
```c
int requiredXP = Level * 1000; // 1000 XP per level
if (CurrentXP >= requiredXP) {
    CurrentXP -= requiredXP;
    Level++;
    OnLevelUp();
}
```
**Note:** Required XP = Level × 1000

---

#### void OnLevelUp()
**File:** examples/xp_system/scripts/4_World/Classes/SimpleSkill.c:35
**Purpose:** Called when level increases
**Override:** Yes
**Example:**
```c
override void OnLevelUp() {
    Print("Level " + Level + " reached!");
}
```

---

#### float GetLevelBonus()
**File:** examples/xp_system/scripts/4_World/Classes/SimpleSkill.c:50
**Returns:** float - bonus multiplier (1.0 + Level × 0.1)
**Example:**
```c
float bonus = skill.GetLevelBonus(); // Level 5 = 1.5x
```

---

### SkillManager

#### void SkillManager(PlayerBase player)
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:10
**Purpose:** Constructor
**Parameters:**
- player (PlayerBase): Player instance
**Initializes:**
- Survival skill
- Crafting skill
- Combat skill

---

#### void AddXP(string skillName, int amount)
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:35
**Purpose:** Add XP to specific skill
**Parameters:**
- skillName (string): "Survival", "Crafting", "Combat"
- amount (int): XP amount
**Example:**
```c
m_SkillManager.AddXP("Combat", 10); // 10 XP for combat
```

---

#### void SaveSkills()
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:45
**Purpose:** Save skills to JSON
**File path:** `$profile:skills_{steam64}.json`
**Format:** JSON
**Example:**
```json
{
    "Survival": {"Name": "Survival", "CurrentXP": 500, "Level": 1},
    "Combat": {"Name": "Combat", "CurrentXP": 1500, "Level": 2}
}
```

---

#### void LoadSkills()
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:60
**Purpose:** Load skills from JSON
**File path:** `$profile:skills_{steam64}.json`
**Note:** Creates new skills if file doesn't exist

---

#### SimpleSkill GetSkill(string skillName)
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:80
**Returns:** SimpleSkill or null
**Parameters:**
- skillName (string): Skill name

---

#### int GetLevel(string skillName)
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:90
**Returns:** int - skill level (1+)
**Note:** Returns 1 if skill not found

---

#### void OnZombieKilled()
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:100
**Purpose:** Called when player kills zombie
**XP:** 10 XP for Combat skill
**Call from:** PlayerBase.EEKilled()

---

#### void OnBookRead()
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:110
**Purpose:** Called when player reads book
**XP:** 1500 XP for Survival skill
**Call from:** PlayerBase.ReadItem()

---

#### void OnItemCrafted()
**File:** examples/xp_system/scripts/4_World/Classes/SkillManager.c:120
**Purpose:** Called when player crafts item
**XP:** 500 XP for Crafting skill
**Call from:** PlayerBase.CraftItem()

---

### PlayerBase (modded)

#### override void OnConnect()
**File:** examples/xp_system/scripts/4_World/Classes/PlayerBase/playerbase_xp.c:10
**Purpose:** Initialize XP system
**Calls:**
```c
m_SkillManager = new SkillManager(this);
m_SkillManager.LoadSkills();
```

---

#### override void OnDisconnect()
**File:** examples/xp_system/scripts/4_World/Classes/PlayerBase/playerbase_xp.c:20
**Purpose:** Save XP system
**Calls:**
```c
m_SkillManager.SaveSkills();
```

---

#### override void EEKilled(Object killer)
**File:** examples/xp_system/scripts/4_World/Classes/PlayerBase/playerbase_xp.c:30
**Purpose:** Give XP to killer
**Logic:**
```c
if (killer.IsMan() && killer.IsPlayer()) {
    killer.m_SkillManager.OnZombieKilled();
}
```

---

#### SkillManager GetSkillManager()
**File:** examples/xp_system/scripts/4_World/Classes/PlayerBase/playerbase_xp.c:50
**Returns:** SkillManager instance

---

## 🚀 TELEPORTATION (examples/teleportation/)

### TeleportNode

#### Properties:
- string Name - Location name
- vector Position - Position (x, y, z)
- string Description - Location description

---

### TeleportManager

#### void TeleportManager(PlayerBase player)
**File:** examples/teleportation/scripts/4_World/Classes/TeleportManager.c:10
**Purpose:** Constructor
**Calls:** LoadNodes()

---

#### void LoadNodes()
**File:** examples/teleportation/scripts/4_World/Classes/TeleportManager.c:20
**Purpose:** Load teleport nodes from JSON
**File path:** `$profile:teleports.json`
**Format:**
```json
[
    {"Name": "Cherno", "Position": [6500, 0, 2500], "Description": "Chernogorsk"}
]
```

---

#### array<TeleportNode> GetNodes()
**File:** examples/teleportation/scripts/4_World/Classes/TeleportManager.c:40
**Returns:** Array of all teleport nodes

---

#### TeleportNode FindNode(string name)
**File:** examples/teleportation/scripts/4_World/Classes/TeleportManager.c:50
**Returns:** TeleportNode or null
**Parameters:**
- name (string): Exact location name (case-sensitive)

---

#### bool CanTeleport(PlayerBase player)
**File:** examples/teleportation/scripts/4_World/Classes/TeleportManager.c:60
**Returns:** true if player can teleport
**Checks:**
- Not in vehicle (GetCommand_Vehicle())
- Not unconscious (IsUnconscious())
- Not restrained (IsRestrained())

---

#### void TeleportPlayer(string location)
**File:** examples/teleportation/scripts/4_World/Classes/TeleportManager.c:80
**Purpose:** Teleport player to location
**Parameters:**
- location (string): Location name
**Calls:** CanTeleport(), FindNode(), player.SetPosition()

---

### PlayerBase (modded)

#### TeleportManager GetTeleportManager()
**File:** examples/teleportation/scripts/4_World/Classes/PlayerBase/playerbase_teleport.c:20
**Returns:** TeleportManager instance

---

#### override bool OnChat(PlayerBase player, string message)
**File:** examples/teleportation/scripts/4_World/Classes/PlayerBase/playerbase_teleport.c:30
**Purpose:** Handle chat commands
**Command:** `/tp <location>`
**Returns:** true if command handled

---

## 📦 STORAGE (examples/storage/)

### StoredItem

#### Properties:
- string ClassName - Item type
- float Health - Item health
- float Quantity - Item quantity
- array<StoredItem> Attachments - Nested attachments
- array<StoredItem> Cargo - Nested cargo items

---

### StorageManager

#### void StorageManager(string steam64)
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:10
**Purpose:** Constructor
**Parameters:**
- steam64 (string): Player Steam64 ID

---

#### void Save()
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:20
**Purpose:** Save storage to JSON
**File path:** `$profile:storage_{steam64}.json`

---

#### void Load()
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:30
**Purpose:** Load storage from JSON
**Note:** Creates new storage if file doesn't exist

---

#### StoredItem StoreItemRecursive(ItemBase item)
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:50
**Purpose:** Recursively store item with attachments and cargo
**Returns:** StoredItem representation
**Note:** Handles weapons with magazines and ammo

---

#### ItemBase RestoreItem(StoredItem stored, vector position)
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:100
**Purpose:** Recursively restore item from storage
**Returns:** ItemBase instance
**Note:** Restores attachments and cargo

---

#### void StoreItem(ItemBase item)
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:150
**Purpose:** Store item and delete from world
**Calls:** StoreItemRecursive(), GetGame().ObjectDelete()

---

#### void StoreInventory(EntityAI source)
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:165
**Purpose:** Store all items from container
**Note:** Iterates cargo and stores each item

---

#### void RestoreInventory(EntityAI target, vector position)
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:180
**Purpose:** Restore all items to container
**Note:** Clears storage after restoration

---

#### int GetItemCount()
**File:** examples/storage/scripts/4_World/Classes/StorageManager.c:200
**Returns:** Number of stored items

---

## 🧟 CUSTOM NPC (examples/custom_zombie/)

### AIBanditConfig

#### Properties:
- float Health - NPC health
- float Damage - NPC damage
- float AgroRange - Agro range in meters
- array<string> Loot - Loot table

---

### CustomBandit

#### override void EEInit()
**File:** examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:10
**Purpose:** Initialize NPC
**Calls:** LoadConfig(), SetHealth()

---

#### AIBanditConfig LoadConfig()
**File:** examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:30
**Returns:** Configuration from JSON
**File path:** `$profile:bandit_config.json`
**Fallback:** Default config if file doesn't exist

---

#### override void EEKilled(Object killer)
**File:** examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:50
**Purpose:** Spawn loot on death
**Calls:** SpawnLoot()

---

#### void SpawnLoot()
**File:** examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:60
**Purpose:** Spawn random loot from table
**Chance:** 50% per item
**Note:** Creates items at NPC position

---

#### override void OnUpdate(float delta_time)
**File:** examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:80
**Purpose:** Custom AI behavior
**Calls:** FindNearestPlayer(), AddTarget()

---

#### PlayerBase FindNearestPlayer()
**File:** examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:100
**Returns:** Nearest player within agro range
**Note:** Returns null if no players nearby

---

## 📦 AUTO STACK (examples/auto_stack/)

### PlayerBase (modded)

#### override void OnInventoryEnter(EntityAI item)
**File:** examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:10
**Purpose:** Trigger auto-stack when item enters inventory
**Checks:** Cooldown (5 seconds)
**Calls:** AutoStackItems()

---

#### void AutoStackItems()
**File:** examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:30
**Purpose:** Automatically combine stackable items
**Logic:**
1. Enumerate all inventory items
2. Find matching types
3. Combine quantities
4. Delete empty items

---

#### bool ShouldStack(ItemBase item)
**File:** examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:70
**Returns:** true if item can be stacked
**Checks:**
- Not Magazine
- Not Weapon
- Not Container
- Has quantity
- Not full

---

#### bool CanStack(ItemBase item1, ItemBase item2)
**File:** examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:95
**Returns:** true if items can be combined
**Checks:**
- Same type
- Same health level
- Same wetness

---

#### bool StackItems(ItemBase target, ItemBase source)
**File:** examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:145
**Returns:** true if items combined
**Logic:**
1. Calculate available space
2. Transfer quantity
3. Delete source if empty

---

#### void ForceAutoStack()
**File:** examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:180
**Purpose:** Force auto-stack immediately
**Note:** Resets cooldown

---

## 🎮 VANILLA API (Common)

### PlayerBase

#### string GetIdentity().GetId()
**Returns:** Player Steam64 ID
**Use for:** Player-specific data, storage

---

#### Man CastToMan()
**Purpose:** Cast EntityAI to Man
**Returns:** Man or null

---

#### PlayerBase Cast(PlayerBase)
**Purpose:** Cast Man to PlayerBase
**Returns:** PlayerBase or null

---

### EntityAI

#### float GetHealth()
**Returns:** Current health

---

#### float GetMaxHealth()
**Returns:** Maximum health

---

#### void SetHealth(float health)
**Purpose:** Set health value

---

#### float GetQuantity()
**Returns:** Current quantity (for stackable items)

---

#### float GetQuantityMax()
**Returns:** Maximum quantity

---

#### void AddQuantity(float amount)
**Purpose:** Add to quantity (can be negative)

---

#### void SetQuantity(float quantity)
**Purpose:** Set quantity value

---

#### string GetType()
**Returns:** Class name

---

### GetGame()

#### void GetPlayers(array<Man> players)
**Purpose:** Get all players on server
**Fills:** Array with all PlayerBase instances

---

#### EntityAI CreateObject(string className, vector position)
**Returns:** Created entity or null
**Purpose:** Spawn entity

---

#### EntityAI CreateObjectEx(string className, vector position, int flags)
**Returns:** Created entity or null
**Flags:** ECE_PLACE_ON_SURFACE, ECE_INVENTORY, etc.

---

#### void ObjectDelete(Object obj)
**Purpose:** Delete entity from world

---

#### bool FileExist(string path)
**Returns:** true if file exists
**Note:** Use $profile: prefix

---

#### float GetTickTime()
**Returns:** Server time in seconds

---

### Inventory

#### void EnumerateInventory(InventoryTraversalType type, array<EntityAI> items)
**Purpose:** Get all items in inventory
**Types:** PREORDER, POSTORDER, DEPTH_FIRST

---

#### int AttachmentCount()
**Returns:** Number of attachments

---

#### EntityAI GetAttachmentFromIndex(int index)
**Returns:** Attachment at index

---

#### CargoBase GetCargo()
**Returns:** Cargo container or null

---

## 📝 JSON API

### JsonFileLoader<T>

#### static void JsonSaveFile(string path, T data)
**Purpose:** Save object to JSON file
**Example:**
```c
JsonFileLoader<SkillData>::JsonSaveFile("$profile:skills.json", data);
```

---

#### static void JsonLoadFile(string path, out T data)
**Purpose:** Load object from JSON file
**Example:**
```c
SkillData data;
JsonFileLoader<SkillData>::JsonLoadFile("$profile:skills.json", data);
```

---

*Last Updated: 2025-01-17*
*Purpose: AI to know exact signatures without reading files*
