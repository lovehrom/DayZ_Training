# AI FAQ - Quick Answers

**Purpose:** Instant answers to common questions
**For:** AI to answer without searching
---

## 🔧 ADVANCED QUESTIONS (Phase 7)

### Q: How to create complete trader system?
**A:** Use topics/27_trader_systems.md + examples/trader_system/
1. NPC Trader creation (4_World/TraderNPC.c)
2. Currency system (3_Game/CurrencySystem.c)
3. Price tables (prices.json)
4. Trading UI (5_Mission/TraderMenu.c)
5. Stock management
**Pattern:** INTEGRATION_PATTERNS.md → Economy + Trading

### Q: How to implement quest system?
**A:** Use topics/28_quest_systems.md + examples/quest_system/
1. Quest definitions (quests.json)
2. Quest tracking (3_Game/QuestManager.c)
3. Quest objectives (kill, collect, explore)
4. Quest rewards (XP, items, currency)
5. Quest journal UI
**Pattern:** INTEGRATION_PATTERNS.md → Quest + Rewards

### Q: How to create auction house?
**A:** Use topics/29_auction_systems.md + examples/auction_system/
1. Auction listings (AuctionManager.c)
2. Bidding system (BiddingSystem.c)
3. Buyout prices
4. Mail delivery (Mailbox.c)
5. Currency integration
**Pattern:** INTEGRATION_PATTERNS.md → Auction + Mail

### Q: How to create ATM/banking system?
**A:** Use topics/30_atm_systems.md + examples/atm_system/
1. ATM entities (BuildingBase with TraderID = -2)
2. Credit card items
3. Transaction queue (FIFO)
4. Bank account management
5. Transaction fees
**Pattern:** INTEGRATION_PATTERNS.md → ATM + Currency

### Q: How to use CF RPC correctly?
**A:** Use topics/31_cf_rpc.md + examples/cf_rpc/
**CRITICAL:** Used by 70%+ mods!
1. Register RPC with proper namespace
2. Use CF.ModStorage for cross-server data
3. Singleplayer support
**Example:** topics/31_cf_rpc.md:50-150

### Q: How to make GUI crafting system?
**A:** Use topics/25_crafting_gui.md + examples/gui_crafting/
1. Crafting menu UI (5_Mission/CraftingMenu.c)
2. Recipe definitions (recipes.json)
3. Craft button handler
4. Progress tracking
**Pattern:** INTEGRATION_PATTERNS.md → Crafting + GUI

### Q: How to create faction system?
**A:** Use topics/15_faction_systems.md + examples/faction_system/
1. Faction manager (3_Game/FactionManager.c)
2. Faction membership (map<playerID, faction>)
3. Faction permissions (building, PvP)
4. Faction storage
**Pattern:** INTEGRATION_PATTERNS.md → Faction + Storage

### Q: How to create loot boxes?
**A:** Use reference/LOOT_BOX_SYSTEM.md + examples/loot_boxes/
1. Loot box entity (BuildingBase)
2. Loot table (loot_table.json)
3. OnOpen handler
4. Spawn timer
**Pattern:** code_snippets.md → Pattern 17 (SpawnItemsInArea)

### Q: How to create time-based events?
**A:** Use topics/20_time_events.md + examples/time_events/
1. Time checker (GetGame().GetDayTime())
2. Event trigger (OnWakeUp, OnSleep)
3. Dynamic loot/XP multiplier
**Pattern:** PATTERNS.md → PATTERN 13

### Q: How to send notification to player?
**A:** Use reference/NOTIFICATION_SYSTEM.md + code_snippets.md
**Pattern:** code_snippets.md → Pattern 28 (NotificationManager)

### Q: How to create building system?
**A:** Use topics/24_building_systems.md + examples/building_system/
1. Building kit item (3_Game/BuildingKit.c)
2. Construction action (ActionBuild)
3. Placement check (CanPlaceBuilding)
4. Building placement (PlaceObject)
**Pattern:** PATTERNS.md → PATTERN 19

### Q: How to implement weapon customization?
**A:** Use reference/WEAPON_CUSTOMIZATION.md + examples/weapon_customization/
1. Weapon mods (attachments)
2. Custom stats (damage, range)
3. Camouflage system
4. Durability system

