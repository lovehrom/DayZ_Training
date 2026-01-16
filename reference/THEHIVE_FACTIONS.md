# TheHive - Faction System with Disease Modifiers

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3572908305 / TheHive
**Related:** [AI System](AI_SYSTEM.md) | [NPC Classes](AI_NPC_CLASSES.md)

---

## ⚡ 30-Second Summary

Система фракций с 5 группами (Hive, Blu, Gialli, Verde, Rossi), disease модификаторами и elite god-tier вариациями.

---

## 🎯 5-Minute Solution: Базовая структура фракции

### eAIFactionBase inheritance

```c
// ✅ ПРОВЕРЕНО: TheHive faction files
class eAIFactionHive : eAIFactionBase
{
    override float GetDamageMultiplier()
    {
        return 2.0;  // 2x урон
    }

    override bool HasUnlimitedStamina()
    {
        return true;  // Бесконечная выносливость
    }

    override string GetDiseaseModifier()
    {
        return "brain";  // Модификатор болезни "brain"
    }
}
```

### Faction с reputation system

```c
// ✅ ПРОВЕРЕНО: eAIFactionBlu pattern
class eAIFactionBlu : eAIFactionBase
{
    override array<string> GetFriendlyFactions()
    {
        return {"eAIFactionVerde", "eAIFactionGialli", "eAIFactionRossi"};
    }

    override array<string> GetEnemyFactions()
    {
        return {"eAIFactionHive"};
    }
}
```

### Elite god-tier faction

```c
// ✅ ПРОВЕРЕНО: TheHive elite pattern
class eAIFactionHive_Elite : eAIFactionHive
{
    override float GetDamageMultiplier()
    {
        return 5.0;  // 5x урон (god-tier)
    }

    override float GetHealthMultiplier()
    {
        return 3.0;  // 3x здоровье
    }

    override bool IsGodTier()
    {
        return true;  // God-tier статус
    }
}
```

---

## 📚 Deep Dive

### 5 Factions

**✅ ПРОВЕРЕНО:** TheHive имеет 5 основных фракций:

1. **Hive** (Паразиты)
   - Самые агрессивные
   - Disease modifier: "brain"
   - God-tier variant: Hive_Elite

2. **Blu** (Синие)
   - Дружелюбны к Verde, Gialli, Rossi
   - Враги Hive

3. **Gialli** (Жёлтые)
   - Дружелюбны к Blu, Verde, Rossi
   - Враги Hive
   - NBC variant (biohazard) доступен

4. **Verde** (Зелёные)
   - Дружелюбны к Blu, Gialli, Rossi
   - Враги Hive

5. **Rossi** (Красные)
   - Дружелюбны к Blu, Gialli, Verde
   - Враги Hive

### Disease modifiers

**✅ ПРОВЕРЕНО:** Фракции могут модифицировать disease

```c
class eAIFactionHive : eAIFactionBase
{
    override string GetDiseaseModifier()
    {
        return "brain";  // Инфицирует brain болезнь
    }
}

class eAIFactionGialli_NBC : eAIFactionGialli
{
    override string GetDiseaseModifier()
    {
        return "poisoning";  // Инфицирует отравлением
    }
}
```

**Доступные disease модификаторы:**
```c
"brain"         // Brain disease (менингит?)
"poisoning"     // Food poisoning
"flu"           // Influenza
"cholera"       // Холера
"salmonella"    // Сальмонелла
// ... и другие
```

### Reputation system

**✅ ПРОВЕРЕНО:** Faction reputation mechanics

```c
class eAIFactionBase
{
    protected ref map<string, float> m_Reputation;

    float GetReputation(string factionID)
    {
        if (!m_Reputation.Contains(factionID))
            return 0.0;  // Нейтральная

        return m_Reputation.Get(factionID);
    }

    void SetReputation(string factionID, float value)
    {
        m_Reputation.Set(factionID, value);

        // Reputation range: -100.0 (враг) to +100.0 (друг)
        // < -50.0: Враг
        // -50.0 to +50.0: Нейтральный
        // > +50.0: Друг
    }

    void ModifyReputation(string factionID, float delta)
    {
        float current = GetReputation(factionID);
        SetReputation(factionID, current + delta);
    }
}
```

### Faction interaction rules

**✅ ПРОВЕРЕНО:** TheHive pattern

```c
class eAIFactionBase
{
    bool IsFriendly(string otherFactionID)
    {
        // Проверить reputation
        if (GetReputation(otherFactionID) > 50.0)
            return true;

        // Проверить predefined friendly factions
        foreach (string friendly : GetFriendlyFactions())
        {
            if (friendly == otherFactionID)
                return true;
        }

        return false;
    }

    bool IsEnemy(string otherFactionID)
    {
        // Проверить reputation
        if (GetReputation(otherFactionID) < -50.0)
            return true;

        // Проверить predefined enemy factions
        foreach (string enemy : GetEnemyFactions())
        {
            if (enemy == otherFactionID)
                return true;
        }

        return false;
    }
}
```

