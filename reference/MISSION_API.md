# Mission API - Миссия и окружение

**Status:** ✅ Verified from raw_data
**Sources:** PlayerBase.c (40+ использований), EntityAI.c
**Related:** [GetGame API](GETGAME_API.md) | [Sound System](SOUND_SYSTEM.md)

---

## ⚡ 30-Second Summary

`GetGame().GetMission()` предоставляет доступ к HUD, трупам, респавну, WorldData (температура, загрязнение) и другим системам миссии.

---

## 🎯 5-Minute Solution: Доступ к HUD и WorldData

### Получить HUD

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:340
Hud hud = GetGame().GetMission().GetHud();

if (hud)
{
    // Показать/скрыть HUD
    hud.ShowHudUI(false);
    hud.ShowHudUI(true);
}
```

### Получить температуру окружающей среды

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:2258,3842
float envTemp = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);
Print("Environment temperature: " + envTemp + "°C");
```

---

## 📚 Deep Dive

### GetMission().GetHud()

**✅ ПРОВЕРЕНО:** PlayerBase.c:340, 3379

```c
Hud hud = GetGame().GetMission().GetHud();
```

**Методы HUD (найденные в raw_data):**
```c
// Show/Hide UI
hud.ShowHudUI(bool show);           // ✅ PlayerBase.c:3379
hud.ShowQuickbarUI(bool show);      // ✅ PlayerBase.c:2283

// Примеры:
modded class PlayerBase
{
    void HideMyHUD()
    {
        Hud hud = GetGame().GetMission().GetHud();
        if (hud)
        {
            hud.ShowHudUI(false);
        }
    }

    void ShowMyHUD()
    {
        Hud hud = GetGame().GetMission().GetHud();
        if (hud)
        {
            hud.ShowHudUI(true);
            hud.ShowQuickbarUI(true);
        }
    }
}
```

### GetMission().InsertCorpse()

**✅ ПРОВЕРЕНО:** PlayerBase.c:1069-1079

```c
// Вставить труп в миссию (для обработки)
if (GetEconomyProfile() && !m_CorpseProcessing && m_CorpseState == 0)
{
    if (GetGame().GetMission().InsertCorpse(this))
    {
        m_CorpseProcessing = true;
        // Труп будет обработан системой
    }
}
```

### GetMission().OnPlayerRespawned()

**✅ ПРОВЕРЕНО:** PlayerBase.c:2281

```c
// Вызывается когда игрок респавнился
GetGame().GetMission().OnPlayerRespawned(this);
GetGame().GetMission().SetPlayerRespawning(false);

// Пример использования:
modded class PlayerBase
{
    override void OnReconnect()
    {
        super.OnReconnect();

        // Сообщить миссии что игрок респавнился
        GetGame().GetMission().OnPlayerRespawned(this);

        // Показать HUD
        Hud hud = GetGame().GetMission().GetHud();
        if (hud)
        {
            hud.ShowQuickbarUI(true);
        }
    }
}
```

### GetMission().HideInventory()

**✅ ПРОВЕРЕНО:** PlayerBase.c:1922, 1933

```c
// Скрыть инвентарь
if (GetGame().GetUIManager().IsMenuOpen(MENU_INVENTORY))
{
    GetGame().GetMission().HideInventory();
}
```

### GetMission().GetWorldData()

**✅ ПРОВЕРЕНО:** PlayerBase.c:7117,7131 | EntityAI.c:2258,3842

```c
WorldData worldData = GetGame().GetMission().GetWorldData();
```

**WorldData методы (найденные в raw_data):**

#### GetPollution()
```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:7117
float pollution = GetGame().GetMission().GetWorldData().GetPollution();
Print("Pollution level: " + pollution);
```

#### GetLiquidTypeEnviroTemperature()
```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:7131
float liquidTemp = GetGame().GetMission().GetWorldData().GetLiquidTypeEnviroTemperature(
    GetPosition(),  // position
    0              // liquid type (0 = ?
);

Print("Liquid temperature: " + liquidTemp + "°C");
```

#### GetBaseEnvTemperatureAtObject()
```c
// ✅ ПРОВЕРЕНО: EntityAI.c:2258,3842
float envTemp = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(
    this  // object (EntityAI)
);

Print("Environment temperature at object: " + envTemp + "°C");
```

