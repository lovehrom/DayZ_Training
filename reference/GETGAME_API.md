# GetGame API - Глобальные методы игры

**✅ ПРОВЕРЕНО:** raw_data/PlayerBase.c (40+ использований)
**Источник:** Распакованный PBO (игровые файлы)

---

## Server/Client Проверки

### IsServer()

Проверяет, выполняется ли код на сервере.

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:368, 1153, 1359, 1402, 2263
if (GetGame().IsServer())
{
    // Код только для сервера
}
```

### IsDedicatedServer()

Проверяет, является ли сервер выделенным (без клиента).

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:394, 1065, 1344, 1389, 2290, 2624
if (!GetGame().IsDedicatedServer())
{
    // Не выделенный сервер (или клиент)
}
```

### IsMultiplayer()

Проверяет, мультиплеер ли это.

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:1075, 2246, 2263, 2358, 2485, 2490, 2544
if (GetGame().IsMultiplayer())
{
    // Мультиплеер режим
}

// Комбинация:
if (GetGame().IsMultiplayer() && GetGame().IsServer())
{
    // Сервер в мультиплеере
}
```

### IsClient()

Прямого `IsClient()` нет в raw_data. Используйте:

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:2485, 2544
if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT && GetGame().IsMultiplayer())
{
    // Клиент в мультиплеере
}
```

---

## GetCallQueue()

Очередь вызовов с категориями.

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:551, 1357
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.OnPlayerLoaded);
GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateCorpseStateVisual, 200, false);
```

**Категории:** `CALL_CATEGORY_SYSTEM`, `CALL_CATEGORY_GAMEPLAY`, `CALL_CATEGORY_GUI`

---

## GetMission()

Доступ к миссии и мировым данным.

```c
// ✅ ПРОВЕРЕНО через агента:
GetGame().GetMission().GetHud()
GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this)
```

---

## RPC()

Remote Procedure Call (см. [RPC_NETWORKING.md](RPC_NETWORKING.md)).

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:3753, 4310, 4320, 5891, 5906
GetGame().RPCSingleParam(this, ERPCs.RPC_SHOCK, damage, true, GetIdentity());
GetGame().RPC(this, ERPCs.RPC_INIT_SET_QUICKBAR, params, true, identity);
```

---

## GetPlayer()

Получить текущего игрока (только на клиенте).

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:2246
if (GetGame().GetPlayer() && GetGame().GetPlayer().GetID() == this.GetID())
{
    // Это локальный игрок
}
```

---

## Template

### Server-only код

```c
class MyItem extends ItemBase
{
    override void OnConsume(PlayerBase player)
    {
        super.OnConsume(player);

        // Только на сервере
        if (GetGame().IsServer())
        {
            // Логика сервера
            player.SetHealth("GlobalHealth", "Health", 100);
        }
    }
}
```

### Server + Multiplayer проверка

```c
class MyItem extends ItemBase
{
    void ServerFunction()
    {
        // Сервер в мультиплеере ИЛИ одиночная игра
        if (GetGame().IsMultiplayer() && GetGame().IsServer())
        {
            // Сделать что-то
        }
    }
}
```

### Отложенный вызов

```c
class MyItem extends ItemBase
{
    void DelayedAction()
    {
        // Вызвать через 200 мс
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.MyMethod, 200, false);
    }

    void MyMethod()
    {
        // Будет вызван через 200 мс
    }
}
```

---

## Источники

- **raw_data/PlayerBase.c** (строки 368, 394, 551, 1065, 1075, 1153, 1344, 1357, 1359, 1389, 1402, 1722, 2246, 2263, 2290, 2358, 2485, 2490, 2544, 2624, 2959, 3078)

---

## ⚠️ Что НЕ найдено

- `GetGame().IsClient()` - используйте `GetInstanceType() == ...INSTANCETYPE_CLIENT`
- Полный список всех GetGame() методов
- Официальная документация по GetGame()

---

*Последнее обновление: 2025-01-16*
*Статус: Проверено на raw_data*
