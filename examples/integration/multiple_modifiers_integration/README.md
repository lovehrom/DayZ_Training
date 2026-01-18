# Multiple Modifiers Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** player_modifiers + stacking system

---

## ⚡ 30-Second Summary

Integration example showing multiple modifiers stacking on player. Different sources (items, skills, effects) can add modifiers that stack or override each other.

---

## 🎯 Features

### Integration Mechanics
- ✅ **Multiple Sources** - Items, skills, buffs, debuffs
- ✅ **Stacking Rules** - Additive, multiplicative, override
- ✅ **Priority System** - Higher priority wins
- ✅ **Duration Tracking** - Temporary vs permanent
- ✅ **Modifier Categories** - Health, stamina, speed, damage

---

## 📁 File Structure

```
multiple_modifiers_integration/
├── README.md                          ← This file
├── config.cpp                         ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── StackingModifier.c              ← Stackable modifier
            ├── ModifierManager.c               ← Manage modifiers
            ├── ItemModifiers.c                 ← Item-based modifiers
            ├── SkillModifiers.c                ← Skill-based modifiers
            └── PlayerBase/
                └── playerbase_modifiers.c      ← Integration
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- `player_modifiers/` - Single modifier effects
- Stacking - Combine multiple modifiers

**Integration:**
1. Multiple sources add modifiers → Manager tracks
2. Calculate final value → Apply stacking rules
3. Update player stats → Apply final effect
4. Remove expired → Cleanup

### Modifier Categories

| Category | Stacking | Example |
|----------|----------|---------|
| Health | Additive | +10 HP from item, +20 HP from skill = +30 HP |
| Stamina | Multiplicative | 1.2x from item, 1.5x from skill = 1.8x total |
| Speed | Max priority | 1.5x (item), 2.0x (skill) = 2.0x (max wins) |

---

## 💡 Implementation

### 1. StackingModifier.c

Stackable modifier:

```c
class StackingModifier
{
    string ID;
    string Name;
    string Category;              // "Health", "Stamina", "Speed", "Damage"
    float Value;
    int Priority;                 // Higher wins (for override)
    float Duration;               // 0 = permanent, >0 = seconds
    float RemainingTime;
    string Source;                // "Item", "Skill", "Buff", "Debuff"

    void StackingModifier(string id, string name, string category, float value, int priority, float duration, string source)
    {
        ID = id;
        Name = name;
        Category = category;
        Value = value;
        Priority = priority;
        Duration = duration;
        RemainingTime = duration;
        Source = source;
    }

    // Check if expired
    bool IsExpired()
    {
        return Duration > 0 && RemainingTime <= 0;
    }

    // Update remaining time
    void Update(float deltaTime)
    {
        if (Duration > 0)
        {
            RemainingTime -= deltaTime;

            if (RemainingTime < 0)
                RemainingTime = 0;
        }
    }

    // Get time remaining
    float GetTimeRemaining()
    {
        return RemainingTime;
    }

    // Is permanent
    bool IsPermanent()
    {
        return Duration == 0;
    }
}
```

### 2. ModifierManager.c

Manage stacking modifiers:

```c
class ModifierManager
{
    protected ref array<ref StackingModifier> m_Modifiers;
    protected PlayerBase m_Player;

    void ModifierManager(PlayerBase player)
    {
        m_Player = player;
        m_Modifiers = new array<ref StackingModifier>;
    }

    // Add modifier
    void AddModifier(StackingModifier modifier)
    {
        // Check if already exists
        StackingModifier existing = GetModifier(modifier.ID);

        if (existing)
        {
            // Update existing
            existing.Value = modifier.Value;
            existing.Priority = modifier.Priority;
            existing.RemainingTime = modifier.Duration;

            Print("[Modifier] Updated " + modifier.ID);
        }
        else
        {
            // Add new
            m_Modifiers.Insert(modifier);

            Print("[Modifier] Added " + modifier.ID);
        }

        // Recalculate
        RecalculateModifiers();
    }

