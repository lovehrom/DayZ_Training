# MASTER INDEX - For AI

**Purpose:** Complete index of ALL mechanics in DayZ_Training
**For:** AI quick lookup (1 second instead of 15)
**Last Updated:** 2025-01-18 (Phase 6 Complete)

---

## 🚀 QUICK LOOKUP (Most Common)

### XP Systems
**Where:**
- Topic: topics/12_xp_systems.md (150 lines)
- Reference: reference/XP_SKILL_SYSTEMS.md (450 lines)
- Example: examples/xp_system/ (5 files, 400 lines)

**Key Files:**
- examples/xp_system/scripts/4_World/Classes/SimpleSkill.c (50 lines)
- examples/xp_system/scripts/4_World/Classes/SkillManager.c (120 lines)
- examples/xp_system/scripts/4_World/Classes/PlayerBase/playerbase_xp.c (60 lines)

**Common Tasks:**
- Add XP for zombie kill → playerbase_xp.c:52
- Create new skill → SkillManager.c:23
- Change XP values → SkillManager.c:89
- Save skills → SkillManager.c:45
- Level up system → SimpleSkill.c:25

---

### Teleportation
**Where:**
- Topic: topics/13_teleportation.md (180 lines)
- Reference: reference/TELEPORTATION_SYSTEMS.md (350 lines)
- Example: examples/teleportation/ (6 files, 300 lines)

**Key Files:**
- examples/teleportation/scripts/4_World/Classes/TeleportManager.c (150 lines)
- examples/teleportation/scripts/4_World/Classes/PlayerBase/playerbase_teleport.c (50 lines)
- examples/teleportation/teleports.json (JSON config)

**Common Tasks:**
- Add teleport point → teleports.json:5
- Change command → playerbase_teleport.c:32
- Add safety check → TeleportManager.c:89

---

### Cross-Server Storage
**Where:**
- Topic: topics/14_storage.md (220 lines)
- Reference: reference/PERSISTENT_STORAGE.md (500 lines)
- Example: examples/storage/ (4 files, 450 lines)

**Key Files:**
- examples/storage/scripts/4_World/Classes/StorageManager.c (250 lines)
- examples/storage/scripts/4_World/Classes/StoredItem.c (15 lines)
- examples/storage/scripts/4_World/Classes/PersonalStorageBox.c (120 lines)

**Common Tasks:**
- Store item → StorageManager.c:145
- Restore items → StorageManager.c:180
- Recursive save → StorageManager.c:65
- Steam64 ID → StorageManager.c:15

---

### Custom NPC (Zombie-based)
**Where:**
- Topic: topics/15_zombie_ai.md (200 lines)
- Reference: reference/AI_NPC_CLASSES.md (400 lines)
- Example: examples/custom_zombie/ (5 files, 250 lines)

**Key Files:**
- examples/custom_zombie/scripts/4_World/Classes/CustomBandit.c (120 lines)
- examples/custom_zombie/scripts/4_World/Classes/AIBanditConfig.c (8 lines)
- examples/custom_zombie/bandit_config.json (JSON)
- examples/custom_zombie/config.cpp (Config)

**Common Tasks:**
- Create NPC class → CustomBandit.c:5
- Load JSON config → CustomBandit.c:35
- Spawn loot → CustomBandit.c:55
- NPC behavior → CustomBandit.c:95

---

### Auto Stack Items
**Where:**
- Reference: reference/AUTO_STACK_MANAGEMENT.md (300 lines)
- Example: examples/auto_stack/ (2 files, 180 lines)

**Key Files:**
- examples/auto_stack/scripts/4_World/Classes/PlayerBase/playerbase_autostack.c (150 lines)

**Common Tasks:**
- Auto-stack on pickup → playerbase_autostack.c:12
- Stack two items → playerbase_autostack.c:145
- Check if stackable → playerbase_autostack.c:65
- Cooldown → playerbase_autostack.c:8

---

## 📚 ALPHABETICAL INDEX

### A
- **Actions** → topics/03_actions.md
- **Advanced Modding** → topics/11_advanced_modding.md
- **Advanced Topics** → topics/34_advanced_topics.md
- **AI Bandits (NPC)** → reference/AI_NPC_CLASSES.md | examples/custom_zombie/
- **AI JSON Config** → reference/AI_JSON_CONFIG.md
- **AI System (Vanilla)** → topics/10_ai.md | reference/AI_SYSTEM.md
- **Animation System** → reference/ANIMATION_SYSTEM.md
- **Armor Systems** → reference/ARMOR_DAMAGE_SYSTEMS.md
- **ATM System** → topics/30_atm_systems.md | reference/ATM_SYSTEM.md | examples/atm_system/
- **Auction System** → topics/29_auction_systems.md | reference/AUCTION_SYSTEM.md
- **Auto Stack** → reference/AUTO_STACK_MANAGEMENT.md | examples/auto_stack/

