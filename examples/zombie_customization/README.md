# Zombie Customization Example

**Status:** ✅ Working Example
**Source:** Dutch Bunker (2718893193)
**Complexity:** Simple

---

## ⚡ 30-Second Summary

Custom zombies with increased stats, special abilities, and loot drops.

---

## 🎯 Features

- ✅ **Custom Health** - Set zombie HP
- ✅ **Custom Damage** - Set attack damage
- ✅ **Loot Drops** - Custom loot table
- ✅ **Special Abilities** - Speed, strength

---

## 💡 Key Concepts

### Custom Zombie

```c
class CustomZombie extends ZombieBase
{
    override void Init()
    {
        super.Init();

        // Set health
        SetHealth("GlobalHealth", "Health", 500);
    }

    override float GetDamage()
    {
        return 100; // High damage
    }
}
```

---

## 🔗 Related Topics

→ [Custom NPC](../topics/15_zombie_ai.md) - NPC creation
→ [Zombie Base](../reference/AI_SYSTEM.md) - Full docs

---

*Последнее обновление: 2025-01-18*