### Q: How to create armor system?
**A:** Use reference/ARMOR_DAMAGE_SYSTEMS.md + examples/armor_system/
1. Damage reduction
2. Hit zones (head, body, legs)
3. Condition system
4. Repair mechanics

### Q: How to add player modifiers?
**A:** Use reference/PLAYER_MODIFIERS.md + examples/player_modifiers/
1. Modifier manager (3_Game/ModifierManager.c)
2. Modifier types (speed, stamina, etc.)
3. Effect stacking
4. Time-based duration
**Pattern:** code_snippets.md → Pattern 26 (CooldownManager)

### Q: How to customize zombies?
**A:** Use reference/ZOMBIE_CUSTOMIZATION.md + examples/zombie_customization/
1. Extend ZombieBase
2. Custom models/textures
3. Custom AI behavior
4. Custom loot tables
**⚠️ CRITICAL:** Extend ZombieBase, NOT InfectedBase!

### Q: How to create integration?
**A:** Use reference/INTEGRATION_PATTERNS.md + examples/integration/
1. Event-driven (events trigger systems)
2. Shared storage (common data)
3. RPC bridges (client-server)
4. Manager pattern (central coordinator)
**Pattern:** code_snippets.md → Pattern 27 (Complete XP System)

### Q: How to secure multiplayer mod?
**A:** Use reference/SECURITY_GUIDE.md + code_snippets.md
**CRITICAL:**
- Validate ALL client input
- Execute critical operations on server
- Use rate limiting
- Check data integrity

### Q: How to optimize performance?
**A:** Use reference/PERFORMANCE_GUIDE.md + code_snippets.md
**Key optimizations:**
- Don't update every frame
- Batch RPC calls
- Entity pooling
- Limit entity count
- Cache expensive operations

### Q: How to deploy mod to Workshop?
**A:** Use topics/26_deployment.md
**Steps:**
1. Build .pbo (AddonBuilder/DZBuilder)
2. Create metadata (meta.cpp)
3. Upload to Workshop (SteamCMD or GUI)
4. Test on local server
5. Create changelog

### Q: How to debug mod issues?
**A:** Use troubleshooting/mechanics_issues.md + troubleshooting/debug_checklist.md
**Process:**
1. Check logs (script.log)
2. Add debug prints
3. Check function signatures
4. Validate parameters
5. Test on clean install

### Q: How to validate player input?
**A:** Use code_snippets.md → Pattern 21 (ValidateStringInput)
**Always validate:**
- String length
- Special characters
- SQL injection attempts
- Overflow attempts

### Q: How to prevent memory leaks?
**A:** Use code_snippets.md → Pattern 26 (LazyObject)
**Common causes:**
- Not cleaning up timers
- Growing arrays
- Event handlers not removed
- Objects not deleted

### Q: How to get player Steam64 ID?
**A:** Use api_quick_ref.md → Player Operations
```c
string steam64 = player.GetIdentity().GetId();
// Use for unique player identification
```

### Q: How to check surface height?
**A:** Use api_quick_ref.md → GetGame() → World & Environment
```c
float y = GetGame().SurfaceY(x, z);
```

### Q: How to spawn items safely?
**A:** Use code_snippets.md → Pattern 14 (SpawnItem)
**Safety checks:**
- Check item type
- Validate position
- Use surface height
- Check entity limits

### Q: How to create timed events?
**A:** Use code_snippets.md → Pattern 8 (SimpleTimer)
**Pattern:**
```c
protected float m_LastUpdate;
protected const float INTERVAL = 1.0;

void Update()
{
    float now = GetGame().GetCallQueue().CallLater(Update, INTERVAL);
}
```

### Q: How to create cooldown?
**A:** Use code_snippets.md → Pattern 10 (CooldownManager)
**Pattern:**
```c
bool CanAct(string playerID, float cooldownSeconds)
{
    if (!m_LastActionTime.Contains(playerID)) return true;

    float now = GetGame().GetTickTime();
    return (now - m_LastActionTime[playerID] >= cooldownSeconds);
}
```

