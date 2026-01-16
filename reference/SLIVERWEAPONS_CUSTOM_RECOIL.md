# SliverWeapons - Custom Recoil System

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3644228096 / SliverWeapons
**Related:** [Weapon Base](../raw_data/ItemBase.c) | [Recoil System]

---

## ⚡ 30-Second Summary

Переопределение стандартной отдачи оружия путём возврата нестандартного `RecoilBase` объекта в методе `SpawnRecoilObject()`.

---

## 🎯 5-Minute Solution: Изменить отдачу оружия

### Override SpawnRecoilObject()

```c
// ✅ ПРОВЕРЕНО: M300.c, SSG550.c (SliverWeapons)
class sl_m300_base : BoltActionRifle_ExternalMagazine_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        // M300 использует отдачу от CZ527 вместо стандартной
        return new CZ527Recoil(this);
    }
}
```

### Нестандартное сопоставление оружие→отдача

```c
// ✅ ПРОВЕРЕНО: SliverWeapons pattern
class sl_ssg550_base : Rifle_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        // SSG550 использует отдачу от AKM
        return new AkmRecoil(this);
    }
}
```

---

## 📚 Deep Dive

### Vanilla recoil pattern

**✅ ПРОВЕРЕНО:** Standard DayZ weapons use:

```c
// Стандартный vanilla pattern (пример из M24)
class M24_Base extends BoltActionRifle_ExternalMagazine_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        return new M24Recoil(this);  // Стандартная отдача
    }
}
```

### SliverWeapons custom pattern

**✅ ПРОВЕРЕНО:** Non-standard recoil assignment:

```c
// SliverWeapons: M300 → CZ527 recoil
class sl_m300_base : BoltActionRifle_ExternalMagazine_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        return new CZ527Recoil(this);  // НЕ M300Recoil!
    }
}

// SliverWeapons: SSG550 → AKM recoil
class sl_ssg550_base : Rifle_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        return new AkmRecoil(this);  // НЕ SSG550Recoil!
    }
}
```

**Уникальность:**
- Оружие использует отдачу от **другого** оружия
- Позволяет быстро настроить "чувство" оружия без создания нового RecoilBase класса

### Доступные RecoilBase классы (vanilla)

```c
// Примеры vanilla recoil классов:
M24Recoil          // Снайперская винтовка M24
CZ527Recoil        // Карабин CZ527
AkmRecoil          // Автомат AKM
SvdRecoil          // Снайперская винтовка SVD
KasinkaRecoil      // Пулемёт M249 (Kasinka)
// ... и другие
```

---

## 💡 Examples

### Example 1: Custom rifle with pistol recoil

```c
class MyCustomRifle : Rifle_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        // Лёгкая отдача как у пистолета
        return new FNX45Recoil(this);
    }
}
```

### Example 2: Heavy sniper with machine gun recoil

```c
class MyHeavySniper : BoltActionRifle_ExternalMagazine_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        // Тяжёлая отдача как у пулемёта
        return new KasinkaRecoil(this);
    }
}
```

### Example 3: SMG with assault rifle recoil

```c
class MyCustomSMG : SubMachineGun_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        // Увеличенная отдача для SMG
        return new M4A1Recoil(this);
    }
}
```

---

## 🔗 Related Topics

→ [ItemBase](../raw_data/ItemBase.c) - Базовый класс предметов
→ [Weapon Base](../raw_data/ItemBase.c) - Базовый класс оружия
→ [Physics System](PHYSICS_SYSTEM.md) - Physics mechanics

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (SliverWeapons)

**Что проверено:**
- ✅ M300.c: `override RecoilBase SpawnRecoilObject() { return new CZ527Recoil(this); }`
- ✅ SSG550.c: `override RecoilBase SpawnRecoilObject() { return new AkmRecoil(this); }`
- ✅ Нестандартное сопоставление оружие→отдача

**Что НЕ проверено:**
- ❌ Полный список всех vanilla RecoilBase классов
- ❌ Работает ли с custom RecoilBase классами
- ❌ Влияет ли это на синхронизацию отдачи между клиентом и сервером

---

## 🚨 Troubleshooting

**Ошибка:** Отдача не изменилась
- **Причина:** Не переопределён SpawnRecoilObject() или неверно указан класс
- **Решение:** Проверьте правильность имени RecoilBase класса

**Ошибка:** "Class Not Found" для RecoilBase
- **Причина:** Указан несуществующий recoil класс
- **Решение:** Используйте только существующие vanilla классы или создайте свой

**Ошибка:** Отдача слишком сильная/слабая
- **Причина:** Неподходящий recoil класс
- **Решение:** Попробуйте другой recoil класс из vanilla

---

## 💡 Tips

### Best Practices

1. **Тестируйте разные recoil классы** - попробуйте несколько вариантов для лучшего "чувства"
2. **Баланс оружия** - учитывайте роль оружия (DMR должен иметь меньшую отдачу чем shotgun)
3. **Документируйте изменения** - укажите в комментариях какой recoil используете

### Recoil подбор по типу оружия

| Тип оружия | Рекомендуемый recoil | Пример |
|-----------|---------------------|--------|
| Sniper rifles | M24Recoil, SvdRecoil, CZ527Recoil | M300 → CZ527Recoil |
| Assault rifles | M4A1Recoil, AkmRecoil, SSG550Recoil | SSG550 → AkmRecoil |
| SMG | FNX45Recoil, MP5Recoil | - |
| LMG | KasinkaRecoil (M249) | - |
| Shotguns | RepeaterRecoil, Izh43Recoil | - |

---

*Последнее обновление: 2025-01-16*
*Источник: SliverWeapons (Workshop ID: 3644228096)*
