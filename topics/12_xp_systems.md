# XP & Skill Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Advanced Modding](11_advanced_modding.md) | [Storage](14_storage.md)

---

## ⚡ 30-Second Summary

XP системы позволяют создавать RPG mechanics: опыт за действия, perk trees, уровень прокачки. Полная документация: [XP_SKILL_SYSTEMS.md](../reference/XP_SKILL_SYSTEMS.md)

---

## 🎯 5-Minute Solution: Простая XP система

### Step 1: Создайте класс навыка

```c
class SimpleSkill
{
    string Name;           // Название навыка
    int CurrentXP;         // Текущий опыт
    int Level;             // Уровень

    void AddXP(int amount)
    {
        CurrentXP += amount;
        CheckLevelUp();
    }

    void CheckLevelUp()
    {
        int requiredXP = Level * 1000; // 1000 XP per level

        if (CurrentXP >= requiredXP)
        {
            CurrentXP -= requiredXP;
            Level++;
            OnLevelUp();
        }
    }

    void OnLevelUp()
    {
        // Выдаем бонус за уровень
        Print("Skill " + Name + " leveled up to " + Level);
    }
}
```

### Step 2: Создайте менеджер навыков

```c
class SkillManager
{
    protected ref map<string, ref SimpleSkill> m_Skills = new map<string, ref SimpleSkill>;
    protected PlayerBase m_Player;

    void SkillManager(PlayerBase player)
    {
        m_Player = player;

        // Инициализируем навыки
        m_Skills.Insert("Survival", new SimpleSkill("Survival"));
        m_Skills.Insert("Crafting", new SimpleSkill("Crafting"));
        m_Skills.Insert("Combat", new SimpleSkill("Combat"));
    }

    void AddXP(string skillName, int amount)
    {
        SimpleSkill skill = m_Skills.Get(skillName);
        if (skill)
        {
            skill.AddXP(amount);
            SaveSkills();
        }
    }

    void SaveSkills()
    {
        // Сохраняем в JSON
        string filePath = "$profile:skills_" + m_Player.GetIdentity().GetId() + ".json";
        JsonFileLoader<map<string, ref SimpleSkill>>::JsonSaveFile(filePath, m_Skills);
    }

    void LoadSkills()
    {
        string filePath = "$profile:skills_" + m_Player.GetIdentity().GetId() + ".json";
        if (FileExist(filePath))
        {
            JsonFileLoader<map<string, ref SimpleSkill>>::JsonLoadFile(filePath, m_Skills);
        }
    }
}
```

### Step 3: Интегрируйте в PlayerBase

```c
modded class PlayerBase
{
    protected SkillManager m_SkillManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Создаем менеджер навыков
        m_SkillManager = new SkillManager(this);
        m_SkillManager.LoadSkills();
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        if (m_SkillManager)
        {
            m_SkillManager.SaveSkills();
        }
    }

    // Даем XP за убийство зомби
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        if (killer && killer.IsMan())
        {
            Man man = killer.CastToMan();
            if (man && man.IsPlayer())
            {
                PlayerBase player = PlayerBase.Cast(man);
                if (player && player.m_SkillManager)
                {
                    player.m_SkillManager.AddXP("Combat", 10); // 10 XP за убийство
                }
            }
        }
    }
}
```

---

## 📚 Deep Dive

### Advanced: Perk Trees (как в ZenSkills)

**ZenSkills** реализует полную perk систему:
- 4 skill trees (Survival, Crafting, Hunting, Gathering)
- 40 perks с tier system (1-4)
- XP values для действий

**Пример perk structure:**
```c
class ZenPerkDef
{
    string DisplayName;
    int Tier;                    // 1-4
    int RequiredLevel;           // Требуемый уровень
    float BonusValue;            // Значение бонуса

    // Примеры perks:
    // Tier 1: +10% скорость крафтинга
    // Tier 2: +20% скорость крафтинга
    // Tier 3: +30% скорость + шанс двойного крафта
    // Tier 4: +50% скорость + 25% шанс тройного крафта
}
```

**Полная документация:** [XP_SKILL_SYSTEMS.md](../reference/XP_SKILL_SYSTEMS.md)

---

## 💡 Examples

### Пример 1: XP за действия

```c
// XP за чтение книги
override void ReadItem(ItemBase book)
{
    super.ReadItem(book);

    if (m_SkillManager)
    {
        m_SkillManager.AddXP("Survival", 1500); // 1500 XP
    }
}

// XP за крафтинг
override void CraftItem(ItemBase item)
{
    super.CraftItem(item);

    if (m_SkillManager)
    {
        m_SkillManager.AddXP("Crafting", 500); // 500 XP
    }
}
```

### Пример 2: Бонусы за уровень

```c
float GetCraftingSpeed()
{
    SimpleSkill crafting = m_Skills.Get("Crafting");

    if (!crafting)
        return 1.0;

    // +10% скорости за уровень
    return 1.0 + (crafting.Level * 0.1);
}
```

### Пример 3: Multi-skill bonuses

```c
// Бонус требует несколько навыков
bool CanUnlockAdvancedCrafting()
{
    SimpleSkill crafting = m_Skills.Get("Crafting");
    SimpleSkill survival = m_Skills.Get("Survival");

    return crafting.Level >= 5 && survival.Level >= 3;
}
```

---

## 🔗 Related Topics

→ [Storage](14_storage.md) - Сохранение навыков
→ [Advanced Modding](11_advanced_modding.md) - Обзор mechanics
→ [JSON Parsing](../reference/JSON_PARSING.md) - Работа с JSON

---

## ⚠️ SOURCE VERIFICATION

**Status:** ⚠️ Mod-derived

**Источник:** ZenSkills (3601119520)

**Что проверено:**
- ✅ XP system работает
- ✅ JSON persistence работает
- ✅ Perk trees работают

**Что НЕ проверено:**
- ❌ Работает ли без ZenSkills мода
- ❌ Производительность с большим количеством игроков

---

## 💡 Tips

### Best Practices:

1. **Используйте JSON persistence** для сохранения навыков
2. **Давайте адекватные значения XP** (не слишком много/мало)
3. **Балансируйте perk requirements** (не делайте слишком сложным)
4. **Тестируйте с большим количеством игроков**

### XP Values Reference:

**Обычные действия:**
- Убийство зомби: 5-10 XP
- Чтение книги: 1000-1500 XP
- Крафтинг предмета: 100-500 XP
- Сбор ресурсов: 10-50 XP

**Редкие действия:**
- Убийство босса: 100-500 XP
- Завершение квеста: 500-1000 XP
- Открытие достижение: 200-1000 XP

### Common Pitfalls:

- ❌ **Слишком быстрый leveling** → игроки быстро достигают cap
- ❌ **Слишком медленный** → игроки теряют интерес
- ❌ **Забыть сохранить** → потеря прогресса
- ❌ **Не сбалансировать perks** → imbalance

---

*Последнее обновление: 2025-01-16*
*Источник: ZenSkills (3601119520)*