### Q: How to batch RPC calls?
**A:** Use code_snippets.md → Pattern 24 (Batch RPC)
**Example:**
```c
// Instead of 1000 individual RPCs:
for (int i = 0; i < 1000; i++) {
    GetRPCManager().SendRPC("MyMod", "Update", new Param1<int>(i), true);
}

// Use single batch RPC:
array<int> data;
for (int i = 0; i < 1000; i++) {
    data.Insert(i);
}
GetRPCManager().SendRPC("Mod", "UpdateBatch", new Param1<array<int>>(data), true);
```

### Q: How to use GetGame() correctly?
**A:** Use api_quick_ref.md → GetGame() - Core Game Access
**Common uses:**
- CreateObject() - Spawn entities
- GetPlayer() - Get current player
- GetTickTime() - Server time
- SurfaceY() - Get ground height

### Q: How to cast between types?
**A:** Use api_quick_ref.md → EntityAI → Casting
**Pattern:**
```c
// Cast EntityAI to ItemBase:
ItemBase item = ItemBase.Cast(entity);
if (item) {
    // Successfully cast
}
```

### Q: How to handle exceptions?
**A:** DayZ doesn't use try-catch
**Instead:** Use null checks
```c
EntityAI entity = GetGame().CreateObject("Item", pos);
if (!entity) {
    Print("[ERROR] Failed to spawn!");
    return;
}
```

### Q: How to create manager class?
**A:** Use code_snippets.md → Pattern 27 (Complete XP System)
**Pattern:**
```c
class MyManager {
    protected map<string, int> m_Data;

    void MyManager(PlayerBase player) {
        m_Player = player;
        Load();
    }

    void Save() {
        // Save to JSON
    }

    void Load() {
        // Load from JSON
    }
}
```

### Q: How to validate JSON data?
**A:** Use troubleshooting/mechanics_issues.md → Storage Debug Checklist
**Check:**
- File not empty
- Types match save/load
- No parse errors

### Q: How to check if server?
**A:** Use api_quick_ref.md → GetGame() → Time & World
```c
bool IsServer() {
    return GetGame().IsServer();
}
```

### Q: How to get all players?
**A:** Use api_quick_ref.md → Player Management → Get All Players
**Pattern:**
```c
array<PlayerBase> players = GetAllPlayers();
```

### Q: How to find nearest object?
**A:** Use code_snippets.md → Pattern 7 (FindNearest)
**Pattern:**
```c
PlayerBase FindNearest(vector pos, float maxDist = 100) {
    array<PlayerBase> players = new array<PlayerBase>;
    GetGame().GetPlayers(players);
    PlayerBase nearest = NULL;
    float minDist = maxDist;

    foreach (PlayerBase p : players) {
        float d = vector.Distance(pos, p.GetPosition());
        if (d < minDist) {
            minDist = d;
            nearest = p;
        }
    }

    return nearest;
}
```

### Q: How to use ARGB colors?
**A:** Use api_quick_ref.md → Colors → ARGB Format
**Pattern:**
```c
text.SetColor(ARGB(255, 255, 0, 0));  // Red
text.SetColor(ARGB(128, 255, 255, 255));  // Semi-transparent white
```

### Q: How to use Math functions?
**A:** Use api_quick_ref.md → Math Functions
**Common:**
- Math.RandomFloat01() - Random 0.0-1.0
- Math.Clamp(value, min, max) - Clamp value
- Math.Abs(x) - Absolute value
- Math.Sin(angle) - Sine (angle in radians)

### Q: How to use arrays and maps?
**A:** Use api_quick_ref.md → Utility Functions
**Arrays:**
```c
array<EntityAI> entities;
entities.Insert(entity);  // Add to end
entities.Get(0);           // Get first
entities.Count();          // Get count
```

**Maps:**
```c
map<string, int> data;
data.Set("key", 123);     // Set key-value pair
data.Get("key");          // Get value by key
data.Contains("key");    // Check if contains
```

---

## 🎓 LEARNING PATTERNS

### ⚠️ CRITICAL: Base Class for NPC

**Q: Which base class for NPC?**
**A:** **InfectedBanditBase** (NEVER AnimalBase!)

**Why InfectedBanditBase:**
- ✅ Has proper animations
- ✅ Has AI behavior
- ✅ Vanilla compatible
- ✅ Used in all working examples

