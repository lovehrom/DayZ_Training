# Weapon Customization Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [ItemBase](../raw_data/ItemBase.c) | [Weapon Base](../raw_data/)

---

## ⚡ 30-Second Summary

Кастомизация оружия: отдача, звуки, механики стрельбы через override методов.

---

## 🎯 5-Minute Solution: Изменить отдачу оружия

### Override SpawnRecoilObject()

```c
class MyCustomRifle : BoltActionRifle_ExternalMagazine_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        // M300 использует отдачу от CZ527
        return new CZ527Recoil(this);
    }
}
```

### Custom sound broadcast

```c
static const int RPC_WEAPON_SOUND = 77801;

void BroadcastWeaponSound()
{
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    foreach (Man man : players)
    {
        PlayerBase pb = PlayerBase.Cast(man);
        if (pb)
        {
            GetGame().RPCSingleParam(pb, RPC_WEAPON_SOUND, null, true, pb.GetIdentity());
        }
    }
}
```

---

## 📚 Deep Dive

## SliverWeapons (3644228096) - Custom Recoil System

### Нестандартное сопоставление оружие→отдача

```c
// M300 → CZ527 recoil
class sl_m300_base : BoltActionRifle_ExternalMagazine_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        return new CZ527Recoil(this);  // НЕ M300Recoil!
    }
}

// SSG550 → AKM recoil
class sl_ssg550_base : Rifle_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        return new AkmRecoil(this);  // НЕ SSG550Recoil!
    }
}
```

**Уникальность:** Быстрое изменение "чувства" оружия без создания нового RecoilBase класса.

### Доступные vanilla recoil классы

```c
M24Recoil          // Снайперская винтовка
CZ527Recoil        // Карабин
AkmRecoil          // Автомат
SvdRecoil          // Снайперская SVD
KasinkaRecoil      // Пулемёт M249
```

---

## 💡 Examples

### Example 1: Custom rifle с pistol recoil

```c
class MyCustomRifle : Rifle_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        return new FNX45Recoil(this);  // Лёгкая отдача
    }
}
```

### Example 2: Heavy sniper с machine gun recoil

```c
class MyHeavySniper : BoltActionRifle_ExternalMagazine_Base
{
    override RecoilBase SpawnRecoilObject()
    {
        return new KasinkaRecoil(this);  // Тяжёлая отдача
    }
}
```

---

## 🔗 Related Topics

→ [ItemBase](../raw_data/ItemBase.c) - Базовый класс предметов
→ [Physics System](PHYSICS_SYSTEM.md) - Physics механики

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (SliverWeapons)

**Что проверено:**
- ✅ M300.c: `override RecoilBase SpawnRecoilObject() { return new CZ527Recoil(this); }`
- ✅ SSG550.c: `override RecoilBase SpawnRecoilObject() { return new AkmRecoil(this); }`
- ✅ RPC_WEAPON_SOUND = 77801 broadcast pattern

**Что НЕ проверено:**
- ❌ Полный список всех vanilla RecoilBase классов
- ❌ Работает ли без SliverWeapons мода

---

## 💡 Tips

### Recoil подбор

| Тип оружия | Рекомендуемый recoil |
|-----------|---------------------|
| Sniper rifles | M24Recoil, SvdRecoil, CZ527Recoil |
| Assault rifles | M4A1Recoil, AkmRecoil, SSG550Recoil |
| SMG | FNX45Recoil, MP5Recoil |
| LMG | KasinkaRecoil |

---

*Последнее обновление: 2025-01-16*
*Источник: SliverWeapons (Workshop ID: 3644228096)*
