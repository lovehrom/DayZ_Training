# Weapon Improvements (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Weapon Customization](WEAPON_CUSTOMIZATION.md) | [Animation System](ANIMATION_SYSTEM.md)

---

## ⚡ 30-Second Summary

Улучшения оружия: сохранение ADS при перезарядке, override действий, enhanced weapon manager.

---

## 🎯 5-Minute Solution: ADS Preservation on Reload

### Override reload action

```c
modded class WeaponManager
{
    protected bool m_WasInADS;

    override void StartWeaponAction(int action, Magazine mag, ItemBase suppressor)
    {
        // Check if player is in ADS before reload
        if (action == WeaponActions.RELOAD)
        {
            PlayerBase player = m_Player;
            m_WasInADS = player.IsInADS();
        }

        super.StartWeaponAction(action, mag, suppressor);
    }

    override void EndWeaponAction()
    {
        super.EndWeaponAction();

        // Restore ADS after reload
        if (m_WasInADS)
        {
            PlayerBase player = m_Player;

            // Force back into ADS
            player.EnterSights();
            m_WasInADS = false;
        }
    }
}
```

---

## 📚 Deep Dive

## BetterBoltAction (3641964151) - ADS Preservation System

### Weapon action override

```c
modded class WeaponManager
{
    protected bool m_IsInADS;

    override void StartWeaponAction(int action, Magazine mag, ItemBase suppressor)
    {
        // Store ADS state before action
        PlayerBase player = m_Player;
        m_IsInADS = player.IsInADS();

        super.StartWeaponAction(action, mag, suppressor);
    }

    override void Update(float delta_time)
    {
        super.Update(delta_time);

        // Check if action completed and was in ADS
        if (m_IsInADS && !IsInWeaponAction())
        {
            PlayerBase player = m_Player;

            // Restore ADS
            if (!player.IsInADS())
            {
                player.EnterSights();
            }

            m_IsInADS = false;
        }
    }

    override void OnWeaponActionEnd()
    {
        super.OnWeaponActionEnd();

        // Restore ADS after action completes
        if (m_IsInADS)
        {
            PlayerBase player = m_Player;
            player.EnterSights();
            m_IsInADS = false;
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3641964151\Addons\BetterBoltAction\BetterBoltAction\scripts\4_world\classes\weapons\weaponmanager.c:3-9, 86-99`

### Enhanced weapon manager with improved logic

```c
modded class WeaponManager
{
    protected ref map<int, bool> m_ActionADSState = new map<int, bool>;

    override void StartWeaponAction(int action, Magazine mag, ItemBase suppressor)
    {
        // Store ADS state for each action type
        PlayerBase player = m_Player;
        m_ActionADSState.Set(action, player.IsInADS());

        super.StartWeaponAction(action, mag, suppressor);
    }

    override void EndWeaponAction()
    {
        int currentAction = GetCurrentWeaponAction();

        super.EndWeaponAction();

        // Restore ADS if was in ADS before action
        if (m_ActionADSState.Find(currentAction) != -1)
        {
            if (m_ActionADSState.Get(currentAction))
            {
                PlayerBase player = m_Player;
                player.EnterSights();
            }

            m_ActionADSState.Remove(currentAction);
        }
    }

    bool WasInADSBeforeAction(int action)
    {
        if (m_ActionADSState.Find(action) != -1)
        {
            return m_ActionADSState.Get(action);
        }

        return false;
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3641964151\Addons\BetterBoltAction\BetterBoltAction\scripts\4_world\classes\weapons\weaponmanager.c:11-122`

### Action-specific ADS handling

```c
modded class WeaponManager
{
    override void StartWeaponAction(int action, Magazine mag, ItemBase suppressor)
    {
        PlayerBase player = m_Player;
        bool wasInADS = player.IsInADS();

        super.StartWeaponAction(action, mag, suppressor);

        // Handle different actions
        switch (action)
        {
            case WeaponActions.RELOAD:
                // Store ADS state for reload
                m_InADS_Reload = wasInADS;
                break;

            case WeaponActions.CHAMBER:
                // Store ADS state for chambering
                m_InADS_Chamber = wasInADS;
                break;

            case WeaponActions.MEASURE:
                // Don't restore ADS for measure action
                break;
        }
    }

    override void OnWeaponActionEnd()
    {
        super.OnWeaponActionEnd();

        PlayerBase player = m_Player;
        int action = GetCurrentWeaponAction();

        // Restore ADS based on action type
        switch (action)
        {
            case WeaponActions.RELOAD:
                if (m_InADS_Reload)
                {
                    player.EnterSights();
                    m_InADS_Reload = false;
                }
                break;

            case WeaponActions.CHAMBER:
                if (m_InADS_Chamber)
                {
                    player.EnterSights();
                    m_InADS_Chamber = false;
                }
                break;
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3641964151\Addons\BetterBoltAction\BetterBoltAction\scripts\4_world\classes\weapons\weaponmanager.c:44-66`

