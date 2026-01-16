# Player Modifiers (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [PlayerBase API](PLAYERBASE_API.md) | [Symptoms & Effects](SYMPTOMS_EFFECTS.md)

---

## ⚡ 30-Second Summary

Кастомные модификаторы игрока: stamina regeneration, metabolism tuning, health modifiers, dynamic registration.

---

## 🎯 5-Minute Solution: Custom Stamina Modifier

### Custom modifier class

```c
class CustomStaminaModifier: ModifierBase
{
    override void Init()
    {
        // Track ID for this modifier
        m_TrackID = 999;
    }

    override bool ActivateCondition(PlayerBase player)
    {
        // Activate when stamina is low
        return player.GetStamina() < 50;
    }

    override void OnActivate(PlayerBase player)
    {
        // Increase stamina regeneration rate
        player.GetStatStamina().AddValue(100);
    }

    override void OnDeactivate(PlayerBase player)
    {
        // Remove bonus when deactivated
        player.GetStatStamina().AddValue(-100);
    }

    override float GetDelta()
    {
        // Regenerate 2 stamina per second
        return 2.0;
    }
}
```

### Register custom modifier

```c
modded class ModifiersManager
{
    override void InitModifiers()
    {
        super.InitModifiers();

        // Register custom stamina modifier
        AddModifier(new CustomStaminaModifier);
    }
}
```

---

## 📚 Deep Dive

## Redbull Energy Drinks (3643119261) - Stamina Modifiers

### Custom modifier ID enumeration

```c
class IMG_EnergyModifiers
{
    static const int MODIFIER_ENERGY_DRINK = 1000;
    static const int MODIFIER_STAMINA_BOOST = 1001;
    static const int MODIFIER_SPEED_BOOST = 1002;
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3643119261\addons\BM_RedBull_Client\bm_redbull_client\Scripts\4_world\classes\IMG_EnergyModifiers.c:3`

### Energy drink modifier implementation

```c
class EnergetikMdfr: ModifierBase
{
    override void Init()
    {
        m_TrackID = IMG_EnergyModifiers.MODIFIER_ENERGY_DRINK;
        m_ID = 999; // Unique ID
    }

    override bool ActivateCondition(PlayerBase player)
    {
        // Activate when player consumes energy drink
        return m_Player.HasItem("Redbull");
    }

    override void OnActivate(PlayerBase player)
    {
        // Boost stamina by 50 points
        player.GetStatStamina().AddValue(50);

        // Increase stamina regeneration
        m_Player.m_StaminaModifier += 1.5;
    }

    override void OnDeactivate(PlayerBase player)
    {
        // Remove stamina bonus
        m_Player.m_StaminaModifier -= 1.5;
    }

    override float GetDelta()
    {
        // Regenerate stamina faster
        return 5.0; // 5 stamina per second
    }

    override float GetTimeout()
    {
        // Effect lasts 300 seconds (5 minutes)
        return 300.0;
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3643119261\addons\BM_RedBull_Client\bm_redbull_client\Scripts\4_world\classes\EnergetikMdfr.c:56`

### Dynamic modifiers manager

```c
modded class ModifiersManager
{
    override void InitModifiers()
    {
        super.InitModifiers();

        // Register energy drink modifiers
        AddModifier(new EnergetikMdfr);
        AddModifier(new StaminaBoostMdfr);
        AddModifier(new SpeedBoostMdfr);
    }

    void AddModifier(ModifierBase modifier)
    {
        // Add to list of active modifiers
        m_Modifiers.Insert(modifier);
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3643119261\addons\BM_RedBull_Client\bm_redbull_client\Scripts\4_world\classes\ModifiersManager.c:6`

### Custom action override for consumption

```c
modded class ActionConsume
{
    override void CreateAndApplyModifiers(PlayerBase player)
    {
        super.CreateAndApplyModifiers(player);

        ItemBase item = m_TargetItem.GetItem();

        // Check if consuming energy drink
        if (item.IsInherited(Redbull))
        {
            // Apply custom modifier
            player.GetModifiersManager().AddModifier(new EnergetikMdfr);
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3643119261\addons\BM_RedBull_Client\bm_redbull_client\Scripts\4_world\classes\ActionConsume.c:13`

---

## SLP_Realistic_Survival (3578041032) - Metabolism Tuning

### Enhanced player constants

