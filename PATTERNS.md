# CODE PATTERNS - For AI

**Purpose:** All common DayZ modding patterns in one place
**For:** AI to apply patterns without searching
**Last Updated:** 2025-01-17

---

## 🎯 PATTERN 1: Modded Class with Manager

**Use:** Add custom system to PlayerBase (XP, storage, etc.)

**Structure:**
```c
modded class PlayerBase
{
    protected CustomManager m_Manager;

    override void OnConnect()
    {
        super.OnConnect(); // ALWAYS FIRST!
        m_Manager = new CustomManager(this);
        m_Manager.Load();
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();
        if (m_Manager) {
            m_Manager.Save();
        }
    }

    CustomManager GetCustomManager()
    {
        return m_Manager;
    }
}
```

**Examples:**
- XP System: examples/xp_system/scripts/4_World/Classes/PlayerBase/playerbase_xp.c
- Teleportation: examples/teleportation/scripts/4_World/Classes/PlayerBase/playerbase_teleport.c

**Key Points:**
1. Use `modded class` keyword
2. Call `super.OnConnect()` FIRST
3. Create manager in OnConnect()
4. Save in OnDisconnect()
5. Provide getter for manager

---

## 📋 PATTERN 2: JSON Persistence

**Use:** Save/Load complex data to JSON files

**Data Class:**
```c
class CustomData
{
    string Field1;
    int Field2;
    float Field3;
    ref array<ref SubData> ArrayField;
}

class SubData
{
    string Name;
    int Value;
}
```

**Manager Class:**
```c
class CustomManager
{
    protected string m_Steam64;
    protected CustomData m_Data;

    void CustomManager(PlayerBase player)
    {
        m_Steam64 = player.GetIdentity().GetId();
    }

    void Save()
    {
        string filePath = "$profile:custom_" + m_Steam64 + ".json";
        JsonFileLoader<CustomData>::JsonSaveFile(filePath, m_Data);
    }

    void Load()
    {
        string filePath = "$profile:custom_" + m_Steam64 + ".json";
        if (FileExist(filePath)) {
            m_Data = new CustomData;
            JsonFileLoader<CustomData>::JsonLoadFile(filePath, m_Data);
        } else {
            // Create default data
            m_Data = new CustomData;
            m_Data.Field1 = "default";
        }
    }
}
```

**Examples:**
- XP: examples/xp_system/scripts/4_World/Classes/SkillManager.c:45
- Storage: examples/storage/scripts/4_World/Classes/StorageManager.c:20

**Key Points:**
1. Use `$profile:` prefix for file paths
2. Include Steam64 ID in filename for per-player data
3. Always check `FileExist()` before loading
4. Create default data if file doesn't exist
5. Use `ref array<ref SubType>` for nested arrays

---

## 🔔 PATTERN 3: Event Handler Override

**Use:** React to game events (kill, craft, read, etc.)

**Kill Event:**
```c
modded class PlayerBase
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(); // ALWAYS FIRST!

        // Check if killer is player
        if (killer && killer.IsMan()) {
            Man man = killer.CastToMan();
            if (man && man.IsPlayer()) {
                PlayerBase player = PlayerBase.Cast(man);
                // Give reward to killer
                if (player.m_Manager) {
                    player.m_Manager.OnPlayerKilled(this);
                }
            }
        }
    }
}
```

**Damage Event:**
```c
override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
{
    super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos);

    // Custom damage handling
    if (source && source.IsMan()) {
        // Attacked by player
    }
}
```

**Inventory Enter Event:**
```c
override void OnInventoryEnter(EntityAI item)
{
    super.OnInventoryEnter(item);

    // Item entered inventory
    if (item && item.IsInherited(ItemBase)) {
        ItemBase ib = ItemBase.Cast(item);
        // Process item
    }
}
```

**Examples:**
- Kill XP: examples/xp_system/scripts/4_World/Classes/PlayerBase/playerbase_xp.c:30
- Auto Stack: examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:10
- Loot Drop: examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:50

**Key Points:**
1. ALWAYS call `super.EVENT()` first
2. Check null before using objects
3. Cast types: `Object → Man → PlayerBase`
4. Use `IsInherited()` to check type

---

## 🔁 PATTERN 4: Recursive Item Processing

**Use:** Process items with attachments and cargo (weapons, containers)