---

## 💡 Examples

### Example 1: Instant ADS restoration

```c
modded class WeaponManager
{
    override void EndWeaponAction()
    {
        super.EndWeaponAction();

        // Immediately restore ADS
        PlayerBase player = m_Player;

        if (!player.IsInADS() && player.IsRaised())
        {
            // Force ADS
            player.EnterSights();
        }
    }
}
```

### Example 2: Smooth ADS transition

```c
modded class WeaponManager
{
    protected float m_ADSRestoreTimer;
    protected const float ADS_RESTORE_DELAY = 0.1; // 100ms delay

    override void EndWeaponAction()
    {
        super.EndWeaponAction();

        // Schedule ADS restore with delay
        m_ADSRestoreTimer = ADS_RESTORE_DELAY;
    }

    override void Update(float delta_time)
    {
        super.Update(delta_time);

        // Smooth ADS restoration
        if (m_ADSRestoreTimer > 0)
        {
            m_ADSRestoreTimer -= delta_time;

            if (m_ADSRestoreTimer <= 0)
            {
                PlayerBase player = m_Player;

                if (!player.IsInADS() && player.IsRaised())
                {
                    player.EnterSights();
                }

                m_ADSRestoreTimer = 0;
            }
        }
    }
}
```

### Example 3: Weapon-specific ADS handling

```c
modded class WeaponManager
{
    override void StartWeaponAction(int action, Magazine mag, ItemBase suppressor)
    {
        super.StartWeaponAction(action, mag, suppressor);

        // Only preserve ADS for bolt-action rifles
        Weapon weapon = m_Player.GetWeaponInHands();

        if (weapon && weapon.IsInherited(BoltActionRifle))
        {
            m_PreserveADS = true;
        }
        else
        {
            m_PreserveADS = false;
        }
    }

    override void EndWeaponAction()
    {
        super.EndWeaponAction();

        if (m_PreserveADS)
        {
            PlayerBase player = m_Player;
            player.EnterSights();
            m_PreserveADS = false;
        }
    }
}
```

---

## 🔗 Related Topics

→ [Weapon Customization](WEAPON_CUSTOMIZATION.md) - Recoil override
→ [Animation System](ANIMATION_SYSTEM.md) - Hand animations
→ [PlayerBase API](PLAYERBASE_API.md) - IsInADS(), EnterSights()

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ ADS preservation: weaponmanager.c:3-9, 86-99
- ✅ Enhanced manager: weaponmanager.c:11-122
- ✅ Action-specific handling: weaponmanager.c:44-66

**Что НЕ проверено:**
- ❌ Работает ли без BetterBoltAction мода
- ❌ Конфликты с другими weapon модами
- ❌ Производительность с частыми reload

---

## 💡 Tips

### Best Practices

1. **Проверяйте IsInADS()** перед StartWeaponAction
2. **Используйте EnterSights()** для восстановления ADS
3. **Храните состояние** для каждого типа действия
3. **Очищайте флаги** после восстановления ADS
4. **Учитывайте weapon type** (bolt-action, semi-auto, etc.)

### Weapon Actions

**Common Actions:**
- `WeaponActions.RELOAD` - Перезарядка магазина
- `WeaponActions.CHAMBER` - Заряжание патрона в патронник
- `WeaponActions.MEASURE` - Измерение расстояния (range finder)
- `WeaponActions.INSPECT` - Осмотр оружия

### When to Preserve ADS

**Recommended:**
- ✅ Bolt-action rifles
- ✅ Lever-action rifles
- ✅ Pump-action shotguns

**Not Recommended:**
- ❌ Full-auto weapons
- ❌ Melee weapons
- ❌ Range finders

### Common Pitfalls

- ❌ **Восстанавливать ADS слишком рано** → animation glitch
- ❌ **Игнорировать IsRaised()** → incorrect state
- ❌ **Забыть очистить флаг** → permanent ADS
- ❌ **Восстанавливать для всех weapons** → weird behavior

---

*Последнее обновление: 2025-01-16*
*Источник: BetterBoltAction (3641964151)*
