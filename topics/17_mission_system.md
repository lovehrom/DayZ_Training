# Mission System

**Status:** ⚠️ Advanced
**Related:** [GUI Systems](16_gui_systems.md) | [GetGame API](../reference/GETGAME_API.md)

---

## ⚡ 30-Second Summary

Mission API предоставляет доступ к HUD, WorldData (температура, загрязнение), трупам и другим системам сервера.

---

## 🎯 5-Minute Solution

### Получить HUD

```c
Hud hud = GetGame().GetMission().GetHud();
if (hud)
{
    hud.ShowHudUI(false); // Скрыть
    hud.ShowHudUI(true);  // Показать
}
```

### Получить температуру

```c
float envTemp = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);
```

---

## 💡 Examples

### Пример 1: Управление HUD

```c
void HideHUD()
{
    Hud hud = GetGame().GetMission().GetHud();
    if (hud) hud.ShowHudUI(false);
}

void ShowHUD()
{
    Hud hud = GetGame().GetMission().GetHud();
    if (hud) hud.ShowHudUI(true);
}
```

### Пример 2: Получить данные мира

```c
class MissionData
{
    float GetTemperature()
    {
        return GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);
    }

    float GetPollution()
    {
        return GetGame().GetMission().GetWorldData().GetEnvTemperaturePollutionAtObject(this);
    }
}
```

---

## 🔗 Related Topics

→ [GUI Systems](16_gui_systems.md) - UI widgets
→ [Reference: Mission API](../reference/MISSION_API.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