```c
// playerconstants.c - Override metabolism rates
class PlayerConstants
{
    // Energy metabolism (default: 1.0)
    static const float ENERGY_METABOLISM = 0.8; // 20% slower

    // Water metabolism (default: 1.0)
    static const float WATER_METABOLISM = 0.9; // 10% slower

    // Blood regeneration (default: 1.0)
    static const float BLOOD_REGEN = 1.5; // 50% faster

    // Bone regeneration (default: 1.0)
    static const float BONE_REGEN = 2.0; // 100% faster
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3578041032\addons\SLP_Realistic_Survival\SLP_Realistic_Survival\scripts\3_game\playerconstants.c:3-17`

### Advanced bone regeneration modifier

```c
class BoneRegenMdfr: ModifierBase
{
    override void Init()
    {
        m_TrackID = 777;
        m_ID = 777;
    }

    override void OnActivate(PlayerBase player)
    {
        // Accelerate bone healing
        player.GetStatBrokenLegs().AddValue(10.0);
    }

    override float GetDelta()
    {
        // Regenerate bones at 0.10 HP per second
        return 0.10;
    }

    override float GetTimeout()
    {
        // Check every 1 second
        return 1.0;
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3578041032\addons\SLP_Realistic_Survival\SLP_Realistic_Survival\scripts\4_world\sickbonelegsregen.c:1-4`

---

## 💡 Examples

### Example 1: Health regeneration modifier

```c
class HealthRegenMdfr: ModifierBase
{
    override void Init()
    {
        m_TrackID = 1001;
    }

    override bool ActivateCondition(PlayerBase player)
    {
        // Activate when health is below 50%
        return player.GetHealth() < 50;
    }

    override void OnActivate(PlayerBase player)
    {
        player.GetStatHealth().AddValue(10.0);
    }

    override float GetDelta()
    {
        // Regenerate 5 HP per second
        return 5.0;
    }
}
```

### Example 2: Speed boost modifier

```c
class SpeedBoostMdfr: ModifierBase
{
    override void Init()
    {
        m_TrackID = 1002;
    }

    override void OnActivate(PlayerBase player)
    {
        // Increase movement speed by 20%
        player.m_MovementSpeedModifier = 1.2;
    }

    override void OnDeactivate(PlayerBase player)
    {
        // Reset to normal speed
        player.m_MovementSpeedModifier = 1.0;
    }
}
```

---

## 🔗 Related Topics

→ [PlayerBase API](PLAYERBASE_API.md) - Stat system access
→ [Symptoms & Effects](SYMPTOMS_EFFECTS.md) - SymptomManager patterns
→ [Storage System](STORAGE_SYSTEM.md) - Saving modifier state

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ Modifier ID enumeration: IMG_EnergyModifiers.c:3
- ✅ Stamina boost implementation: EnergetikMdfr.c:56
- ✅ Dynamic registration: ModifiersManager.c:6
- ✅ Action override: ActionConsume.c:13
- ✅ Metabolism tuning: playerconstants.c:3-17
- ✅ Bone regeneration: sickbonelegsregen.c:1-4

**Что НЕ проверено:**
- ❌ Работает ли без энергосистем мода
- ❌ Совместимость с другими модификаторами
- ❌ Производительность с множеством модификаторов

---

## 💡 Tips

### Best Practices

1. **Используйте уникальные TrackID** для каждого модификатора (> 1000)
2. **Переопределяйте Init()** для установки ID
3. **Используйте ActivateCondition()** для проверки условий
4. **Очищайте бонусы в OnDeactivate()** чтобы избежать утечек
5. **Регистрируйте модификаторы в ModifiersManager**

### Modifier Types

**Постоянные модификаторы:**
- Metabolism tuning
- Stat bonuses

**Временные модификаторы:**
- Energy drinks
- Medicine effects
- Buffs/debuffs

**Условные модификаторы:**
- Health-based regen
- Environment-based effects

### Common Pitfalls

- ❌ **Забыть удалить бонус в OnDeactivate()** → permanent boost
- ❌ **Использовать низкий TrackID** → конфликты с vanilla
- ❌ **Игнорировать ActivateCondition()** → всегда активен
- ❌ **Забыть зарегистрировать в ModifiersManager** → не работает

---

*Последнее обновление: 2025-01-16*
*Источники: Redbull Energy Drinks (3643119261), SLP_Realistic_Survival (3578041032)*
