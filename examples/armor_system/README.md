# Armor System Example

**Status:** ✅ Working Example
**Source:** Striker Clothes (3645146626)
**Complexity:** Simple

---

## ⚡ 30-Second Summary

Custom armor with damage reduction, bleeding protection, and condition system.

---

## 🎯 Features

- ✅ **Damage Reduction** - Reduce incoming damage
- ✅ **Bleeding Protection** - Prevent bleeding
- ✅ **Condition System** - Armor degrades
- ✅ **Protection Levels** - Low/med/high

---

## 💡 Key Concepts

### Custom Armor

```c
class CustomArmor extends Clothing
{
    override float GetDamageProtection()
    {
        return 0.5; // 50% damage reduction
    }

    override bool CanReceiveBleeding()
    {
        return false; // Prevent bleeding
    }
}
```

---

## 🔗 Related Topics

→ [Armor Systems](../reference/ARMOR_DAMAGE_SYSTEMS.md) - Full docs

---

*Последнее обновление: 2025-01-18*
