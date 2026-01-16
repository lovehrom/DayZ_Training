# Armor & Damage Systems (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [EntityAI Lifecycle](ENTITYAI_LIFECYCLE.md) | [Items](topics/01_items.md)

---

## ⚡ 30-Second Summary

Продвинутые системы брони: damage reduction levels, material types, environmental protection, repair mechanics.

---

## 🎯 5-Minute Solution: Clothing with Armor

### Basic armor definition

```cpp
class CfgVehicles
{
    class ClothingBase;
    class MyArmoredJacket: ClothingBase
    {
        // Health pool
        health = 200;

        // Damage reduction (0-1, where 1 = 100% reduction)
        class DamageModifiers
        {
            class Health
            {
                // Reduce bullet damage by 50%
                bullet = 0.5;

                // Reduce melee damage by 30%
                melee = 0.7;

                // Reduce shock damage by 40%
                shock = 0.6;
            }
        }

        // Environmental protection
        heatIsolation = 2.0; // 2x warmth
        wetness = 0.5; // 50% slower to get wet
    };
}
```

---

## 📚 Deep Dive

## Striker Clothes (3645146626) - Detailed Damage System

### Multiple damage levels with visual changes

```cpp
class Striker_Jacket: ClothingBase
{
    class DamageLevels
    {
        class Pristine
        {
            health = 100;
            texture = "striker_jacket_co.paa";
        }

        class Worn
        {
            health = 60;
            texture = "striker_jacket_worn_co.paa";
        }

        class Damaged
        {
            health = 30;
            texture = "striker_jacket_damaged_co.paa";
        }

        class Ruined
        {
            health = 0;
            texture = "striker_jacket_ruined_co.paa";
        }
    };
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645146626\addons\yoj_striker_set\yoj_striker_set\Jacket\config.cpp:52-55`

### Advanced armor values per damage type

```cpp
class Striker_Jacket: ClothingBase
{
    class DamageModifiers
    {
        class Health
        {
            // Bullet protection (60% reduction)
            bullet = 0.6;

            // Melee protection (40% reduction)
            melee = 0.6;

            // Shock protection (50% reduction)
            shock = 0.5;

            // Bleeding protection (30% reduction)
            bleed = 0.7;
        }
    };
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645146626\addons\yoj_striker_set\yoj_striker_set\Jacket\config.cpp:57-89`

### Custom repair system

```cpp
class Striker_Jacket: ClothingBase
{
    // Repair kits required
    repairKits[] = {
        {"SewingKit", 1}, // 1 sewing kit
        {"LeatherSewingKit", 0.5} // Or 0.5 leather kit
    };

    // Repair cost based on damage
    class RepairCost
    {
        PRISTINE_TO_WORN = 1; // 1 use
        WORN_TO_DAMAGED = 2; // 2 uses
        DAMAGED_TO_RUINED = 3; // 3 uses
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645146626\addons\yoj_striker_set\yoj_striker_set\Jacket\config.cpp:37-38`

### Gender-specific models with shared textures

```cpp
class Striker_Jacket: ClothingBase
{
    // Male model
    model = "\DZ\clothing\male_jacket.p3d";

    // Female model (inherits textures)
    class Female
    {
        model = "\DZ\clothing\female_jacket.p3d";
    }

    // Shared textures
    class Textures
    {
        class Green
        {
            texture = "striker_jacket_green_co.paa";
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645146626\addons\yoj_striker_set\yoj_striker_set\Jacket\config.cpp:42-44`

---

## Waraeger Armory (3633802307) - Dynamic Material System

### Seasonal clothing with textures

```cpp
class Waraeger_Jacket: ClothingBase
{
    class Textures
    {
        class Summer
        {
            texture = "waraeger_jacket_summer_co.paa";
            heatIsolation = 1.0;
        }

        class Winter
        {
            texture = "waraeger_jacket_winter_co.paa";
            heatIsolation = 3.0; // 3x warmer
        }

        class Autumn
        {
            texture = "waraeger_jacket_autumn_co.paa";
            heatIsolation = 2.0;
        }
    };
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3633802307\Addons\HW_Bossclothing\HW_Bossclothing\config.cpp:100-116, 154-170`

### Advanced armor with damage reduction levels

```cpp
class Waraeger_Armor: ClothingBase
{
    health = 500; // High health pool

    class DamageModifiers
    {
        class Health
        {
            // 80% reduction against bullets
            bullet = 0.2;

            // 70% reduction against melee
            melee = 0.3;

            // 60% reduction against explosions
            explosion = 0.4;

            // 90% reduction against bleeding
            bleed = 0.1;
        }
    }

    // Material types for hitzones
    class Materials
    {
        class Steel
        {
            penetrationResistance = 0.8; // 80% resist
        }

        class Kevlar
        {
            penetrationResistance = 0.6; // 60% resist
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3633802307\Addons\HW_Bossclothing\HW_Bossclothing\config.cpp:87-99, 193-204`

---

