# SymptomManager - Эффекты игрока

**✅ ПРОВЕРЕНО:** raw_data/PlayerBase.c
**Источник:** Распакованный PBO (игровые файлы)

---

## Что такое SymptomManager

Система для управления эффектами на игроке (боли, оглушения, слепота и т.д.)

---

## Инициализация

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:48, 383
class PlayerBase
{
    ref SymptomManager m_SymptomManager;

    void OnInit()
    {
        m_SymptomManager = new SymptomManager(this);
    }
}
```

---

## Известные методы

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:1067
GetSymptomManager().OnPlayerKilled();

// RPC методы (из контекста)
GetSymptomManager().OnRPC(ERPCs.RPC_PLAYER_SYMPTOM_ON, ctx);
GetSymptomManager().OnRPC(ERPCs.RPC_PLAYER_SYMPTOM_OFF, ctx);
```

---

## Известные Symptom RPCs

```c
// ✅ ПРОВЕРЕНО по контексту
ERPCs.RPC_PLAYER_SYMPTOM_ON     // Включить симптом
ERPCs.RPC_PLAYER_SYMPTOM_OFF    // Выключить симптом
```

---

## Возможные симптомы (НЕ ПРОВЕРЕНО)

⚠️ **ВНИМАНИЕ:** Это непроверенный список! Требует поиска в raw_data.

```
Blindness       - Слепота
Deafness        - Глухота
Pain            - Боль
Unconsciousness - Бессознательное состояние
Tremor          - Трясись рук
...
```

---

## Template

```c
// Server → Client: Включить эффект
void ApplySymptom(PlayerBase player)
{
    Param1<int> symptomParam = new Param1<int>(SYMPTOM_ID);
    GetGame().RPCSingleParam(player, ERPCs.RPC_PLAYER_SYMPTOM_ON, symptomParam, true, player.GetIdentity());
}

// Client: Обработать RPC (в SymptomManager)
void OnRPC(ERPCs rpc, ParamsReadContext ctx)
{
    if (rpc == ERPCs.RPC_PLAYER_SYMPTOM_ON)
    {
        Param1<int> symptomParam = new Param1<int>(0);
        if (ctx.Read(symptomParam))
        {
            ActivateSymptom(symptomParam.param1);
        }
    }
}
```

---

## Источники

- **raw_data/PlayerBase.c** (строки 48, 383, 1067)

---

## ⚠️ Что НЕ найдено

- Полный список Symptom IDs
- Методы `ActivateSymptom()`, `DeactivateSymptom()`
- Официальная документация по SymptomManager
- Список всех возможных симптомов

---

*Последнее обновление: 2025-01-16*
*Статус: Базовая структура найдена, требуется углублённый поиск*