**Store Recursive:**
```c
StoredItem StoreItemRecursive(ItemBase item)
{
    if (!item) return null;

    StoredItem stored = new StoredItem;
    stored.ClassName = item.GetType();
    stored.Health = item.GetHealth();
    stored.Quantity = item.GetQuantity();

    // Store attachments
    for (int i = 0; i < item.GetInventory().AttachmentCount(); i++) {
        ItemBase attachment = ItemBase.Cast(item.GetInventory().GetAttachmentFromIndex(i));
        if (attachment) {
            StoredItem storedAttachment = StoreItemRecursive(attachment);
            if (storedAttachment) {
                stored.Attachments.Insert(storedAttachment);
            }
        }
    }

    // Store cargo
    CargoBase cargo = item.GetInventory().GetCargo();
    if (cargo) {
        for (int j = 0; j < cargo.GetItemCount(); j++) {
            ItemBase cargoItem = cargo.GetItem(j);
            if (cargoItem) {
                StoredItem storedCargo = StoreItemRecursive(cargoItem);
                if (storedCargo) {
                    stored.Cargo.Insert(storedCargo);
                }
            }
        }
    }

    return stored;
}
```

**Restore Recursive:**
```c
ItemBase RestoreItem(StoredItem stored, vector position)
{
    if (!stored) return null;

    // Create item
    ItemBase item = ItemBase.Cast(GetGame().CreateObjectEx(stored.ClassName, position, ECE_PLACE_ON_SURFACE));
    if (!item) return null;

    // Restore properties
    item.SetHealth(stored.Health);
    item.SetQuantity(stored.Quantity);

    // Restore attachments
    foreach (StoredItem storedAttachment : stored.Attachments) {
        ItemBase attachment = RestoreItem(storedAttachment, position);
        if (attachment) {
            item.GetInventory().AttachmentEntityAttachment(attachment);
        }
    }

    // Restore cargo
    foreach (StoredItem storedCargo : stored.Cargo) {
        ItemBase cargoItem = RestoreItem(storedCargo, position);
        if (cargoItem) {
            item.GetInventory().GetCargo().AddEntityToCargo(cargoItem);
        }
    }

    return item;
}
```

**Examples:**
- Storage: examples/storage/scripts/4_World/Classes/StorageManager.c:50

**Key Points:**
1. Check null at start
2. Use `CreateObjectEx` with `ECE_PLACE_ON_SURFACE`
3. Process attachments BEFORE cargo
4. Handle failures gracefully

---

## 📡 PATTERN 5: Chat Command Handler

**Use:** Handle custom chat commands (/tp, /help, etc.)

```c
modded class PlayerBase
{
    protected ref CommandManager m_CommandManager;

    override void OnConnect()
    {
        super.OnConnect();
        m_CommandManager = new CommandManager(this);
    }

    override bool OnChat(PlayerBase player, string message)
    {
        // Let base class handle first
        if (super.OnChat(player, message))
            return true;

        // Check for custom commands
        if (message.StartsWith("/tp ")) {
            string location = message.Substring(4); // After "/tp "
            m_CommandManager.Teleport(location.Trim());
            return true; // Handled
        }

        return false; // Not handled
    }
}

class CommandManager
{
    protected PlayerBase m_Player;

    void CommandManager(PlayerBase player)
    {
        m_Player = player;
    }

    void Teleport(string location)
    {
        // Teleport logic
    }
}
```

**Examples:**
- Teleportation: examples/teleportation/scripts/4_World/Classes/PlayerBase/playerbase_teleport.c:30

**Key Points:**
1. Call `super.OnChat()` first
2. Return `true` if you handled the command
3. Return `false` if not handled
4. Use `Substring()` to extract arguments
5. Use `Trim()` to remove whitespace

---

## 🎲 PATTERN 6: Weighted Random Selection

**Use:** Select item from loot table with weights

**Simple Version:**
```c
string GetRandomLoot()
{
    array<string> loot = {"Item1", "Item2", "Item3"};
    return loot.GetRandomElement();
}
```

**Weighted Version:**
```c
class LootEntry
{
    string ItemName;
    float Weight;
}

string GetWeightedLoot(array<ref LootEntry> lootTable)
{
    float totalWeight = 0;
    foreach (LootEntry entry : lootTable) {
        totalWeight += entry.Weight;
    }

    float random = Math.RandomFloat01() * totalWeight;
    float current = 0;

    foreach (LootEntry entry : lootTable) {
        current += entry.Weight;
        if (random <= current) {
            return entry.ItemName;
        }
    }

    return lootTable[0].ItemName; // Fallback
}
```

