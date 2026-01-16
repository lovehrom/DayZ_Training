# XP & Skill Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [JSON Parsing](JSON_PARSING.md) | [Storage System](STORAGE_SYSTEM.md)

---

## ⚡ 30-Second Summary

Полноценные системы опыта и навыков с деревьями перков, JSON сохранением и наградами за действия.

---

## 🎯 5-Minute Solution: Базовая структура XP системы

### Skill definition structure

```c
class ZenSkillDef
{
    string DisplayName;
    int StartingEXP;
    int EXP_Per_Perk;
    ref map<string, ref ZenPerkDef> Perks;
}

class ZenPerkDef
{
    string DisplayName;
    ref array<float> Rewards;
}
```

### XP значения для действий

```c
Чтение книги: 1500 EXP
Убийство зомби: 5 EXP
Убийство животного: 15 EXP
Сбор ягод: 2 EXP
Рыбалка: 10 EXP
```

---

## 📚 Deep Dive

## ZenSkills (3601119520)

### 4 Skill Trees
- **Survival** - Health, stamina, hunger/thirst
- **Crafting** - Crafting speed, quantity bonuses
- **Hunting** - Damage, skinning, trophies
- **Gathering** - Resource gathering, farming

### 40 Perks with tiers (1-4 levels)

```c
class ZenPerkDef
{
    string DisplayName = "Iron Lungs";
    ref array<float> Rewards = {
        1.0,    // Level 1: +1 stamina
        1.5,    // Level 2: +1.5 stamina
        2.0     // Level 3: +2 stamina
    };
}
```

### JSON persistence

```c
string filePath = "$profile:\Zenarchist\Skills\\" + playerID + ".json";
JsonFileLoader<ZenPlayerSkillData>.JsonSaveFile(filePath, data);
```

---

## 💡 Examples

### Example 1: Добавить EXP за действие

```c
modded class PlayerBase
{
    void OnZombieKilled(Object killer)
    {
        super.OnZombieKilled(killer);

        if (GetGame().IsServer())
        {
            AddSkillEXP("Hunting", 5);
            CheckPerkUnlocks("Hunting");
        }
    }
}
```

### Example 2: Применить perk эффект

```c
void ApplyPerkEffects(string skillName, int perkLevel, float reward)
{
    switch (skillName)
    {
        case "Survival_IronLungs":
            GetStatStamina().SetMax(100 * reward);
            break;
        case "Crafting_Speed":
            SetCraftingSpeedMultiplier(reward);
            break;
    }
}
```

---

## 🔗 Related Topics

→ [JSON Parsing](JSON_PARSING.md) - JSON file loading
→ [Storage System](STORAGE_SYSTEM.md) - OnStoreSave/OnStoreLoad
→ [PlayerBase API](PLAYERBASE_API.md) - PlayerBase mechanics

---

*Последнее обновление: 2025-01-16*
*Источник: ZenSkills (Workshop ID: 3601119520)*