    // Remove modifier
    void RemoveModifier(string id)
    {
        for (int i = m_Modifiers.Count() - 1; i >= 0; i--)
        {
            if (m_Modifiers[i].ID == id)
            {
                m_Modifiers.Remove(i);

                Print("[Modifier] Removed " + id);

                // Recalculate
                RecalculateModifiers();

                return;
            }
        }
    }

    // Get modifier by ID
    StackingModifier GetModifier(string id)
    {
        foreach (StackingModifier modifier : m_Modifiers)
        {
            if (modifier.ID == id)
                return modifier;
        }

        return null;
    }

    // Update modifiers (call every frame)
    void Update(float deltaTime)
    {
        // Update durations
        foreach (StackingModifier modifier : m_Modifiers)
        {
            modifier.Update(deltaTime);
        }

        // Remove expired
        for (int i = m_Modifiers.Count() - 1; i >= 0; i--)
        {
            if (m_Modifiers[i].IsExpired())
            {
                Print("[Modifier] Expired " + m_Modifiers[i].ID);
                m_Modifiers.Remove(i);
            }
        }

        // Recalculate
        RecalculateModifiers();
    }

    // Recalculate final values
    void RecalculateModifiers()
    {
        // Group by category
        map<string, ref array<ref StackingModifier>> categoryMap;
        categoryMap = new map<string, ref array<ref StackingModifier>>;

        foreach (StackingModifier modifier : m_Modifiers)
        {
            if (!categoryMap.Contains(modifier.Category))
            {
                categoryMap.Set(modifier.Category, new array<ref StackingModifier>);
            }

            categoryMap.Get(modifier.Category).Insert(modifier);
        }

        // Calculate final value for each category
        foreach (string category, array<ref StackingModifier> modifiers : categoryMap)
        {
            float finalValue = CalculateCategoryValue(category, modifiers);
            ApplyModifier(category, finalValue);
        }
    }

    // Calculate final value for category
    float CalculateCategoryValue(string category, array<ref StackingModifier> modifiers)
    {
        if (modifiers.Count() == 0)
            return 0;

        // Different stacking rules per category
        switch (category)
        {
            case "Health":
                return CalculateAdditive(modifiers);

            case "Stamina":
                return CalculateMultiplicative(modifiers);

            case "Speed":
                return CalculateMaxPriority(modifiers);

            case "Damage":
                return CalculateAdditive(modifiers);

            default:
                return 0;
        }
    }

    // Additive stacking
    float CalculateAdditive(array<ref StackingModifier> modifiers)
    {
        float total = 0;

        foreach (StackingModifier modifier : modifiers)
        {
            total += modifier.Value;
        }

        return total;
    }

    // Multiplicative stacking
    float CalculateMultiplicative(array<ref StackingModifier> modifiers)
    {
        float total = 1.0;

        foreach (StackingModifier modifier : modifiers)
        {
            total *= modifier.Value;
        }

        return total;
    }

    // Max priority (override)
    float CalculateMaxPriority(array<ref StackingModifier> modifiers)
    {
        StackingModifier highest = modifiers[0];

        foreach (StackingModifier modifier : modifiers)
        {
            if (modifier.Priority > highest.Priority)
                highest = modifier;
        }

        return highest.Value;
    }

    // Apply modifier to player
    void ApplyModifier(string category, float value)
    {
        switch (category)
        {
            case "Health":
                // Max health bonus
                m_Player.GetStatHealth().Add(value);
                break;

            case "Stamina":
                // Stamina regeneration multiplier
                // (apply in OnUpdate)
                break;

            case "Speed":
                // Movement speed multiplier
                // (apply in OnUpdate)
                break;

            case "Damage":
                // Damage bonus
                // (apply in attack)
                break;
        }
    }

