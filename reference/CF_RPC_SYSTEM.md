# CF RPC System - Community Framework RPC

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 1559212036 / Community Framework v0.0.0
**Related:** [RPC Networking](RPC_NETWORKING.md) | [GetGame API](GETGAME_API.md)

---

## ⚡ 30-Second Summary

Community Framework предоставляет namespace-based RPC систему через `RPCManager`, которая позволяет отправлять RPC вызовы по имени мода и функции вместо ID.

---

## 🎯 5-Minute Solution: Использовать CF RPC

### Регистрация RPC handler

```c
// ✅ ПРОВЕРЕНО: CF/Scripts/3_Game/CommunityFramework/RPC/RPCManager.c:171-187
class MyRPCClass
{
    void RPCHandler(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Обработка RPC
    }

    void RegisterRPCs()
    {
        // Регистрировать handler
        CF.RPC.RegisterRPC("MyMod", "RPCHandler", this);

        // Или с указанием singleplayer execution type
        CF.RPC.RegisterRPC("MyMod", "RPCHandler", this, SingleplayerExecutionType.Both);
    }
}
```

### Отправка RPC

```c
// ✅ ПРОВЕРЕНО: CF/Scripts/3_Game/CommunityFramework/RPC/RPCManager.c:111-139
// Отправка RPC с параметрами
CF.RPC.SendRPC("MyMod", "RPCHandler", new Param1<string>("Hello"));

// Отправка guaranteed RPC
CF.RPC.SendRPC("MyMod", "RPCHandler", new Param1<int>(42), true);

// Отправка конкретному игроку
CF.RPC.SendRPC("MyMod", "RPCHandler", params, false, playerIdentity);

// Отправка конкретному объекту
CF.RPC.SendRPC("MyMod", "RPCHandler", params, false, NULL, targetObject);
```

---

## 📚 Deep Dive

### RPCManager

**✅ ПРОВЕРЕНО:** CF/Scripts/3_Game/CommunityFramework/RPC/RPCManager.c

```c
class RPCManager
{
    static const int FRAMEWORK_RPC_ID = 10042;

    // Отправка RPC
    void SendRPC(
        string modName,                  // Имя мода (namespace)
        string funcName,                  // Имя функции
        Param params = NULL,              // Параметры
        bool guaranteed = false,          // Guaranteed delivery
        PlayerIdentity sendToIdentity = NULL,  // Кому отправить
        Object sendToTarget = NULL        // Целевой объект
    );

    // Регистрация RPC handler
    bool AddRPC(
        string modName,                  // Имя мода
        string funcName,                  // Имя функции
        Class instance,                   // Объект-обработчик
        int singlePlayerExecType = SingleplayerExecutionType.Server
    );

    // Удаление RPC
    void RemoveRPC(string modName, string funcName);
}
```

### Namespace-based RPC

**Принцип работы:**
- RPC идентифицируется по `{modName}::{funcName}` вместо числового ID
- Все CF RPC используют один `FRAMEWORK_RPC_ID = 10042`
- Метаданные (modName, funcName) отправляются первыми в `Param2<string, string>`

**Пример из кода:**

```c
// ✅ ПРОВЕРЕНО: RPCManager.c:56-65
Param2< string, string > metaData;
if ( !ctx.Read( metaData ) )
{
    CF_Log.Warn( "Failed reading the RPC metadata!" );
    return;
}

string modName = metaData.param1;
string funcName = metaData.param2;
```

### Singleplayer Execution Type

**✅ ПРОВЕРЕНО:** RPCManager.c:8-13

```c
enum SingleplayerExecutionType
{
    Server = 0,  // Выполняется на сервере в singleplayer
    Client,      // Выполняется на клиенте в singleplayer
    Both         // Выполняется и на сервере, и на клиенте
}
```

**Использование:**

```c
// Регистрация для выполнения в singleplayer на обеих сторонах
CF.RPC.RegisterRPC("MyMod", "RPCHandler", this, SingleplayerExecutionType.Both);
```

### Call Type

**✅ ПРОВЕРЕНО:** RPCManager.c:15-19

```c
enum CallType
{
    Server = 0,  // Вызов от сервера
    Client       // Вызов от клиента
}
```

**Использование в handler:**

```c
void RPCHandler(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type == CallType.Server)
    {
        // Вызвано на сервере
    }
    else if (type == CallType.Client)
    {
        // Вызвано на клиенте
    }
}
```

---

## 💡 Examples

### Example 1: Simple RPC

**Файл:** CF/Scripts/3_Game/CommunityFramework/RPC/RPCManager.c:111-139

```c
modded class PlayerBase
{
    override void OnInit()
    {
        super.OnInit();

        // Зарегистрировать RPC handler
        CF.RPC.RegisterRPC("MyMod", "MyRPC", this);
    }

    void MyRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param1<string> data;
        if (ctx.Read(data))
        {
            Print("Received: " + data.param1);
        }
    }

    void SendMyRPC(string message)
    {
        // Отправить RPC
        CF.RPC.SendRPC("MyMod", "MyRPC", new Param1<string>(message));
    }
}
```

### Example 2: Multi-param RPC

```c
void SendPlayerData(PlayerIdentity ident)
{
    // Отправить несколько параметров
    CF.RPC.SendRPC("MyMod", "PlayerDataRPC",
        new Param3<string, int, vector>("PlayerName", 100, "0 0 0"),
        false,  // not guaranteed
        ident   // отправить конкретному игроку
    );
}

void PlayerDataRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    Param3<string, int, vector> data;
    if (ctx.Read(data))
    {
        string name = data.param1;
        int health = data.param2;
        vector position = data.param3;
    }
}
```

### Example 3: Guaranteed RPC

```c
void SendCriticalData(PlayerIdentity ident)
{
    // Critical data - guaranteed delivery
    CF.RPC.SendRPC("MyMod", "CriticalRPC",
        new Param1<int>(PlayerScore),
        true,   // guaranteed
        ident
    );
}
```

---

## 🔗 Related Topics

→ [RPC Networking](RPC_NETWORKING.md) - Vanilla RPC система
→ [GetGame API](GETGAME_API.md) - GetGame().RPC()

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (Community Framework)

**Что проверено:**
- ✅ RPCManager.c:111-139 - SendRPC метод
- ✅ RPCManager.c:171-187 - AddRPC метод
- ✅ RPCManager.c:51-109 - OnRPC handler
- ✅ RPCManager.c:8-19 - Enums (SingleplayerExecutionType, CallType)

**Что НЕ проверено:**
- ❌ Работает ли без CF мода
- ❌ Совместимость с версией DayZ
- ❌ Performance impact

---

## 🚨 Troubleshooting

**Ошибка:** "RPC does not seem to exist"
- **Причина:** Handler не зарегистрирован или неправильное modName/funcName
- **Решение:** Проверьте регистрацию через `CF.RPC.RegisterRPC()`

**Ошибка:** "Failed reading the RPC metadata"
- **Причина:** Ошибка чтения Param2<string, string>
- **Решение:** Убедитесь что используете CF.RPC.SendRPC()

---

*Последнее обновление: 2025-01-16*
*Источник: Community Framework v0.0.0 (Workshop ID: 1559212036)*