### B
- **Better Bolt Action** → reference/WEAPON_IMPROVEMENTS.md
- **BZ Garage** → reference/VEHICLE_SYSTEMS.md
- **Building System (Mod)** → reference/BUILDING_SYSTEMS_MOD.md
- **Building System (Vanilla)** → reference/BUILDING_SYSTEM.md

### C
- **Central Economy** → topics/33_central_economy.md | reference/CENTRAL_ECONOMY.md
- **CF Event System** → reference/CF_EVENT_SYSTEM.md
- **CF Framework** → reference/CF_FRAMEWORK.md
- **CF ModStorage** → reference/CF_MODSTORAGE.md
- **CF RPC System** → topics/31_cf_rpc.md | reference/CF_RPC_SYSTEM.md | examples/cf_rpc/
- **Collections API** → reference/COLLECTIONS_AND_TYPEDEFS.md
- **Config** → topics/02_config.md
- **Crafting (GUI)** → reference/CRAFTING_GUI_SYSTEMS.md | examples/gui_crafting/
- **Crafting (Vanilla)** → topics/06_crafting.md
- **Custom RPC** → reference/CUSTOM_RPC.md

### D
- **Damage System** → reference/ARMOR_DAMAGE_SYSTEMS.md
- **DMT Starter Shed** → reference/BUILDING_SYSTEMS_MOD.md
- **Dutch Bunker** → reference/ZOMBIE_CUSTOMIZATION.md

### E
- **Economy** → topics/05_economy.md
- **EnderChest** → reference/PERSISTENT_STORAGE.md
- **Enforce Script Core** → reference/ENFORCE_SCRIPT_CORE.md
- **EntityAI Lifecycle** → reference/ENTITYAI_LIFECYCLE.md
- **Events** → topics/07_events.md

### F
- **Faction System** → reference/FACTION_SYSTEMS.md

### G
- **GetGame API** → reference/GETGAME_API.md
- **Guide System** → reference/TELEPORTATION_SYSTEMS.md

### H
- **Hook Points** → reference/HOOK_POINTS.md
- **The Hive** → reference/FACTION_SYSTEMS.md

### I
- **Infection System** → reference/MEDIUM_PRIORITY_SYSTEMS.md
- **Input Device System** → reference/INPUT_DEVICE_SYSTEM.md
- **Integration Patterns** → reference/INTEGRATION_PATTERNS.md | examples/integration/
- **Items** → topics/01_items.md
- **Inventory System** → reference/INVENTORY_SYSTEM.md
- **Insurance System** → reference/MEDIUM_PRIORITY_SYSTEMS.md

### J
- **JSON Parsing** → reference/JSON_PARSING.md

### L
- **Loot Boxes** → reference/LOOT_BOX_SYSTEMS.md

### M
- **Mail System** → topics/32_mail_systems.md | reference/MAIL_SYSTEM.md | examples/mail_system/
- **Material Pack** → reference/CRAFTING_GUI_SYSTEMS.md | reference/LOOT_BOX_SYSTEMS.md
- **Materials and Textures** → reference/MATERIALS_AND_TEXTURES.md
- **Medical System** → reference/MEDICAL_SYSTEM.md
- **Medium Priority Systems** → reference/MEDIUM_PRIORITY_SYSTEMS.md
- **Mission API** → reference/MISSION_API.md
- **Modifiers (Player)** → reference/PLAYER_MODIFIERS.md

### N
- **Network (RPC)** → topics/08_network.md | reference/RPC_NETWORKING.md
- **Notifications** → reference/NOTIFICATION_SYSTEM.md

### P
- **Particle System** → reference/PARTICLE_SYSTEM.md
- **Persistent Storage** → topics/14_storage.md | reference/PERSISTENT_STORAGE.md | examples/storage/
- **Physics System** → reference/PHYSICS_SYSTEM.md
- **PlayerBase API** → topics/04_playerbase.md | reference/PLAYERBASE_API.md
- **PlayerModifiers** → reference/PLAYER_MODIFIERS.md
- **Performance Guide** → reference/PERFORMANCE_GUIDE.md

### R
- **Redbull Energy Drinks** → reference/PLAYER_MODIFIERS.md
- **RPC Networking** → topics/08_network.md | reference/RPC_NETWORKING.md

