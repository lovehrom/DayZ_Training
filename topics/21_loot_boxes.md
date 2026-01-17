# Loot Box Systems

**Status:** ⚠️ Mod-derived
**Related:** [Storage](14_storage.md) | [Inventory](../reference/INVENTORY_SYSTEM.md)

---

## ⚡ 30-Second Summary

Loot boxes: случайные награды, weighted chances, rarity tiers, auto-cleanup пустых crate.

---

## 🎯 5-Minute Solution

### Weighted random

```c
class LootBox extends ItemBase
{
    override void Open(ItemBase item)
    {
        super.Open(item);

        float roll = Math.RandomFloat01();

        string reward;
        if (roll < 0.01) // 1%
            reward = GetLegendaryLoot();
        else if (roll < 0.10) // 9%
            reward = GetEpicLoot();
        else // 90%
            reward = GetCommonLoot();

        GetGame().CreateObject(reward, GetPosition());
        GetGame().ObjectDelete(this);
    }
}
```

---

## 💡 Examples

### Пример 1: Guaranteed + random

```c
void OpenBox()
{
    // Guaranteed
    GetGame().CreateObject("Bandage", GetPosition());

    // Random
    string loot = GetRandomLoot();
    GetGame().CreateObject(loot, GetPosition());
}
```

### Пример 2: Multiple rewards

```c
void OpenBox()
{
    for (int i = 0; i < 3; i++)
    {
        string loot = GetRandomLoot();
        GetGame().CreateObject(loot, GetPosition());
    }
}
```

---

## 🔗 Related Topics

→ [Storage](14_storage.md) - Recursive save
→ [Reference: Loot Boxes](../reference/LOOT_BOX_SYSTEMS.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
*Источники: Material Pack (3620127965), Specimen RO-9 (3645022168)*