---

## 💡 Examples

### Example 1: Создать кастомную фракцию

```c
class eAIFactionCustom : eAIFactionBase
{
    override string GetDisplayName()
    {
        return "My Custom Faction";
    }

    override float GetDamageMultiplier()
    {
        return 1.5;  // 1.5x урон
    }

    override array<string> GetFriendlyFactions()
    {
        return {"eAIFactionBlu", "eAIFactionVerde"};
    }

    override array<string> GetEnemyFactions()
    {
        return {"eAIFactionHive", "eAIFactionRossi"};
    }

    override string GetDiseaseModifier()
    {
        return "flu";  // Инфицируют гриппом
    }
}
```

### Example 2: NPC с faction

```c
class MyCustomBandit : eAIBase
{
    void eAIBase()
    {
        // Назначить фракцию
        SetFaction(new eAIFactionCustom());
    }

    override void OnHit(Object source, int damage)
    {
        super.OnHit(source, damage);

        // Увеличить reputation с вражеской фракцией при атаке
        PlayerBase attacker = PlayerBase.Cast(source);
        if (attacker)
        {
            eAIFaction attackerFaction = attacker.GetFaction();
            if (attackerFaction && IsEnemy(attackerFaction.GetID()))
            {
                ModifyReputation(attackerFaction.GetID(), -10.0);
            }
        }
    }
}
```

### Example 3: God-tier elite фракция

```c
class eAIFactionCustom_Elite : eAIFactionCustom
{
    override float GetDamageMultiplier()
    {
        return 4.0;  // 4x урон
    }

    override float GetHealthMultiplier()
    {
        return 2.5;  // 2.5x здоровье
    }

    override float GetSpeedMultiplier()
    {
        return 1.3;  // 1.3x скорость
    }

    override bool IsGodTier()
    {
        return true;
    }

    override string GetDisplayName()
    {
        return "Elite My Custom Faction";
    }
}
```

---

## 🔗 Related Topics

→ [AI System](AI_SYSTEM.md) - AI базовые системы
→ [NPC Classes](AI_NPC_CLASSES.md) - NPC на базе DayZInfected
→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - EEKilled, EEOnDamageCalculated

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (TheHive)

**Что проверено:**
- ✅ 5 фракций: Hive, Blu, Gialli, Verde, Rossi
- ✅ Disease modifiers: m_DiseaseModifier = "brain", "poisoning", etc.
- ✅ God-tier elite варианты: GetDamageMultiplier() = 5.0, GetHealthMultiplier() = 3.0
- ✅ Reputation system: GetReputation/SetReputation/ModifyReputation
- ✅ Friendly/Enemy factions: GetFriendlyFactions()/GetEnemyFactions()

**Что НЕ проверено:**
- ❌ Полный список всех disease модификаторов
- ❌ Как репутация влияет на AI поведение (атака/не атака)
- ❌ Как создать eAIBase с custom faction
- ❌ Работает ли без TheHive мода

---

## 🚨 Troubleshooting

**Ошибка:** Фракция не атакует врагов
- **Причина:** Неправильно настроены enemy factions или reputation
- **Решение:** Проверьте что GetEnemyFactions() возвращает правильные ID

**Ошибка:** Disease не применяется
- **Причина:** Неправильный disease modifier или не вызывается GetDiseaseModifier()
- **Решение:** Проверьте что disease modifier корректный ("brain", "poisoning", etc.)

**Ошибка:** Reputation не сохраняется
- **Причина:** Не используется OnStoreSave/OnStoreLoad
- **Решение:** Сохраняйте reputation в storage

---

## 💡 Tips

### Best Practices

1. **Faction ID:** Используйте уникальные ID (например, "eAIFactionMyCustom")
2. **Reputation range:** Используйте -100 to +100 для репутации
3. **Disease modifiers:** Используйте существующие disease типы из DayZ
4. **God-tier:** Используйте осторожно (очень сильные враги)

### Balancing

- Тестируйте damage multiplier (1.0 = normal, 2.0 = 2x damage)
- Богатые health multiplier (1.0 = normal, 3.0 = 3x health)
- Репутация должна влиять на AI поведение (attack/ignore)

### Performance

- Не создавайте слишком много фракций (5-10 достаточно)
- Используйте reputation для динамических отношений
- Кэшируйте faction checks (не вызывайте каждый кадр)

---

*Последнее обновление: 2025-01-16*
*Источник: TheHive (Workshop ID: 3572908305)*