**Tier Version:**
```c
string GetTieredLoot()
{
    float roll = Math.RandomFloat01();

    if (roll < 0.01) // 1%
        return GetLegendaryLoot();
    else if (roll < 0.10) // 9%
        return GetEpicLoot();
    else if (roll < 0.30) // 20%
        return GetRareLoot();
    else // 70%
        return GetCommonLoot();
}
```

**Examples:**
- Loot Boxes: reference/LOOT_BOX_SYSTEMS.md
- NPC Loot: examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:60

**Key Points:**
1. Weights are relative, not percentages
2. Use `Math.RandomFloat01()` for 0-1 range
3. Always have fallback

---

## 🔍 PATTERN 7: Find Nearest Player

**Use:** Find closest player to entity

```c
PlayerBase FindNearestPlayer(EntityAI source, float maxDistance = 100.0)
{
    array<PlayerBase> players = new array<PlayerBase>;
    GetGame().GetPlayers(players);

    PlayerBase nearest = null;
    float minDistance = maxDistance;

    foreach (PlayerBase player : players) {
        float distance = vector.Distance(source.GetPosition(), player.GetPosition());
        if (distance < minDistance) {
            minDistance = distance;
            nearest = player;
        }
    }

    return nearest;
}
```

**Examples:**
- NPC Agro: examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c:100

**Key Points:**
1. Use `GetGame().GetPlayers()` to get all players
2. Use `vector.Distance()` for distance
3. Return null if no players in range

---

## ⏱️ PATTERN 8: Cooldown System

**Use:** Prevent spam of actions

```c
modded class PlayerBase
{
    protected float m_LastActionTime;
    protected const float ACTION_COOLDOWN = 5.0; // 5 seconds

    bool CanPerformAction()
    {
        float currentTime = GetGame().GetTickTime();
        return (currentTime - m_LastActionTime) >= ACTION_COOLDOWN;
    }

    void PerformAction()
    {
        if (!CanPerformAction()) {
            NotificationSystem.Notify(this, "Action on cooldown!");
            return;
        }

        // Do action
        m_LastActionTime = GetGame().GetTickTime();
    }
}
```

**Examples:**
- Auto Stack: examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:10

**Key Points:**
1. Use `GetGame().GetTickTime()` for current time
2. Store last action time
3. Check cooldown before action
4. Update time after action

---

## 🔢 PATTERN 9: Level System

**Use:** Calculate level from experience

```c
class SimpleSkill
{
    int CurrentXP;
    int Level;

    void AddXP(int amount)
    {
        CurrentXP += amount;
        CheckLevelUp();
    }

    void CheckLevelUp()
    {
        int requiredXP = Level * 1000; // Linear: 1K, 2K, 3K...

        // Alternative: Exponential
        // int requiredXP = Level * Level * 1000; // 1K, 4K, 9K...

        if (CurrentXP >= requiredXP) {
            CurrentXP -= requiredXP;
            Level++;
            OnLevelUp();
        }
    }

    void OnLevelUp()
    {
        // Give bonus, notify, etc.
    }
}
```

**Examples:**
- XP System: examples/xp_system/scripts/4_World/Classes/SimpleSkill.c:15

