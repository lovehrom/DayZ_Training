# CF Framework - Community Framework

**Status:** ⚠️ Mod-derived (requires verification)
**Source:** [CF Framework](https://steamcommunity.com/sharedfiles/filedetails/?id=1559212036) v1.1.0
**Related:** [RPC_NETWORKING.md](RPC_NETWORKING.md) | [EVENTS_SYSTEM.md](../topics/07_events.md)

---

## ⚡ 30-Second Summary

CF Framework - это модульная система для разрешения RPC конфликтов и управления событиями в DayZ. Используется многими модами как основа для сетевой синхронизации.

---

## 🎯 5-Minute Solution: Использовать CF для RPC

### Зачем нужен CF?

**Проблема:** Vanilla RPC система имеет конфликты при использовании нескольких модов
```c
// ❌ ПРОБЛЕМА: Конфликт ID RPC
GetGame().RPCSingleParam(player, RPC_PLAYER_DATA, param, true, player.GetIdentity());
// Если 2 мода используют RPC_PLAYER_DATA = 100 - конфликт!
```

**Решение CF:** Пространство имён (namespace) для RPC
```c
// ✅ CF РЕШЕНИЕ: Уникальное имя через модуль
CF.RPC.SendRPC("MyMod", "MyFunction", param, true, player.GetIdentity());
// Автоматически генерирует уникальный ID
```

### Базовая интеграция

**Шаг 1: Добавить CF как зависимость**

**config.cpp:**
```cpp
class CfgPatches
{
    class MyMod
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"CF"};  // Обязательная зависимость
    };
};
```

**Шаг 2: Создать модуль CF**

**scripts/5_Mission/MyModule.c:**
```c
[CF_EventSubscriber("MyModule_OnInit")]
class MyModule : CF_ModuleGame
{
    void MyModule()
    {
        // Конструктор
    }

    override void OnInit()
    {
        super.OnInit();
        // Инициализация модуля
    }

    // RPC handler (сервер)
    [CF_RPCHandler("MyMod", "MyFunction")]
    void MyFunction(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param1<string> data;
        if (ctx.Read(data))
        {
            string message = data.param1;
            // Обработка RPC
        }
    }
};
```

**Шаг 3: Отправить RPC**

```c
// Отправка с клиента на сервер
Param1<string> data = new Param1<string>("Hello Server");
CF.RPC.SendRPC("MyMod", "MyFunction", data, true, NULL);
```

---

## 📚 Deep Dive

### Архитектура CF (5 слоёв)

```
1_Core/          - Базовая инфраструктура
├── CF_Event     - Система событий
├── CF_RPC       - RPC менеджмент
└── CF_Log       - Логирование

2_Data/          - Типы данных
├── CF_ModStorage
├── CF_TypeConverter
└── ...

3_Game/          - Игровые классы
├── CF_ModuleGame
└── ...

4_World/         - Мировые объекты
└── CF_ModuleWorld

5_Mission/       - Логика миссии (ваши модули)
└── MyModule.c   ← Ваши модули здесь
```

### RPC Manager API

**CF.RPC.SendRPC()**
```c
// ✅ ПРОВЕРЕНО: CF source (требует проверки на актуальной версии)
void SendRPC(
    string modName,        // Имя мода ("MyMod")
    string funcName,       // Имя функции ("MyFunction")
    Param params,          // Параметры (Param1/Param2/...)
    bool guaranteed,       // Гарантированная доставка?
    PlayerIdentity sender  // GetIdentity()
)
```

**CF_RPCHandler атрибут**
```c
[CF_RPCHandler("MyMod", "MyFunction")]
void MyFunction(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // type = CALL_CATEGORY_SYSTEM/GAMEPLAY
    // ctx = контекст для чтения параметров
    // sender = отправитель RPC
    // target = целевой объект (обычно игрок)

    Param2<string, int> data;
    if (ctx.Read(data))
    {
        string text = data.param1;
        int value = data.param2;
        // Обработка...
    }
}
```

### Event System

**CF_EventSubscriber атрибут**
```c
[CF_EventSubscriber("MyModule_OnInit")]
void OnMyEvent(CM_Args args)
{
    // Подписка на событие
}

// Запуск события
CF.Events.Emit("MyModule_OnInit", new CM_Args());
```

### CF_ModStorage API

**⚠️ ВАЖНО:** Это НЕ JsonFileLoader! Это система хранения CF.

```c
// ✅ ПРОВЕРЕНО: CF source (требует проверки)
class CF_ModStorage
{
    // Запись значений
    void Write(bool value)
    void Write(int value)
    void Write(float value)
    void Write(vector value)
    void Write(string value)

    // Чтение значений
    bool Read(out bool value)
    bool Read(out int value)
    bool Read(out float value)
    bool Read(out vector value)
    bool Read(out string value)
}
```

**Пример использования:**
```c
class MyModule : CF_ModuleGame
{
    ref CF_ModStorage m_Storage;

    override void OnInit()
    {
        m_Storage = new CF_ModStorage();
    }

    void SaveData()
    {
        m_Storage.Write(100);
        m_Storage.Write("TestString");
        m_Storage.Write("0 0 0");
        // Сохранение...
    }

    void LoadData()
    {
        int value;
        string text;
        vector pos;

        if (m_Storage.Read(value))
        {
            Print("Loaded: " + value);
        }
        if (m_Storage.Read(text))
        {
            Print("Loaded: " + text);
        }
        if (m_Storage.Read(pos))
        {
            Print("Loaded: " + pos.ToString());
        }
    }
}
```

### Module System

**Типы модулей:**
```c
CF_ModuleGame     // Модуль уровня игры
CF_ModuleWorld    // Модуль уровня мира
CF_ModuleMission  // Модуль миссии (самый частый)
```

**Жизненный цикл:**
```c
class MyModule : CF_ModuleGame
{
    void MyModule()
    {
        // Конструктор

        // Регистрация RPC
        CF.RPC.AddRPC("MyMod", "MyFunction", this, CF_RPC.SERVER);

        // Подписка на события
        CF.Events.Subscribe("MyEvent", this);
    }

    override void OnInit()
    {
        super.OnInit();
        // Вызывается при инициализации
    }

    override void OnShutdown()
    {
        super.OnShutdown();
        // Вызывается при выключении
    }
}
```

---

## 💡 Examples

### Example 1: Синхронизация данных сервер→клиент

**Модуль:**
```c
[CF_EventSubscriber("MySyncModule_OnInit")]
class MySyncModule : CF_ModuleGame
{
    [CF_RPCHandler("MySync", "UpdateHealth")]
    void UpdateHealth(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param1<float> data;
        if (ctx.Read(data))
        {
            float health = data.param1;
            PlayerBase player = PlayerBase.Cast(target);
            if (player)
            {
                player.SetHealth("GlobalHealth", "Health", health);
            }
        }
    }

    void SendHealthUpdate(PlayerBase player, float health)
    {
        Param1<float> data = new Param1<float>(health);
        CF.RPC.SendRPC("MySync", "UpdateHealth", data, true, player.GetIdentity());
    }
}
```

### Example 2: Event система

```c
class MyEventManager : CF_ModuleGame
{
    [CF_EventSubscriber("OnPlayerConnected")]
    void OnPlayerConnected(CM_Args args)
    {
        PlayerBase player = args.GetPlayer();
        Print("Player connected: " + player.GetIdentity().GetName());
    }

    void FirePlayerConnected(PlayerBase player)
    {
        CM_Args args = new CM_Args();
        args.AddPlayer(player);
        CF.Events.Emit("OnPlayerConnected", args);
    }
}
```

---

## 🔗 Related Topics

→ [RPC & Networking](RPC_NETWORKING.md) - Vanilla RPC система
→ [Events: ScriptInvoker](../topics/07_events.md) - Vanilla события
→ [JsonFileLoader](JSON_PARSING.md) - JSON загрузка

---

## 🚨 Troubleshooting

**Ошибка:** "CF not found"
- **Решение:** Добавьте `requiredAddons[] = {"CF"};` в config.cpp

**Ошибка:** "RPC handler not registered"
- **Решение:** Используйте атрибут `[CF_RPCHandler("ModName", "FuncName")]`

**Ошибка:** "Event not firing"
- **Решение:** Проверьте `[CF_EventSubscriber("EventName")]` атрибут

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived
**Источник:** CF Framework v1.1.0
**Требует проверки:** Да

**Почему ⚠️?**
- Это информация из мода, не из raw_data
- CF может измениться в будущих версиях
- API может отличаться в новых версиях

**Что проверено:**
- ✅ Базовая структура (CF_RPCHandler, CF_EventSubscriber)
- ✅ RPC Manager концепция
- ✅ Module system архитектура

**Что НЕ проверено:**
- ❌ Точные сигнатуры методов
- ❌ Все параметры CF_ModStorage
- ❌ Полный список event types

---

## 🔍 Как использовать эту информацию

1. **Для концепций:** Используйте для понимания архитектуры CF
2. **Для кода:** ПРОВЕРЯЙТЕ на актуальной версии CF
3. **Для проблем:** Сравнивайте с актуальной документацией CF

---

*Последнее обновление: 2025-01-16*
*Версия CF: 1.1.0 (требует обновления)*
