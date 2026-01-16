# RPC & Networking - Remote Procedure Calls

**✅ ПРОВЕРЕНО:** raw_data/PlayerBase.c
**Источник:** Распакованный PBO (игровые файлы)

---

## Синтаксис RPC

### Single Parameter RPC

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:3753
GetGame().RPCSingleParam(target, ERPCs.RPC_TYPE, param, doBroadcast, GetIdentity());

// Примеры из PlayerBase.c:
GetGame().RPCSingleParam(this, ERPCs.RPC_SHOCK, damage, true, GetIdentity());                    // 3753
GetGame().RPCSingleParam(this, ERPCs.DIAG_MISC_ENVIRONMENT_DEBUG_DATA, envDebugData, false, GetIdentity()); // 4310
GetGame().RPCSingleParam(this, ERPCs.RPC_WARNING_ITEMDROP, null, true, identity);                // 5906
```

### Multi Parameter RPC

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:5891
GetGame().RPC(target, ERPCs.RPC_TYPE, params, doBroadcast, identity);

// Пример:
GetGame().RPC(this, ERPCs.RPC_INIT_SET_QUICKBAR, params, true, identity); // 5891
```

---

## Параметры

| Параметр | Тип | Описание |
|----------|-----|----------|
| target | `Object` | Кому отправляем (`this` = себе) |
| rpc_type | `ERPCs` | Тип RPC (см. список ниже) |
| param/params | `Param` | Данные для передачи |
| doBroadcast | `bool` | `true` = всем, `false` = конкретному игроку |
| identity | `PlayerIdentity` | `GetIdentity()` для текущего игрока |

---

## Известные ERPCs

**✅ ПРОВЕРЕНО:** raw_data/PlayerBase.c

```c
// Игроковые RPCs
ERPCs.RPC_SHOCK                            // 3753 - Урон/шок
ERPCs.RPC_USER_ACTION_MESSAGE              // Из контекста
ERPCs.RPC_PLAYER_SYMPTOM_ON                // SymptomManager
ERPCs.RPC_INIT_SET_QUICKBAR                // 5891 - Инициализация хотбара
ERPCs.RPC_WARNING_ITEMDROP                 // 5906 - Предупреждение о дропе предмета

// Debug RPCs
ERPCs.DIAG_MISC_ENVIRONMENT_DEBUG_DATA     // 4310 - Debug данные окружения
ERPCs.DIAG_MISC_FALLDAMAGE_DEBUG_DATA      // 4320 - Debug данные падения
```

---

## Param Классы

```c
// Single parameter
Param1<string>(msg)
Param1<float>(dmg)

// Multi parameter (пример из контекста)
params = new Param2<int, int>(slotIndex, quickbarEntityID);
```

---

## Template

### Server → Client RPC

```c
// На сервере
void SendDamageToClient(PlayerBase player, float damage)
{
    Param1<float> dmgParam = new Param1<float>(damage);
    GetGame().RPCSingleParam(player, ERPCs.RPC_SHOCK, dmgParam, true, player.GetIdentity());
}
```

### Client → Server RPC

```c
// На клиенте
void RequestAction()
{
    Param1<string> msg = new Param1<string>("action_request");
    GetGame().RPCSingleParam(this, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, GetIdentity());
}
```

---

## GetIdentity()

```c
// ✅ ПРОВЕРЕНО: Используется во всех RPC вызовах в PlayerBase.c
PlayerIdentity identity = GetIdentity();
GetGame().RPCSingleParam(this, ERPCs.RPC_TYPE, param, true, GetIdentity());
```

---

## Источники

- **raw_data/PlayerBase.c** (строки 3753, 4310, 4320, 5891, 5906)

---

## ⚠️ Что НЕ найдено

- Полный список всех ERPCs enum значений
- Официальная документация по RPC системе
- Примеры custom RPCs (только встроенные)

---

*Последнее обновление: 2025-01-16*
*Статус: Проверено на raw_data*