    // Get all modifiers
    array<ref StackingModifier> GetAllModifiers()
    {
        return m_Modifiers;
    }
}
```

### 3. ItemModifiers.c

Item-based modifiers:

```c
class ItemModifierSystem
{
    // Add modifier from equipped item
    static void OnItemEquipped(PlayerBase player, ItemBase item)
    {
        string itemType = item.GetType();

        // Redbull gives stamina boost
        if (itemType ==("Redbull"))
        {
            StackingModifier modifier = new StackingModifier(
                "redbull_stamina",
                "Redbull Stamina",
                "Stamina",
                1.5, // 50% more stamina regen
                1, // Priority
                600, // 10 minutes
                "Item"
            );

            player.GetModifierManager().AddModifier(modifier);

            NotificationSystem.Notify(player, "Redbull stamina boost activated!");
        }

        // Night vision goggles give vision boost
        if (itemType == "NVGHeadstrap")
        {
            StackingModifier modifier = new StackingModifier(
                "nvg_vision",
                "Night Vision",
                "Vision",
                1.0,
                10, // High priority
                0, // Permanent while equipped
                "Item"
            );

            player.GetModifierManager().AddModifier(modifier);
        }
    }

    // Remove modifier when item unequipped
    static void OnItemUnequipped(PlayerBase player, ItemBase item)
    {
        string itemType = item.GetType();

        if (itemType == "Redbull")
        {
            player.GetModifierManager().RemoveModifier("redbull_stamina");
        }

        if (itemType == "NVGHeadstrap")
        {
            player.GetModifierManager().RemoveModifier("nvg_vision");
        }
    }
}
```

### 4. SkillModifiers.c

Skill-based modifiers:

```c
class SkillModifierSystem
{
    // Add modifier from skill level
    static void OnSkillLevelUp(PlayerBase player, string skillName, int level)
    {
        // Sprint skill gives speed boost
        if (skillName == "Sprint")
        {
            float speedBonus = 1.0 + (level * 0.1); // 10% per level

            StackingModifier modifier = new StackingModifier(
                "sprint_speed",
                "Sprint Speed",
                "Speed",
                speedBonus,
                5, // Medium priority
                0, // Permanent
                "Skill"
            );

            player.GetModifierManager().AddModifier(modifier);

            NotificationSystem.Notify(player, "Sprint speed increased to " + (speedBonus * 100) + "%");
        }

        // Vitality skill gives health boost
        if (skillName == "Vitality")
        {
            float healthBonus = level * 10; // 10 HP per level

            StackingModifier modifier = new StackingModifier(
                "vitality_health",
                "Vitality Health",
                "Health",
                healthBonus,
                5,
                0,
                "Skill"
            );

            player.GetModifierManager().AddModifier(modifier);

            NotificationSystem.Notify(player, "Health increased by " + healthBonus);
        }
    }
}
```

### 5. PlayerBase Integration

```c
modded class PlayerBase
{
    protected ref ModifierManager m_ModifierManager;

    override void OnConnect()
    {
        super.OnConnect();

        m_ModifierManager = new ModifierManager(this);
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Update modifiers
        if (m_ModifierManager)
        {
            m_ModifierManager.Update(timeslice);
        }
    }

    // Get modifier manager
    ModifierManager GetModifierManager()
    {
        return m_ModifierManager;
    }

    // Item equipped
    override void OnItemInHandsChanged(EntityAI old_item, EntityAI new_item)
    {
        super.OnItemInHandsChanged(old_item, new_item);

        // Remove old modifier
        if (old_item)
        {
            ItemModifierSystem.OnItemUnequipped(this, ItemBase.Cast(old_item));
        }

        // Add new modifier
        if (new_item)
        {
            ItemModifierSystem.OnItemEquipped(this, ItemBase.Cast(new_item));
        }
    }

    // Skill level up
    void OnSkillLevelUp(string skillName, int level)
    {
        SkillModifierSystem.OnSkillLevelUp(this, skillName, level);
    }

    // Get speed multiplier (for movement)
    float GetSpeedMultiplier()
    {
        // Calculate from modifiers
        array<ref StackingModifier> modifiers = m_ModifierManager.GetAllModifiers();

        float speedMult = 1.0;
        foreach (StackingModifier modifier : modifiers)
        {
            if (modifier.Category == "Speed")
            {
                speedMult = modifier.Value; // Max priority already applied
            }
        }

        return speedMult;
    }

    // Chat command: /modifiers
    bool ChatCommand_Modifiers()
    {
        array<ref StackingModifier> modifiers = m_ModifierManager.GetAllModifiers();

        NotificationSystem.Notify(this, "=== Active Modifiers ===");

        foreach (StackingModifier modifier : modifiers)
        {
            string timeStr = modifier.IsPermanent() ? "Permanent" : "" + modifier.GetTimeRemaining() + "s";

            NotificationSystem.Notify(this, modifier.Name + " (" + modifier.Category + "): " + modifier.Value + " [" + timeStr + "]");
        }

        return true;
    }
}
```

---

## 📝 Usage Examples

### Example 1: Temporary Buff

```c
class AdrenalineShot extends ItemBase
{
    override void OnConsume(PlayerBase player)
    {
        super.OnConsume(player);

        // Add temporary buff
        StackingModifier modifier = new StackingModifier(
            "adrenaline_buff",
            "Adrenaline",
            "Speed",
            2.0, // 2x speed
            10, // High priority
            30, // 30 seconds
            "Buff"
        );

        player.GetModifierManager().AddModifier(modifier);

        NotificationSystem.Notify(player, "Adrenaline rush! 2x speed for 30s");
    }
}
```

### Example 2: Environmental Debuff

```c
class EnvironmentalSystem
{
    static void OnEnterColdZone(PlayerBase player)
    {
        // Add cold debuff
        StackingModifier modifier = new StackingModifier(
            "cold_debuff",
            "Freezing",
            "Health",
            -5, // Lose 5 HP
            1,
            0, // Permanent while in zone
            "Environment"
        );

        player.GetModifierManager().AddModifier(modifier);
    }

