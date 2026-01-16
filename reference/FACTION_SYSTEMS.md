# Faction Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [AI System](AI_SYSTEM.md) | [NPC Classes](AI_NPC_CLASSES.md)

---

## ⚡ 30-Second Summary

Система фракций с reputation, disease модификаторами и god-tier elite вариациями.

---

## 🎯 5-Minute Solution: Базовая структура фракции

### eAIFactionBase inheritance

```c
class eAIFactionCustom : eAIFactionBase
{
    override float GetDamageMultiplier()
    {
        return 2.0;  // 2x урон
    }

    override bool HasUnlimitedStamina()
    {
        return true;
    }

    override string GetDiseaseModifier()
    {
        return "brain";  // Модификатор болезни
    }
}
```

### Faction с reputation

```c
class eAIFactionCustom : eAIFactionBase
{
    override array<string> GetFriendlyFactions()
    {
        return {"eAIFactionBlu", "eAIFactionVerde"};
    }

    override array<string> GetEnemyFactions()
    {
        return {"eAIFactionHive"};
    }
}
```

---

## 📚 Deep Dive

## TheHive (3572908305) - Faction System с Diseases

### 5 Factions
1. **Hive** - Паразиты (brain disease, 2x damage)
2. **Blu** - Синие (дружелюбны к Verde, Gialli, Rossi)
3. **Gialli** - Жёлтые (NBC variant с poisoning)
4. **Verde** - Зелёные
5. **Rossi** - Красные

### Disease modifiers

```c
override string GetDiseaseModifier()
{
    return "brain";  // или "poisoning", "flu", "cholera"
}
```

### God-tier elite фракции

```c
class eAIFactionHive_Elite : eAIFactionHive
{
    override float GetDamageMultiplier()
    {
        return 5.0;  // God-tier
    }

    override float GetHealthMultiplier()
    {
        return 3.0;
    }

    override bool IsGodTier()
    {
        return true;
    }
}
```

### Reputation system

```c
float GetReputation(string factionID)
{
    return m_Reputation.Get(factionID);  // -100 to +100
}

void SetReputation(string factionID, float value)
{
    m_Reputation.Set(factionID, value);
}

bool IsFriendly(string otherFactionID)
{
    return GetReputation(otherFactionID) > 50.0;
}
```

---

## 💡 Examples

### Example 1: Создать кастомную фракцию

```c
class eAIFactionMyCustom : eAIFactionBase
{
    override float GetDamageMultiplier()
    {
        return 1.5;
    }

    override string GetDiseaseModifier()
    {
        return "flu";
    }

    override array<string> GetFriendlyFactions()
    {
        return {"eAIFactionBlu", "eAIFactionVerde"};
    }
}
```

---

## 🔗 Related Topics

→ [AI System](AI_SYSTEM.md) - AI базовые системы
→ [NPC Classes](AI_NPC_CLASSES.md) - NPC на базе DayZInfected
→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - EEKilled

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (TheHive)

**Что проверено:**
- ✅ 5 фракций: Hive, Blu, Gialli, Verde, Rossi
- ✅ Disease modifiers: m_DiseaseModifier = "brain", "poisoning"
- ✅ God-tier elite: GetDamageMultiplier() = 5.0
- ✅ Reputation system: GetReputation/SetReputation

**Что НЕ проверено:**
- ❌ Полный список disease модификаторов
- ❌ Работает ли без TheHive мода

---

## 💡 Tips

### Best Practices

1. **Faction ID:** Используйте уникальные ID ("eAIFactionMyCustom")
2. **Reputation range:** -100 to +100
3. **Disease modifiers:** Используйте существующие типы
4. **God-tier:** Используйте осторожно

---

*Последнее обновление: 2025-01-16*
*Источник: TheHive (Workshop ID: 3572908305)*