**Why NOT AnimalBase:**
- ❌ Wrong animations
- ❌ No AI behavior
- ❌ Limited compatibility

### ⚠️ CRITICAL: Server Authority

**Q: Can client execute code?**
**A:** NO! Client must send RPC to server

**❌ WRONG:**
```c
// Client-side (exploitable!)
void AddXP(int amount) {
    m_XP += amount;  // Client can send ANY amount!
}
```

**✅ CORRECT:**
```c
// Client: Send request
void RequestAddXP(int amount) {
    GetRPCManager().SendRPC("MyMod", "AddXP", new Param1<int>(amount), true);
}

// Server: Validate and execute
void ServerAddXP(PlayerIdentity sender, Object target, ParamsReadContext ctx) {
    int amount;
    if (!ctx.Read(amount)) return;

    // Validate
    if (amount > 100) {
        Print("[Security] Invalid XP amount: " + amount);
        return;
    }

    m_XP[sender.GetId()] += amount;
}
```

---

**Last Updated:** 2025-01-18 (Phase 7 Complete)

---

## 🎯 MOST COMMON QUESTIONS

### Q: How to create XP system?
**A:** Use examples/xp_system/
1. Copy files to your project
2. Add to config.cpp
3. Server restart
**Details:** topics/12_xp_systems.md
**API:** API_SIGNATURES.md → XP SYSTEM
**Pattern:** PATTERNS.md → PATTERN 1, 9, 14

---

### Q: How to create custom NPC?
**A:** Use examples/custom_zombie/
1. Extend InfectedBanditBase (NOT AnimalBase!)
2. Load config from JSON
3. Spawn loot in EEKilled()
**Details:** topics/15_zombie_ai.md
**Code:** examples/custom_zombie/scripts/.../CustomBandit.c
**⚠️ CRITICAL:** Always use InfectedBanditBase, never AnimalBase!

---

### Q: How to make cross-server storage?
**A:** Use examples/storage/
1. Steam64 ID-based storage
2. Recursive save (attachments, cargo)
3. JSON persistence at $profile:
**Details:** topics/14_storage.md
**API:** API_SIGNATURES.md → STORAGE
**Pattern:** PATTERNS.md → PATTERN 4

---

### Q: How to teleport player?
**A:** Use examples/teleportation/
1. Command: `/tp <location>`
2. JSON config at $profile:teleports.json
3. Safety checks (not in vehicle, etc.)
**Details:** topics/13_teleportation.md
**API:** API_SIGNATURES.md → TELEPORTATION
**Pattern:** PATTERNS.md → PATTERN 5, 13

---

### Q: Which base class for NPC?
**A:** **InfectedBanditBase** (never AnimalBase!)
**Why:**
- ✅ Correct animations
- ✅ Proper AI behavior
- ✅ Vanilla compatibility
**Example:** examples/custom_zombie/scripts/.../CustomBandit.c

---

### Q: How to save player data?
**A:** Use JSON persistence
**Pattern:** PATTERNS.md → PATTERN 2
**File path:** `$profile:data_{steam64}.json`
**Code:**
```c
string steam64 = player.GetIdentity().GetId();
string filePath = "$profile:data_" + steam64 + ".json";
JsonFileLoader<Data>::JsonSaveFile(filePath, data);
```

---

### Q: How to get player Steam64 ID?
**A:**
```c
string steam64 = player.GetIdentity().GetId();
```
**Use for:** Player-specific data, storage, etc.

---

### Q: How to give XP for action?
**A:** Override event handler
**Pattern:** PATTERNS.md → PATTERN 3
**Example (kill):**
```c
override void EEKilled(Object killer) {
    super.EEKilled();
    if (killer && killer.IsPlayer()) {
        PlayerBase player = PlayerBase.Cast(killer);
        player.m_SkillManager.AddXP("Combat", 10);
    }
}
```

---

### Q: How to stack items automatically?
**A:** Use examples/auto_stack/
**Trigger:** OnInventoryEnter()
**Pattern:** PATTERNS.md → PATTERN 15
**Code:** examples/auto_stack/scripts/.../playerbase_autostack.c