---

## 💡 Examples

### Example 1: Проверка температуры окружающей среды

```c
modded class PlayerBase
{
    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Получить температуру окружающей среды
        float envTemp = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);

        Print("Environment temp: " + envTemp + "°C");

        // Если холодно - показать сообщение
        if (envTemp < 0)
        {
            Print("It's freezing!");
        }
    }
}
```

### Example 2: Custom HUD toggle

```c
modded class PlayerBase
{
    protected bool m_CustomHUDHidden = false;

    void ToggleCustomHUD()
    {
        m_CustomHUDHidden = !m_CustomHUDHidden;

        Hud hud = GetGame().GetMission().GetHud();
        if (hud)
        {
            hud.ShowHudUI(!m_CustomHUDHidden);
        }

        if (m_CustomHUDHidden)
        {
            SetInventorySoftLock(true);
        }
        else
        {
            SetInventorySoftLock(false);
        }
    }
}
```

### Example 3: Обработка респавна

```c
modded class PlayerBase
{
    override void OnReconnect()
    {
        super.OnReconnect();

        // Сообщить миссии о респавне
        GetGame().GetMission().OnPlayerRespawned(this);
        GetGame().GetMission().SetPlayerRespawning(false);

        // Показать UI
        Hud hud = GetGame().GetMission().GetHud();
        if (hud)
        {
            hud.ShowQuickbarUI(true);
            hud.ShowHudUI(true);
        }
    }
}
```

### Example 4: Проверка загрязнения

```c
modded class PlayerBase
{
    bool IsInPollutedArea()
    {
        float pollution = GetGame().GetMission().GetWorldData().GetPollution();

        if (pollution > 0.5)
        {
            Print("High pollution area!");
            return true;
        }

        return false;
    }
}
```

---

## 🔗 Related Topics

→ [GetGame API](GETGAME_API.md) - GetGame() методы
→ [Sound System](SOUND_SYSTEM.md) - SEffectManager
→ [Temperature System](TEMPERATURE_SYSTEM.md) - GetTemperature()

---

## 🚨 Troubleshooting

**Ошибка:** GetMission() возвращает NULL
- **Причина:** Вызываете слишком рано (OnInit до создания миссии)
- **Решение:** Проверьте GetGame().GetMission() != NULL

**Ошибка:** GetHud() возвращает NULL
- **Причина:** Вызываете на сервере (HUD только на клиенте)
- **Решение:** Используйте `#ifndef SERVER`

**Ошибка:** WorldData методы не работают
- **Причина:** GetWorldData() может вернуть NULL
- **Решение:** Проверьте что WorldData != NULL

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ✅ Verified from raw_data

**Что проверено:**
- ✅ GetMission().GetHud() - PlayerBase.c:340, 3379
- ✅ GetMission().InsertCorpse() - PlayerBase.c:1069-1079
- ✅ GetMission().OnPlayerRespawned() - PlayerBase.c:2281
- ✅ GetMission().HideInventory() - PlayerBase.c:1922,1933
- ✅ GetWorldData().GetPollution() - PlayerBase.c:7117
- ✅ GetWorldData().GetLiquidTypeEnviroTemperature() - PlayerBase.c:7131
- ✅ GetWorldData().GetBaseEnvTemperatureAtObject() - EntityAI.c:2258,3842

**Что НЕ проверено:**
- ❌ Полный список HUD методов
- ❌ Полный список WorldData методов
- ❌ GetActiveRefresherLocations() (упомянут в плане, но не найден в grep)

---

## 🔍 Как использовать эту информацию

1. **HUD:** Используйте GetHud() для показа/скрытия UI
2. **Environment:** GetWorldData() для температуры, загрязнения
3. **Respawn:** OnPlayerRespawned() для обработки респавна
4. **Corpses:** InsertCorpse() для обработки трупов

**ВАЖНО:** GetHud() работает только на клиенте! Используйте `#ifndef SERVER`

---

*Последнее обновление: 2025-01-16*
*Источник: raw_data/PlayerBase.c (40+ использований), EntityAI.c*
