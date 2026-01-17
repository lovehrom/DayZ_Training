# Loot Box System Example

**Status:** ✅ Working Example
**Source:** Material Pack (3620127965) + Specimen RO-9 (3645022168)
**Complexity:** Simple

---

## ⚡ 30-Second Summary

Loot boxes with weighted random drops, rarity tiers (common, rare, epic, legendary), guaranteed + random rewards, and auto-cleanup.

---

## 🎯 Features

- ✅ **Weighted Random** - Different drop chances
- ✅ **Rarity Tiers** - Common (90%), Rare (9%), Epic (1%), Legendary (0.1%)
- ✅ **Guaranteed Drops** - Always get base items
- ✅ **Multiple Rewards** - Configurable count
- ✅ **Auto-Cleanup** - Delete empty crate

---

## 📁 File Structure

```
loot_boxes/
├── README.md
├── config.cpp
└── scripts/
    ├── 4_World/
    │   └── LootBox.c           ← Main loot box class
    └── 3_Game/
        └── LootBoxConfig.c     ← Drop tables
```

---

## 💡 Key Concepts

### Weighted Random System

```c
float roll = Math.RandomFloat01();

if (roll < 0.001)        // 0.1% - Legendary
    reward = GetLegendaryLoot();
else if (roll < 0.01)    // 1% - Epic
    reward = GetEpicLoot();
else if (roll < 0.10)    // 9% - Rare
    reward = GetRareLoot();
else                     // 90% - Common
    reward = GetCommonLoot();
```

---

## 📝 API Reference

### LootBox Class

```c
class LootBox extends ItemBase
{
    // Override to handle opening
    override void Open(ItemBase item)

    // Get random loot from rarity tier
    string GetRandomLoot(int rarity)

    // Get guaranteed loot
    array<string> GetGuaranteedLoot()
}
```

---

## 🎮 Usage Examples

### Example 1: Create Loot Box

```c
// Spawn loot box at position
EntityAI box = GetGame().CreateObject("LootBox", player.GetPosition());
```

### Example 2: Custom Drop Rates

```c
// Edit LootBoxConfig.c
class LegendaryDrops
{
    // 0.1% chance
    array<string> Items = {"AK74", "M4A1", "SVD"};
}
```

---

## 🔗 Related Topics

→ [Loot Boxes](../topics/21_loot_boxes.md) - Mechanics
→ [Storage](../topics/14_storage.md) - Persistence

---

*Последнее обновление: 2025-01-18*