---

### Q: How to create weighted loot table?
**A:** Use weighted random
**Pattern:** PATTERNS.md → PATTERN 6
**Example:**
```c
class LootEntry { string Name; float Weight; }
// Weights: 50, 30, 20 = 50%, 30%, 20% (not 0.5, 0.3, 0.2)
```

---

### Q: How to handle chat commands?
**A:** Override OnChat()
**Pattern:** PATTERNS.md → PATTERN 5
**Example:**
```c
override bool OnChat(PlayerBase player, string message) {
    if (super.OnChat(player, message)) return true;
    if (message.StartsWith("/cmd ")) {
        HandleCommand(message.Substring(5));
        return true;
    }
    return false;
}
```

---

### Q: How to find nearest player?
**A:**
**Pattern:** PATTERNS.md → PATTERN 7
```c
PlayerBase FindNearest(vector pos, float maxDist = 100) {
    array<PlayerBase> players = new array<PlayerBase>;
    GetGame().GetPlayers(players);
    PlayerBase nearest = null;
    float minDist = maxDist;
    foreach (PlayerBase p : players) {
        float d = vector.Distance(pos, p.GetPosition());
        if (d < minDist) { minDist = d; nearest = p; }
    }
    return nearest;
}
```

---

### Q: How to add cooldown?
**A:**
**Pattern:** PATTERNS.md → PATTERN 8
```c
protected float m_LastTime;
protected const float COOLDOWN = 5.0;
bool CanUse() {
    return (GetGame().GetTickTime() - m_LastTime) >= COOLDOWN;
}
```

---

### Q: How to show notification to player?
**A:**
**Pattern:** PATTERNS.md → PATTERN 12
```c
Param1<string> msg = new Param1<string>("Message");
GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, player.GetIdentity());
```

---

### Q: How to check if item is stackable?
**A:**
```c
bool IsStackable(ItemBase item) {
    if (!item) return false;
    if (item.IsInherited(Magazine)) return false;
    if (item.IsInherited(Weapon)) return false;
    if (item.GetInventory().GetCargo()) return false;
    return item.GetQuantity() > 0 && item.GetQuantity() < item.GetQuantityMax();
}
```

---

### Q: How to recursively save items?
**A:**
**Pattern:** PATTERNS.md → PATTERN 4
**Full code:** examples/storage/scripts/.../StorageManager.c:50
**Key:** Handle attachments AND cargo

---

### Q: How to spawn item?
**A:**
```c
EntityAI item = GetGame().CreateObject("ClassName", position);
// OR
EntityAI item = GetGame().CreateObjectEx("ClassName", position, ECE_PLACE_ON_SURFACE);
```

---

### Q: How to delete item?
**A:**
```c
GetGame().ObjectDelete(item);
```

---

### Q: How to get all items in inventory?
**A:**
```c
array<EntityAI> items = new array<EntityAI>;
inventory.EnumerateInventory(InventoryTraversalType.PREORDER, items);
```

---

### Q: How to cast EntityAI to ItemBase?
**A:**
```c
ItemBase item = ItemBase.Cast(entity);
if (item) {
    // Successfully cast
}
```

---

### Q: How to check if object is player?
**A:**
```c
if (entity.IsMan()) {
    Man man = entity.CastToMan();
    if (man.IsPlayer()) {
        PlayerBase player = PlayerBase.Cast(man);
        // Use player
    }
}
```

---

### Q: How to get game time?
**A:**
```c
float time = GetGame().GetTickTime(); // Server time in seconds
```

---

### Q: How to check if file exists?
**A:**
```c
if (FileExist("$profile:config.json")) {
    // File exists
}
```

---

### Q: Where to save player data?
**A:** `$profile:` folder
**Examples:**
- `$profile:skills_{steam64}.json`
- `$profile:storage_{steam64}.json`
- `$profile:teleports.json`

---

### Q: How to create manager class?
**A:**
**Pattern:** PATTERNS.md → PATTERN 14
**Structure:**
```c
class CustomManager {
    protected PlayerBase m_Player;
    void CustomManager(PlayerBase player) { m_Player = player; Load(); }
    void Load() { /* Load JSON */ }
    void Save() { /* Save JSON */ }
    void DoAction() { /* Game logic */ }
}
```

