# Loot Box Systems (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Inventory System](INVENTORY_SYSTEM.md) | [EntityAI Lifecycle](ENTITYAI_LIFECYCLE.md)

---

## ⚡ 30-Second Summary

Системы лут-боксов: случайные награды, weighted chances, rarity tiers, автоматическая очистка.

---

## 🎯 5-Minute Solution: Simple Loot Box

### Loot box item

```c
class LootBox extends ItemBase
{
    override void Open(ItemBase item)
    {
        super.Open(item);

        // Define loot table
        array<string> common = {"Apple", "SodaCan", "Bandage"};
        array<string> rare = {"Rifle", "Magazine", "Optic"};
        array<string> legendary = {"NVG", "GPS", "ModernOptic"};

        // Roll for rarity
        float roll = Math.RandomFloat01();

        string reward;

        if (roll < 0.01) // 1% legendary
        {
            reward = legendary.GetRandomElement();
        }
        else if (roll < 0.15) // 14% rare
        {
            reward = rare.GetRandomElement();
        }
        else // 85% common
        {
            reward = common.GetRandomElement();
        }

        // Spawn reward
        GetGame().CreateObject(reward, GetPosition());

        // Delete box
        GetGame().ObjectDelete(this);
    }
}
```

---

## 📚 Deep Dive

## Material pack Free (3620127965) - Gift Box Mechanics

### Random reward system with weights

```c
class GiftBox extends ItemBase
{
    protected ref array<ref LootEntry> m_LootTable;

    void GiftBox()
    {
        // Initialize loot table
        m_LootTable = new array<ref LootEntry>;

        m_LootTable.Insert(new LootEntry("Apple", 50.0)); // 50% chance
        m_LootTable.Insert(new LootEntry("CannedBeans", 30.0)); // 30% chance
        m_LootTable.Insert(new LootEntry("Rifle", 15.0)); // 15% chance
        m_LootTable.Insert(new LootEntry("Magazine_Random", 5.0)); // 5% chance
    }

    override void Open(ItemBase item)
    {
        super.Open(item);

        // Select reward based on weights
        string reward = SelectWeightedLoot();

        // Spawn reward
        EntityAI spawned = GetGame().CreateObject(reward, GetPosition());

        if (spawned)
        {
            // Play spawn sound
            SEffectManager.PlaySound("loot_box_open_SoundSet", GetPosition());
        }

        // Delete box
        GetGame().ObjectDelete(this);
    }

    string SelectWeightedLoot()
    {
        float totalWeight = 0;

        foreach (LootEntry entry : m_LootTable)
        {
            totalWeight += entry.Weight;
        }

        float random = Math.RandomFloat01() * totalWeight;
        float current = 0;

        foreach (LootEntry entry : m_LootTable)
        {
            current += entry.Weight;

            if (random <= current)
            {
                return entry.ItemName;
            }
        }

        return m_LootTable.Get(0).ItemName;
    }
}

class LootEntry
{
    string ItemName;
    float Weight;

    void LootEntry(string name, float weight)
    {
        ItemName = name;
        Weight = weight;
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3620127965\addons\Kote_CraftMaterial\Kote_CraftMaterial\scripts\4_World\GiftBox.c:47-136`

### Multiple rewards system

```c
class GiftBox_Multi extends GiftBox
{
    override void Open(ItemBase item)
    {
        super.Open(item);

        // Give 3 random rewards
        for (int i = 0; i < 3; i++)
        {
            string reward = SelectWeightedLoot();
            GetGame().CreateObject(reward, GetPosition());
        }

        // Delete box
        GetGame().ObjectDelete(this);
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3620127965\addons\Kote_CraftMaterial\Kote_CraftMaterial\scripts\4_World\GiftBox.c:139-143`

---

## Specimen RO-9 (3645022168) - Auto-Cleanup Loot Crate

### Static loot crate with cleanup

```c
class SubjectCrate extends Container_Base
{
    protected float m_CleanupTime;

    override void EEInit()
    {
        super.EEInit();

        // Set cleanup time (300 seconds = 5 minutes)
        m_CleanupTime = GetGame().GetTickTime() + 300;
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Check if empty and time expired
        if (GetGame().GetTickTime() > m_CleanupTime)
        {
            if (IsEmpty())
            {
                GetGame().ObjectDelete(this);
            }
        }
    }

    bool IsEmpty()
    {
        // Check if crate has items
        array<EntityAI> items = new array<EntityAI>;
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

        return items.Count() == 0;
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645022168\addons\subject\subject\scripts\4_world\subject.c:92-106`