**Key Points:**
1. Subtract required XP from current (don't reset)
2. Can chain multiple level-ups
3. Formula can be linear or exponential

---

## 📦 PATTERN 10: Enumerate Inventory

**Use:** Process all items in inventory

**Basic:**
```c
void ProcessInventory(EntityAI container)
{
    array<EntityAI> items = new array<EntityAI>;
    container.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

    foreach (EntityAI entity : items) {
        ItemBase item = ItemBase.Cast(entity);
        if (item) {
            // Process item
        }
    }
}
```

**With Filtering:**
```c
void ProcessFood(EntityAI container)
{
    array<EntityAI> items = new array<EntityAI>;
    container.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

    foreach (EntityAI entity : items) {
        ItemBase item = ItemBase.Cast(entity);
        if (item && item.IsInherited(Edible_Base)) {
            // Process food item
        }
    }
}
```

**Examples:**
- Auto Stack: examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:30

**Key Points:**
1. Use `InventoryTraversalType.PREORDER` (top-down)
2. Always cast to `ItemBase`
3. Use `IsInherited()` to check type
4. Check null before using

---

## 🎮 PATTERN 11: Custom NPC (Zombie-based)

**Use:** Create NPC using zombie as base

```c
class CustomBandit extends InfectedBanditBase
{
    protected ref CustomConfig m_Config;

    override void EEInit()
    {
        super.EEInit();
        m_Config = LoadConfig();
        SetHealth(m_Config.Health);
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        SpawnLoot();
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Custom AI
        PlayerBase nearest = FindNearestPlayer();
        if (nearest) {
            AddTarget(nearest);
        }
    }
}
```

**Examples:**
- Custom Zombie: examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c

**Key Points:**
1. Extend `InfectedBanditBase` (NOT AnimalBase!)
2. Load config in `EEInit()`
3. Override `EEKilled()` for loot
4. Override `OnUpdate()` for custom AI
5. Always call `super` methods

---

## 🔔 PATTERN 12: Notification System

**Use:** Send message to player

```c
class NotificationSystem
{
    static void Notify(PlayerBase player, string message)
    {
        if (!player) return;

        Param1<string> msg = new Param1<string>(message);
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, player.GetIdentity());
    }
}

// Usage:
NotificationSystem.Notify(player, "You gained 10 XP!");
```

**Examples:**
- XP System: examples/xp_system/scripts/4_World/Classes/SkillManager.c
- Teleportation: examples/teleportation/scripts/4_World/Classes/TeleportManager.c

**Key Points:**
1. Check null before sending
2. Use `Param1<string>` for message
3. Use `ERPCs.RPC_USER_ACTION_MESSAGE` for chat
4. Pass `player.GetIdentity()` for sender

---

## ⚠️ PATTERN 13: Safety Checks

**Use:** Validate conditions before actions

```c
bool IsSafeToTeleport(PlayerBase player)
{
    // Not in vehicle
    if (player.GetCommand_Vehicle())
        return false;

    // Not unconscious
    if (player.IsUnconscious())
        return false;

    // Not restrained
    if (player.IsRestrained())
        return false;

    // Not in combat (optional)
    float lastDamage = player.GetLastDamageTime();
    float currentTime = GetGame().GetTickTime();
    if ((currentTime - lastDamage) < 300) // 5 minutes
        return false;

    return true;
}
```

**Examples:**
- Teleportation: examples/teleportation/scripts/4_World/Classes/TeleportManager.c:60

**Key Points:**
1. Check vehicle state
2. Check player state (unconscious, restrained)
3. Check combat (optional)
4. Return false if ANY check fails

---

## 🔗 PATTERN 14: Manager Pattern

**Use:** Centralized management of game system

```c
class CustomManager
{
    protected PlayerBase m_Player;
    protected ref CustomData m_Data;

    // Constructor
    void CustomManager(PlayerBase player)
    {
        m_Player = player;
        m_Data = new CustomData;
        Load();
    }

    // Load/Save
    void Load() { /* Load from JSON */ }
    void Save() { /* Save to JSON */ }

    // Actions
    void DoSomething() { /* Game logic */ }
    void SomethingElse() { /* More logic */ }

    // Queries
    bool CanDoSomething() { return true; }
    int GetValue() { return m_Data.Value; }

    // Events
    void OnEvent() { /* Handle event */ }
}
```

**Examples:**
- Skill Manager: examples/xp_system/scripts/4_World/Classes/SkillManager.c
- Teleport Manager: examples/teleportation/scripts/4_World/Classes/TeleportManager.c
- Storage Manager: examples/storage/scripts/4_World/Classes/StorageManager.c

**Key Points:**
1. Hold reference to player
2. Load in constructor
3. Save manually or in event
4. Separate actions, queries, events
5. Use for complex systems

---

## 📊 PATTERN 15: Stack Item Combination

**Use:** Combine two stackable items

```c
bool CombineItems(ItemBase target, ItemBase source)
{
    // Check if can stack
    if (target.GetType() != source.GetType())
        return false;

    float targetQty = target.GetQuantity();
    float sourceQty = source.GetQuantity();
    float maxQty = target.GetQuantityMax();

    if (targetQty >= maxQty)
        return false; // Target full

    // Calculate transfer
    float space = maxQty - targetQty;
    float toTransfer = Math.Min(space, sourceQty);

    // Transfer
    target.AddQuantity(toTransfer);
    source.AddQuantity(-toTransfer);

    // Delete if empty
    if (source.GetQuantity() <= 0) {
        GetGame().ObjectDelete(source);
    }

    return true;
}
```

**Examples:**
- Auto Stack: examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c:145

**Key Points:**
1. Check same type
2. Check space available
3. Calculate transfer amount
4. Update quantities
5. Delete empty source

---

*Last Updated: 2025-01-17*
*Purpose: AI to apply patterns without searching*
