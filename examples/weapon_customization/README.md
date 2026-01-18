# Weapon Customization Example

**Status:** ✅ Working Example
**Source:** SliverWeapons (3644228096)
**Complexity:** Simple

---

## ⚡ 30-Second Summary

Custom weapon stats (damage, rate of fire, reload speed) with attachment systems.

---

## 🎯 Features

- ✅ **Custom Damage** - Set weapon damage
- ✅ **Fire Rate** - Adjust RPM
- ✅ **Reload Speed** - Faster/slower reload
- ✅ **Attachments** - Custom optics, magazines

---

## 💡 Key Concepts

### Custom Weapon

```c
class CustomWeapon extends Rifle_Base
{
    override float GetDamageSystem()
    {
        return 100; // High damage
    }

    override float GetReloadTime()
    {
        return 2.0; // 2 seconds
    }
}
```

---

## 🔗 Related Topics

→ [Weapon Mods](../reference/WEAPON_MODIFICATIONS.md) - Full docs

---

*Последнее обновление: 2025-01-18*