    static void OnLeaveColdZone(PlayerBase player)
    {
        player.GetModifierManager().RemoveModifier("cold_debuff");
    }
}
```

### Example 3: Combo Effects

```c
class ComboSystem
{
    static void CheckCombo(PlayerBase player)
    {
        ModifierManager manager = player.GetModifierManager();

        // Check if has both redbull and adrenaline
        bool hasRedbull = manager.GetModifier("redbull_stamina") != null;
        bool hasAdrenaline = manager.GetModifier("adrenaline_buff") != null;

        if (hasRedbull && hasAdrenaline)
        {
            // Combo effect!
            StackingModifier combo = new StackingModifier(
                "caffeine_adrenaline_combo",
                "Combo",
                "Speed",
                3.0, // 3x speed!
                100, // Highest priority
                30,
                "Combo"
            );

            manager.AddModifier(combo);

            NotificationSystem.Notify(player, "COMBO! 3x speed!");
        }
    }
}
```

---

## 🔗 Related Examples

→ [Player Modifiers](../player_modifiers/) - Individual modifier mechanics
→ [XP System](../xp_system/) - Skill-based modifiers
→ [Integration: Faction+Teleport](../faction_teleport_integration/) - Another integration

---

## ⚠️ Important Notes

### Stacking Rules
- **Additive:** Health, Damage (sum all values)
- **Multiplicative:** Stamina, Speed (multiply all values)
- **Max Priority:** Special effects (highest wins)

### Modifier Sources
- **Item:** Equipment, consumables
- **Skill:** Level-up bonuses
- **Buff:** Temporary positive effects
- **Debuff:** Temporary negative effects
- **Environment:** Zone-based effects
- **Combo:** Special combinations

### Priority System
- **1-10:** Normal modifiers
- **11-50:** Important effects
- **51-100:** Critical/combo effects
- Higher priority wins in conflicts

---

*Last updated: 2025-01-18*
*Combines: player_modifiers + stacking*
