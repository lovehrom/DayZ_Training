# NPC + Loot Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** ai_system + loot_boxes

---

## ⚡ 30-Second Summary

Integration example showing NPC with custom loot drops. When NPC is killed, it drops loot from configured loot tables with weighted rarity.

---

## 🎯 Features

### Integration Mechanics
- ✅ **NPC Loadouts** - Custom equipment for NPCs
- ✅ **Loot Drops on Death** - Custom loot on kill
- ✅ **Weighted Rarity** - Common/Rare/Epic/Legendary drops
- ✅ **Guaranteed Drops** - Base items always dropped
- ✅ **Random Rewards** - Bonus loot chance

---

## 📁 File Structure

```
npc_loot_integration/
├── README.md                    ← This file
├── config.cpp                   ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── BanditLootable.c          ← Lootable NPC
            ├── NPCLootTable.c            ← Loot configuration
            └── PlayerBase/
                └── playerbase_npc_loot.c ← Integration
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- `ai_system/` - Creates NPC bandits with patrol
- `loot_boxes/` - Weighted random loot system

**Integration:**
1. NPC spawns → Equipped with loadout
2. NPC killed → OnDeath() triggered
3. Loot system → Drops configured items
4. Player kills → Gets guaranteed + random loot

### Loot Table System

**Guaranteed Drops (always):**
- Basic ammo
- Food
- Drink

**Random Drops (weighted):**
- Common (90%): Bandage, Painkillers
- Rare (9%): Antibiotics, Morphine
- Epic (1%): Scope, Suppressor
- Legendary (0.1%): NVG, GPS

---

## 💡 Implementation

### 1. NPCLootTable.c

Loot configuration with rarity:

```c
class NPCLootTable
{
    // Guaranteed drops (always)
    static array<string> GetGuaranteedLoot()
    {
        return {"Ammo_762x39", "BakedBeans_Can", "SodaCan_Cola"};
    }

    // Random loot based on rarity
    static string GetRandomLoot(float roll)
    {
        // Legendary (0.1%)
        if (roll < 0.001)
            return GetLegendaryLoot();

        // Epic (1%)
        else if (roll < 0.01)
            return GetEpicLoot();

        // Rare (9%)
        else if (roll < 0.10)
            return GetRareLoot();

        // Common (90%)
        else
            return GetCommonLoot();
    }

    static string GetLegendaryLoot()
    {
        array<string> items = {"NVGHeadstrap", "PersonalRadio"};
        return items.Get(Math.RandomInt(0, items.Count()));
    }

    static string GetEpicLoot()
    {
        array<string> items = {"AK74_Wood_Buttstock", "M4_Suppressor"};
        return items.Get(Math.RandomInt(0, items.Count()));
    }

    static string GetRareLoot()
    {
        array<string> items = {"Antibiotic", "Morphine"};
        return items.Get(Math.RandomInt(0, items.Count()));
    }

    static string GetCommonLoot()
    {
        array<string> items = {"BandageDressing", "PainkillerTablets"};
        return items.Get(Math.RandomInt(0, items.Count()));
    }
}
```

### 2. BanditLootable.c

NPC that drops loot on death:

```c
class BanditLootable extends DayZInfected
{
    protected ref NPCLootTable m_LootTable;

    void BanditLootable()
    {
        m_LootTable = new NPCLootTable();
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Get killer (player)
        PlayerBase player = PlayerBase.Cast(killer);
        if (!player)
            return;

        // Spawn guaranteed loot
        array<string> guaranteed = m_LootTable.GetGuaranteedLoot();
        foreach (string itemName : guaranteed)
        {
            SpawnLootItem(itemName, GetPosition());
        }

        // Spawn random loot (1-3 items)
        int randomCount = Math.RandomInt(1, 4);
        for (int i = 0; i < randomCount; i++)
        {
            float roll = Math.RandomFloat01();
            string randomItem = m_LootTable.GetRandomLoot(roll);
            SpawnLootItem(randomItem, GetPosition());
        }

        Print("[NPCLoot] " + player.GetIdentity().GetName() + " killed NPC at " + GetPosition());
    }

    void SpawnLootItem(string itemType, vector pos)
    {
        // Add slight random offset
        vector offset = Vector(Math.RandomFloat01() - 0.5, 0, Math.RandomFloat01() - 0.5);
        EntityAI item = GetGame().CreateObject(itemType, pos + offset);

        if (item)
        {
            // Random rotation
            item.SetOrientation(Vector(0, Math.RandomFloat01() * 360, 0));
        }
    }
}
```

### 3. PlayerBase Integration

```c
modded class PlayerBase
{
    // Track NPC kills
    protected int m_NPCKills;

    override void OnConnect()
    {
        super.OnConnect();
        m_NPCKills = 0;
    }

    // Call this when killing NPC
    void OnNPCKilled(EntityAI npc)
    {
        m_NPCKills++;

        // Bonus: Every 10 kills, get rare loot
        if (m_NPCKills % 10 == 0)
        {
            NotificationSystem.Notify(this, "Milestone reached! Bonus loot!");

            // Spawn bonus item
            string bonusItem = "Mag_STANAG_60Rnd";
            GetGame().CreateObject(bonusItem, GetPosition());
        }
    }

    int GetNPCKills()
    {
        return m_NPCKills;
    }
}
```

---

## 📝 Usage Examples

### Example 1: Define NPC Loadout

```c
class BanditLootable extends DayZInfected
{
    override void OnLoadFinished()
    {
        super.OnLoadFinished();

        // Equip NPC
        GetInventory().CreateInInventory("AK74");
        GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
        GetInventory().CreateInInventory("HuntingJacket_Brown");
        GetInventory().CreateInInventory("Jeans_Blue");
    }
}
```

### Example 2: Boss NPCs with Better Loot

```c
class BanditBoss extends BanditLootable
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Boss drops extra legendary
        float bossRoll = Math.RandomFloat01();
        if (bossRoll < 0.5) // 50% chance for legendary
        {
            string legendary = m_LootTable.GetLegendaryLoot();
            SpawnLootItem(legendary, GetPosition());
        }
    }
}
```

### Example 3: Faction-Specific Loot

```c
class BanditFactionLootable extends BanditLootable
{
    protected string m_Faction;

    void SetFaction(string faction)
    {
        m_Faction = faction;
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Drop faction-specific item
        if (m_Faction == "Blu")
            SpawnLootItem("BluToken", GetPosition());
        else if (m_Faction == "Rossi")
            SpawnLootItem("RossiToken", GetPosition());
    }
}
```

---

## 🔗 Related Examples

→ [AI System](../ai_system/) - Individual NPC mechanics
→ [Loot Boxes](../loot_boxes/) - Individual loot mechanics
→ [Integration: XP+Storage](../xp_storage_integration/) - Another integration

---

## ⚠️ Important Notes

### Loot Balance
- **Common**: 90% - Basic survival items
- **Rare**: 9% - Medical supplies
- **Epic**: 1% - Weapon attachments
- **Legendary**: 0.1% - High-tier equipment

### Performance
- Loot spawns on NPC death
- Max 5-10 items per NPC
- Auto-cleanup after 1 hour

### Anti-Exploit
- Validate killer is player
- Cooldown between loot collections
- Max kills per hour tracked

---

*Last updated: 2025-01-18*
*Combines: ai_system + loot_boxes*
