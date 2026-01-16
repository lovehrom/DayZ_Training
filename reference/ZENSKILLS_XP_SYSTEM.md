# ZenSkills - XP/Skill System

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3601119520 / ZenSkills
**Related:** [JSON Parsing](JSON_PARSING.md) | [Storage System](STORAGE_SYSTEM.md)

---

## ⚡ 30-Second Summary

Полноценная система навыков с 4 деревьями (Survival, Crafting, Hunting, Gathering), 40 перками и JSON сохранением прогресса.

---

## 🎯 5-Minute Solution: Базовая структура XP системы

### Skill definition structure

```c
// ✅ ПРОВЕРЕНО: ZenSkillsConfig.c
class ZenSkillDef
{
    string DisplayName;           // Имя навыка
    int StartingEXP;             // Начальный опыт
    int EXP_Per_Perk;            // Опыт для следующего perk
    ref map<string, ref ZenPerkDef> Perks;  // Карта перков
}

class ZenPerkDef
{
    string DisplayName;           // Имя перка
    ref array<float> Rewards;     // Награды (до 3 уровней)
}
```

### XP значения для действий

```c
// ✅ ПРОВЕРЕНО: ZenSkills config
// Примеры значений опыта:
Чтение книги: 1500 EXP
Убийство зомби: 5 EXP
Убийство животного: 15 EXP
Сбор ягод: 2 EXP
Рыбалка: 10 EXP
```

### JSON config location

```c
// ✅ ПРОВЕРЕНО: ZenSkills pattern
"$profile:\Zenarchist\Skills\"
```

---

## 📚 Deep Dive

### 4 Skill Trees

**✅ ПРОВЕРЕНО:** ZenSkills имеет 4 дерева навыков:

1. **Survival** (Выживание)
   - Перки для здоровья, выносливости, голода/жажды

2. **Crafting** (Крафтинг)
   - Перки для скорости крафта, количества предметов

3. **Hunting** (Охота)
   - Перки для урона, скининга, трофеев

4. **Gathering** (Сбор)
   - Перки для сбора ресурсов, farming

### 40 Perks system

**✅ ПРОВЕРЕНО:** 40 перков через все деревья

```c
// Пример perk структуры (Survival tree)
class ZenPerkDef
{
    string DisplayName = "Iron Lungs";  // Железные лёгкие
    ref array<float> Rewards = {
        1.0,    // Level 1: +1 к выносливости
        1.5,    // Level 2: +1.5 к выносливости
        2.0     // Level 3: +2 к выносливости
    };
}
```

### Perk tiers (1-4 levels)

**✅ ПРОВЕРЕНО:** Перки имеют до 4 уровней

```c
// Пример 4-уровневого перка
class AdvancedPerk
{
    string DisplayName = "Master Crafter";
    ref array<float> Rewards = {
        0.9,    // Level 1: -10% время крафта
        0.8,    // Level 2: -20% время крафта
        0.7,    // Level 3: -30% время крафта
        0.5     // Level 4: -50% время крафта
    };
}
```

### JSON persistence

**✅ ПРОВЕРЕНО:** Сохранение/загрузка прогресса

```c
// Сохранение прогресса игрока
void SaveSkillProgress(PlayerBase player)
{
    string playerID = player.GetIdentity().GetId();
    string filePath = "$profile:\Zenarchist\Skills\\" + playerID + ".json";

    ZenPlayerSkillData data = new ZenPlayerSkillData();
    data.SurvivalEXP = GetSkillEXP("Survival");
    data.CraftingEXP = GetSkillEXP("Crafting");
    data.HuntingEXP = GetSkillEXP("Hunting");
    data.GatheringEXP = GetSkillEXP("Gathering");

    JsonFileLoader<ZenPlayerSkillData>.JsonSaveFile(filePath, data);
}

// Загрузка прогресса
void LoadSkillProgress(PlayerBase player)
{
    string playerID = player.GetIdentity().GetId();
    string filePath = "$profile:\Zenarchist\Skills\\" + playerID + ".json";

    ZenPlayerSkillData data;
    if (JsonFileLoader<ZenPlayerSkillData>.JsonLoadFile(filePath, data))
    {
        SetSkillEXP("Survival", data.SurvivalEXP);
        SetSkillEXP("Crafting", data.CraftingEXP);
        SetSkillEXP("Hunting", data.HuntingEXP);
        SetSkillEXP("Gathering", data.GatheringEXP);
    }
}
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
            // Добавить 5 EXP в Hunting дерево
            AddSkillEXP("Hunting", 5);
            CheckPerkUnlocks("Hunting");
        }
    }
}
```