### S
- **Safe Zone System** → reference/SAFE_ZONE_SYSTEM.md
- **Security Guide** → reference/SECURITY_GUIDE.md
- **SliverWeapons** → reference/CUSTOM_RPC.md | reference/WEAPON_CUSTOMIZATION.md
- **Sound System** → reference/SOUND_SYSTEM.md
- **SLP Realistic Ambient** → reference/TIME_BASED_EVENTS.md
- **SLP Realistic Survival** → reference/PLAYER_MODIFIERS.md
- **Specimen RO-9** → reference/ZOMBIE_CUSTOMIZATION.md | reference/LOOT_BOX_SYSTEMS.md
- **Storage System** → reference/STORAGE_SYSTEM.md
- **String Parsing** → reference/STRINGS_AND_PARSING.md
- **Striker Clothes** → reference/ARMOR_DAMAGE_SYSTEMS.md
- **Symptoms & Effects** → reference/SYMPTOMS_EFFECTS.md

### T
- **Teleportation** → topics/13_teleportation.md | reference/TELEPORTATION_SYSTEMS.md | examples/teleportation/
- **Temperature System** → reference/TEMPERATURE_SYSTEM.md
- **Time-Based Events** → reference/TIME_BASED_EVENTS.md
- **Timer and CallQueue API** → reference/TIMER_AND_QUEUE_API.md
- **Trader System** → topics/27_trader_systems.md | reference/TRADER_SYSTEM.md | examples/trader_system/
- **Troubleshooting** → troubleshooting/common_errors.md

### U
- **UI Framework** → reference/UI_FRAMEWORK.md
- **UI Widget Methods** → reference/UI_WIDGET_METHODS.md
- **UI System** → topics/09_ui.md | reference/UI_HUD_SYSTEM.md

### V
- **Vehicle Lock System** → reference/VE_VEHICLE_LOCK_SYSTEM.md
- **Vehicle Systems** → reference/VEHICLE_SYSTEMS.md
- **Visual Object API** → reference/VISUAL_OBJECT_API.md


### W
- **Waraeger Armory** → reference/ARMOR_DAMAGE_SYSTEMS.md
- **Weapon Customization** → reference/WEAPON_CUSTOMIZATION.md
- **Weapon Improvements** → reference/WEAPON_IMPROVEMENTS.md

### X
- **XP Skill Systems** → topics/12_xp_systems.md | reference/XP_SKILL_SYSTEMS.md | examples/xp_system/

### Z
- **Zombie AI (Custom)** → topics/15_zombie_ai.md | reference/AI_NPC_CLASSES.md | examples/custom_zombie/
- **Zombie Bears** → reference/ZOMBIE_CUSTOMIZATION.md
- **Zombie Customization** → reference/ZOMBIE_CUSTOMIZATION.md

---

## 🎯 BY CATEGORY

### Advanced Mechanics (Mod-derived)
- topics/11_advanced_modding.md - Overview of 22 mechanics
- topics/12_xp_systems.md - XP & Skill systems
- topics/13_teleportation.md - Teleportation
- topics/14_storage.md - Cross-server storage
- topics/15_zombie_ai.md - Custom NPC

### Infrastructure & Frameworks
- reference/CF_RPC_SYSTEM.md - Namespace RPC
- reference/CF_EVENT_SYSTEM.md - Attribute events
- reference/CF_MODSTORAGE.md - Simplified storage
- reference/JSON_PARSING.md - JSON loading

### Gameplay Mechanics
- reference/XP_SKILL_SYSTEMS.md - RPG progression
- reference/FACTION_SYSTEMS.md - Faction systems
- reference/TELEPORTATION_SYSTEMS.md - Teleportation
- reference/LOOT_BOX_SYSTEMS.md - Loot crates

### Storage Systems
- reference/PERSISTENT_STORAGE.md - Cross-server storage
- reference/STORAGE_SYSTEM.md - Vanilla storage

### AI & NPCs
- reference/AI_NPC_CLASSES.md - Custom AI classes
- reference/AI_JSON_CONFIG.md - JSON configuration
- reference/ZOMBIE_CUSTOMIZATION.md - Custom zombies

### Crafting & GUI
- reference/CRAFTING_GUI_SYSTEMS.md - GUI crafting
- reference/AUTO_STACK_MANAGEMENT.md - Auto stacking

### Weapons & Combat
- reference/WEAPON_CUSTOMIZATION.md - Custom weapons
- reference/WEAPON_IMPROVEMENTS.md - Weapon improvements
- reference/ARMOR_DAMAGE_SYSTEMS.md - Armor systems

### Time & Events
- reference/TIME_BASED_EVENTS.md - Day/night events