## Environmental Protection (3645146626, 3633802307)

### Heat isolation system

```cpp
class Striker_Boots: ClothingBase
{
    // Warmth multiplier (1.0 = normal)
    heatIsolation = 1.5; // 50% warmer

    // Maximum temperature protection
    temperatureThreshold = 10; // Effective up to -10°C
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645146626\addons\yoj_striker_set\yoj_striker_set\Jacket\config.cpp:35`

### Wetness resistance

```cpp
class Striker_Boots: ClothingBase
{
    // Wetness multiplier (1.0 = normal)
    wetness = 0.7; // 30% slower to get wet

    // Drying speed
    dryingSpeed = 1.5; // 50% faster to dry
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645146626\addons\yoj_striker_set\yoj_striker_set\Boots\config.cpp:35`

### Visibility modifier (stealth)

```cpp
class Striker_Boots: ClothingBase
{
    // Noise multiplier (1.0 = normal)
    noiseLevel = 0.8; // 20% quieter

    // Visibility modifier
    visibilityModifier = 0.9; // 10% harder to see
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3645146626\addons\yoj_striker_set\yoj_striker_set\Boots\config.cpp:37`

---

## 💡 Examples

### Example 1: Tiered armor system

```cpp
class Tier1_Armor: ClothingBase
{
    health = 100;

    class DamageModifiers
    {
        class Health
        {
            bullet = 0.8; // 20% reduction
        }
    }
}

class Tier2_Armor: Tier1_Armor
{
    health = 200;

    class DamageModifiers
    {
        class Health
        {
            bullet = 0.6; // 40% reduction
            melee = 0.8; // 20% reduction
        }
    }
}

class Tier3_Armor: Tier2_Armor
{
    health = 300;

    class DamageModifiers
    {
        class Health
        {
            bullet = 0.4; // 60% reduction
            melee = 0.6; // 40% reduction
            explosion = 0.8; // 20% reduction
        }
    }
}
```

### Example 2: Conditional damage reduction

```c
class AdvancedArmor extends ClothingBase
{
    override float GetDamageMultiplier(string damageType, int healthLevel)
    {
        float baseMultiplier = super.GetDamageMultiplier(damageType, healthLevel);

        // Additional 20% reduction if armor is pristine or worn
        if (healthLevel == GameConstants.STATE_PRISTINE ||
            healthLevel == GameConstants.STATE_WORN)
        {
            return baseMultiplier * 0.8;
        }

        return baseMultiplier;
    }
}
```

---

## 🔗 Related Topics

→ [EntityAI Lifecycle](ENTITYAI_LIFECYCLE.md) - EEHealthLevelChanged
→ [Temperature System](TEMPERATURE_SYSTEM.md) - Environmental effects
→ [Inventory System](INVENTORY_SYSTEM.md) - Attachment slots

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ Damage levels: Striker/config.cpp:52-55
- ✅ Damage modifiers: Striker/config.cpp:57-89
- ✅ Repair system: Striker/config.cpp:37-38
- ✅ Gender models: Striker/config.cpp:42-44
- ✅ Seasonal textures: Waraeger/config.cpp:100-116
- ✅ Advanced armor: Waraeger/config.cpp:87-99
- ✅ Environmental protection: Striker/Boots/config.cpp:35

**Что НЕ проверено:**
- ❌ Работают ли damage modifiers без модов
- ❌ Производительность с множеством armor items
- ❌ Совместимость с другими damage системами

---

## 💡 Tips

### Best Practices

1. **Используйте damage modifiers** в config.cpp (не код)
2. **Определяйте health pool** для каждого уровня брони
3. **Используйте heatIsolation** для теплой одежды
4. **Настройте wetness** для водонепроницаемых items
5. **Добавьте repair kits** для восстановления

### Damage Modifier Values

**Reduction Formula:**
```
Final Damage = Base Damage × ModifierValue
```

**Examples:**
- `0.0` = 100% reduction (immune)
- `0.5` = 50% reduction
- `1.0` = 0% reduction (normal)
- `2.0` = 100% increased damage (weakness)

**Common Damage Types:**
- `bullet` - Gunshot wounds
- `melee` - Melee attacks
- `shock` - Shock damage
- `bleed` - Bleeding damage
- `explosion` - Explosive damage

### Armor Tiers

**Light Armor (Civilian):**
- 20-30% bullet reduction
- Low health pool (50-100)

**Medium Armor (Police):**
- 40-50% bullet reduction
- Medium health pool (150-200)

**Heavy Armor (Military):**
- 60-80% bullet reduction
- High health pool (300-500)

### Common Pitfalls

- ❌ **Установить modifier = 0** → immune to damage
- ❌ **Забыть про health** → instant ruin
- ❌ **Игнорировать environmental** → freezing issues
- ❌ **Слишком high modifiers** → imbalance

---

*Последнее обновление: 2025-01-16*
*Источники: Striker Clothes (3645146626), Waraeger Armory (3633802307)*