---

### Q: How to integrate manager in PlayerBase?
**A:**
**Pattern:** PATTERNS.md → PATTERN 1
```c
modded class PlayerBase {
    protected CustomManager m_Manager;
    override void OnConnect() {
        super.OnConnect();
        m_Manager = new CustomManager(this);
    }
    override void OnDisconnect() {
        super.OnDisconnect();
        if (m_Manager) m_Manager.Save();
    }
}
```

---

### Q: What order to call super methods?
**A:**
**OnConnect(), OnDisconnect(), OnUpdate():** Call super FIRST
**EEKilled(), EEHitBy(), EE*():** Call super FIRST
**OnChat():** Call super FIRST, check return value

**Example:**
```c
override void OnConnect() {
    super.OnConnect(); // ALWAYS FIRST
    // Your code
}
```

---

### Q: How to create custom event?
**A:**
```c
modded class PlayerBase {
    override void SomeEvent() {
        super.SomeEvent();
        // Your custom logic
        if (m_Manager) {
            m_Manager.OnSomeEvent();
        }
    }
}
```

---

### Q: How to make GUI crafting?
**A:**
**Reference:** reference/CRAFTING_GUI_SYSTEMS.md
**Note:** Complex, requires modded CraftingManager

---

### Q: How to create faction system?
**A:**
**Reference:** reference/FACTION_SYSTEMS.md
**Source:** TheHive mod (3572908305)

---

### Q: How to add custom animations?
**A:**
**Reference:** reference/ANIMATION_SYSTEM.md
**Complex:** High

---

### Q: How to create loot box?
**A:**
**Reference:** reference/LOOT_BOX_SYSTEMS.md
**Pattern:** PATTERNS.md → PATTERN 6 (weighted random)

---

### Q: How to make time-based events?
**A:**
**Reference:** reference/TIME_BASED_EVENTS.md
**Key:** GetGame().GetWorld().GetWorldTime()
**Format:** 0-24 (6.0 = 6 AM, 20.0 = 8 PM)

---

### Q: How to modify weapon recoil?
**A:**
**Reference:** reference/WEAPON_CUSTOMIZATION.md
**Method:** Override SpawnRecoilObject()

---

### Q: How to preserve ADS on reload?
**A:**
**Reference:** reference/WEAPON_IMPROVEMENTS.md
**Source:** BetterBoltAction (3641964151)

---

### Q: How to add armor to clothing?
**A:**
**Reference:** reference/ARMOR_DAMAGE_SYSTEMS.md
**Key:** DamageModifiers in config.cpp

---

### Q: How to use Community Framework (CF)?
**A:**
**Reference:** reference/CF_RPC_SYSTEM.md
**Key Features:**
- Namespace RPC (no numeric IDs)
- Attribute events
- Simplified storage

---

### Q: What are common pitfalls?
**A:**
1. ❌ Using AnimalBase for NPC → Use InfectedBanditBase
2. ❌ Forgetting super.OnConnect() → Always call first
3. ❌ Hardcoded paths → Use $profile:
4. ❌ Not checking null → Always validate
5. ❌ Not saving data → Save in OnDisconnect()

---

### Q: How to get started?
**A:**
1. Read: 00_INDEX.md
2. Choose example: examples/
3. Copy to your project
4. Modify as needed

---

### Q: Where to find specific mechanic?
**A:**
1. Check: MASTER_INDEX.md (read this first!)
2. Search: search_index.md
3. Deep dive: reference/

---

### Q: How to report bug?
**A:**
Check: troubleshooting/common_errors.md

---

## 🔥 CRITICAL REMINDERS

### ALWAYS:
1. ✅ Call super.EVENT() first
2. ✅ Check null before using
3. ✅ Use InfectedBanditBase for NPC
4. ✅ Use $profile: for files
5. ✅ Save data in OnDisconnect()

### NEVER:
1. ❌ Use AnimalBase for NPC
2. ❌ Forget super methods
3. ❌ Hardcode paths
4. ❌ Skip null checks
5. ❌ Forget to save

---

*Last Updated: 2025-01-17*
*Purpose: AI instant answers without searching*