### Player Systems
- reference/PLAYER_MODIFIERS.md - Player modifiers
- reference/NOTIFICATION_SYSTEM.md - Notifications

### Vehicles
- reference/VEHICLE_SYSTEMS.md - Vehicle systems

### Building
- reference/BUILDING_SYSTEMS_MOD.md - Mod building

---

## 📁 WORKING EXAMPLES

### Complete Mods (Ready to Use)
- examples/xp_system/ - XP system (5 files, 400 lines)
- examples/teleportation/ - Teleportation (6 files, 300 lines)
- examples/storage/ - Cross-server storage (4 files, 450 lines)
- examples/custom_zombie/ - Custom NPC (5 files, 250 lines)
- examples/auto_stack/ - Auto stacking (2 files, 180 lines)

### Phase 3 Systems (Trader, Quest, Auction)
- examples/gui_crafting/ - GUI crafting (12 files, 2243 lines)
- examples/faction_system/ - Factions (6 files, 1109 lines)
- examples/loot_boxes/ - Loot boxes (5 files, 450 lines)
- examples/time_events/ - Day/night events (4 files, 350 lines)
- examples/notifications/ - Notifications (6 files, 400 lines)
- examples/building_system/ - Base building (8 files, 800 lines)
- examples/player_modifiers/ - Player modifiers (5 files, 300 lines)
- examples/weapon_customization/ - Custom weapons (7 files, 500 lines)
- examples/armor_system/ - Armor systems (6 files, 400 lines)
- examples/zombie_customization/ - Custom zombies (5 files, 350 lines)

### Phase 4 Systems (ATM, CF RPC, Mail)
- examples/atm_system/ - ATM/Banking (12 files, 2000 lines)
- examples/cf_rpc/ - CF RPC framework (8 files, 1000 lines)
- examples/mail_system/ - Mail system (5 files, 400 lines)

### Integration Examples
- examples/integration/xp_storage_integration/ - XP + Storage
- examples/integration/faction_teleport_integration/ - Faction + Teleport
- examples/integration/npc_loot_integration/ - NPC + Loot
- examples/integration/time_spawn_integration/ - Time + Spawning
- examples/integration/vehicle_storage_integration/ - Vehicle + Storage
- examples/integration/building_permissions_integration/ - Building + Factions
- examples/integration/economy_crafting_integration/ - Economy + Crafting
- examples/integration/notification_achievement_integration/ - Notifications + XP
- examples/integration/multiple_modifiers_integration/ - Multiple modifiers
- examples/integration/full_mod_integration/ - ALL systems combined

### Basic Examples
- examples/minimal_item/ - Minimal item example

---

## 🔗 CROSS REFERENCES

### When user asks about XP:
1. Quick answer: topics/12_xp_systems.md
2. Deep dive: reference/XP_SKILL_SYSTEMS.md
3. Working code: examples/xp_system/

### When user asks about NPC:
1. Quick answer: topics/15_zombie_ai.md
2. Deep dive: reference/AI_NPC_CLASSES.md
3. Working code: examples/custom_zombie/
4. IMPORTANT: Use InfectedBanditBase, NOT AnimalBase!

### When user asks about storage:
1. Quick answer: topics/14_storage.md
2. Deep dive: reference/PERSISTENT_STORAGE.md
3. Working code: examples/storage/
4. Key: Steam64 ID-based

### When user asks about teleportation:
1. Quick answer: topics/13_teleportation.md
2. Deep dive: reference/TELEPORTATION_SYSTEMS.md
3. Working code: examples/teleportation/
4. Command: /tp <location>

---

## ⚠️ CRITICAL NOTES FOR AI

### ALWAYS REMEMBER:
1. **NPC base class:** Use InfectedBanditBase, NOT AnimalBase!
2. **JSON path:** Use $profile: for JSON files
3. **Steam64 ID:** GetIdentity().GetId() for player ID
4. **Super calls:** Always call super.EVENT() first
5. **Modded classes:** Use modded class keyword

### COMMON PITFALLS:
- ❌ Using AnimalBase for NPC → Use InfectedBanditBase
- ❌ Forgetting super.OnConnect() → Always call first
- ❌ Hardcoded paths → Use $profile:
- ❌ Not checking null → Always validate
- ❌ Forgetting to Save() → Save in OnDisconnect()

### FILE STRUCTURE:
```
scripts/4_World/Classes/
├── SimpleSkill.c           ← Class definition
├── SkillManager.c          ← Manager class
└── PlayerBase/
    └── playerbase_xp.c     ← Integration (modded class)
```

---

*Last Updated: 2025-01-17*
*Purpose: AI quick lookup (read this first!)*