---

## 💡 Examples

### Example 1: Tiered rarity system

```c
class TieredLootBox extends ItemBase
{
    void SpawnReward()
    {
        float roll = Math.RandomFloat01();

        string reward;

        if (roll < 0.01) // 1% Legendary
        {
            reward = GetLegendaryLoot();
        }
        else if (roll < 0.10) // 9% Epic
        {
            reward = GetEpicLoot();
        }
        else if (roll < 0.30) // 20% Rare
        {
            reward = GetRareLoot();
        }
        else // 70% Common
        {
            reward = GetCommonLoot();
        }

        GetGame().CreateObject(reward, GetPosition());
    }

    string GetLegendaryLoot()
    {
        array<string> loot = {"NVG", "GPS", "ModernOptic", "KashtanOptic"};
        return loot.GetRandomElement();
    }

    string GetEpicLoot()
    {
        array<string> loot = {"SMG_VSS", "Magazine_VSS", "AmmoBox_VSS"};
        return loot.GetRandomElement();
    }

    string GetRareLoot()
    {
        array<string> loot = {"Rifle", "Shotgun", "Magazine_Random"};
        return loot.GetRandomElement();
    }

    string GetCommonLoot()
    {
        array<string> loot = {"Apple", "SodaCan", "Bandage"};
        return loot.GetRandomElement();
    }
}
```

### Example 2: Guaranteed + random rewards

```c
class GuaranteedLootBox extends ItemBase
{
    override void Open(ItemBase item)
    {
        super.Open(item);

        // Guaranteed rewards (always get these)
        GetGame().CreateObject("Bandage", GetPosition());
        GetGame().CreateObject("SodaCan", GetPosition());

        // Plus one random reward
        string randomReward = GetRandomReward();
        GetGame().CreateObject(randomReward, GetPosition());

        GetGame().ObjectDelete(this);
    }

    string GetRandomReward()
    {
        array<string> loot = {
            "Rifle",
            "Magazine",
            "AmmoBox",
            "MedicalKit"
        };

        return loot.GetRandomElement();
    }
}
```

---

## 🔗 Related Topics

→ [Inventory System](INVENTORY_SYSTEM.md) - EnumerateInventory patterns
→ [EntityAI Lifecycle](ENTITYAI_LIFECYCLE.md) - EEInit, OnUpdate
→ [Storage System](STORAGE_SYSTEM.md) - Saving loot state

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ Weighted loot system: GiftBox.c:47-136
- ✅ Multiple rewards: GiftBox.c:139-143
- ✅ Auto-cleanup crate: subject.c:92-106

**Что НЕ проверено:**
- ❌ Работает ли без Material Pack мода
- ❌ Производительность с большим количеством box
- ❌ Совместимость с другими loot системами

---

## 💡 Tips

### Best Practices

1. **Используйте weighted chances** вместо фиксированных процентов
2. **Удаляйте box** после открытия для экономии памяти
3. **Добавьте cleanup** для пустых crate (300 сек recommended)
4. **Используйте SEffectManager** для звуков открытия
5. **Группируйте loot** по rarity tiers

### Rarity Distribution

**Common model:**
- Common: 70% (70 items out of 100)
- Rare: 20% (20 items)
- Epic: 9% (9 items)
- Legendary: 1% (1 item)

**Alternative:**
- Common: 50%
- Uncommon: 30%
- Rare: 15%
- Ultra Rare: 5%

### Performance Tips

- **Ограничьте loot table** (< 50 items)
- **Используйте weights** вместо nested if-else
- **Предварительно вычислите** total weight в EEInit
- **Удаляйте пустые crate** автоматически

### Common Pitfalls

- ❌ **Забыть удалить box** → memory leak
- ❌ **Игнорировать weights** → неправильное распределение
- ❌ **Спаунить слишком много** → performance drop
- ❌ **Не проверять IsServer()** → дублирование

---

*Последнее обновление: 2025-01-16*
*Источники: Material pack Free (3620127965), Specimen RO-9 (3645022168)*