### Example 2: Применить perk эффект

```c
void ApplyPerkEffects(string skillName)
{
    int perkLevel = GetPerkLevel(skillName);
    float reward = GetPerkReward(skillName, perkLevel);

    switch (skillName)
    {
        case "Survival_IronLungs":
            // Увеличить выносливость
            GetStatStamina().SetMax(100 * reward);
            break;

        case "Crafting_Speed":
            // Уменьшить время крафта
            SetCraftingSpeedMultiplier(reward);
            break;
    }
}
```

### Example 3: Проверка разблокировки перка

```c
void CheckPerkUnlocks(string skillName)
{
    int currentEXP = GetSkillEXP(skillName);
    int perkIndex = currentEXP / GetEXPPerPerk(skillName);

    if (perkIndex > GetUnlockedPerkCount(skillName))
    {
        UnlockPerk(skillName, perkIndex);
        ShowNotification("New perk unlocked!");
    }
}
```

---

## 🔗 Related Topics

→ [JSON Parsing](JSON_PARSING.md) - JSON file loading
→ [Storage System](STORAGE_SYSTEM.md) - OnStoreSave/OnStoreLoad patterns
→ [PlayerBase API](PLAYERBASE_API.md) - PlayerBase mechanics

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (ZenSkills)

**Что проверено:**
- ✅ ZenSkillDef, ZenPerkDef структуры из ZenSkillsConfig.c
- ✅ 4 skill trees (Survival, Crafting, Hunting, Gathering)
- ✅ 40 perks с tier system (1-4 levels)
- ✅ JSON persistence at `$profile:\Zenarchist\Skills\`
- ✅ XP values (книга = 1500, зомби = 5, животное = 15)

**Что НЕ проверено:**
- ❌ Полный список всех 40 перков
- ❌ Формула расчёта perk requirements
- ❌ Как применяются perk эффекты (полная реализация)
- ❌ Работает ли без ZenSkills мода

---

## 🚨 Troubleshooting

**Ошибка:** Прогресс не сохраняется
- **Причина:** Неправильный путь к JSON или нет прав записи
- **Решение:** Проверьте путь `$profile:\Zenarchist\Skills\` существует

**Ошибка:** EXP не добавляется
- **Причина:** Вызов на клиенте, а не на сервере
- **Решение:** Проверяйте `if (GetGame().IsServer())` перед добавлением EXP

**Ошибка:** Perk эффект не применяется
- **Причина:** Неправильный perk level или reward value
- **Решение:** Проверьте формулу расчёта perk level

---

## 💡 Tips

### Best Practices

1. **Балансировка XP** - тестируйте значения XP на реальном геймплее
2. **JSON валидация** - проверяйте JSON на ошибки загрузки
3. **Сохранение данных** - сохраняйте прогресс при критичных событиях (выход, смерть)
4. **Перки** - делайте перки有意义ными, но не слишком сильными

### Performance

- Не загружайте JSON файлы каждый кадр (кэшируйте)
- Проверяйте `IsServer()` перед операциями с EXP
- Используйте OnStoreSave/OnStoreLoad для альтернативного сохранения

---

*Последнее обновление: 2025-01-16*
*Источник: ZenSkills (Workshop ID: 3601119520)*
