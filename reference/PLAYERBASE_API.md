# PlayerBase API - Ключевые методы и системы

**✅ ПРОВЕРЕНО:** raw_data/PlayerBase.c
**Источник:** Распакованный PBO (игровые файлы)

---

## Health & States

### SetUnconscious

**⚠️ МЕТОД НЕ СУЩЕСТВУЕТ**

В raw_data/PlayerBase.c метод `SetUnconscious()` **НЕ найден**.

**Используемое решение:**
```c
// ⚠️ КОСТЫЛЬ: Установить бессознательное состояние через шок
player.AddHealth("Shock", "Health", -1000);  // Shock >= 0 = unconscious
```

**Примечание:** Это не официальный метод, а обходной путь.

---

## Timer System

### Создание Timer

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:359
ref Timer m_AnalyticsTimer;
m_AnalyticsTimer = new Timer(CALL_CATEGORY_SYSTEM);
```

### Timer Categories

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c
CALL_CATEGORY_SYSTEM    // 359 - Системный таймер
CALL_CATEGORY_GAMEPLAY  // Из контекста
CALL_CATEGORY_GUI       // 1356 - GUI операции
```

### CallQueue (отложенное выполнение)

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:551, 1356
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.OnPlayerLoaded);
GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateCorpseStateVisual, 200, false);
```

**Параметры CallLater:**
- `function` - функция для вызова
- `delay` - задержка в мс
- `repeat` - повторять ли (`true`/`false`)

---

## SymptomManager

### Инициализация

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:48, 383
ref SymptomManager m_SymptomManager;

// В OnInit():
m_SymptomManager = new SymptomManager(this);
```

### Использование

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:1067
GetSymptomManager().OnPlayerKilled();

// RPC примеры:
GetSymptomManager().OnRPC(ERPCs.RPC_PLAYER_SYMPTOM_ON, ctx);
GetSymptomManager().OnRPC(ERPCs.RPC_PLAYER_SYMPTOM_OFF, ctx);
```

---

## Misc Methods

### OnPlayerLoaded

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:551
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.OnPlayerLoaded);
```

### UpdateCorpseStateVisual

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:1356
GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateCorpseStateVisual, 200, false);
```

---

## Полезные поля PlayerBase

```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c:46-50
ref Environment              m_Environment;
ref EmoteManager             m_EmoteManager;
ref SymptomManager           m_SymptomManager;
ref VirtualHud               m_VirtualHud;
ref StaminaHandler           m_StaminaHandler;
```

---

## Источники

- **raw_data/PlayerBase.c** (строки 48, 359, 383, 551, 1067, 1356)

---

## ⚠️ Что НЕ найдено

- `SetUnconscious()` - метод не существует в raw_data
- Полный список SymptomManager API
- Официальная документация по Timer категориям

---

*Последнее обновление: 2025-01-16*
*Статус: Проверено на raw_data*
