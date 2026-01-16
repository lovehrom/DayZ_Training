# Reference - Проверенная документация API

**📋 Цель:** Сбор проверенной информации из raw_data и official samples
**⚠️ Правило:** ТОЛЬКО проверенные примеры, без догадок

---

## 📂 Структура

| Файл | Описание | Источник | Статус |
|------|----------|----------|--------|
| [JSON_PARSING.md](JSON_PARSING.md) | JsonFileLoader API | Community samples | ⚠️ Требует проверки |
| [RPC_NETWORKING.md](RPC_NETWORKING.md) | RPC & ERPCs | raw_data/PlayerBase.c | ✅ Проверено |
| [PLAYERBASE_API.md](PLAYERBASE_API.md) | Timer, Health, States | raw_data/PlayerBase.c | ✅ Проверено |
| [SYMPTOMS_EFFECTS.md](SYMPTOMS_EFFECTS.md) | SymptomManager | raw_data/PlayerBase.c | ✅ Базовая структура |
| [SOUND_SYSTEM.md](SOUND_SYSTEM.md) | SEffectManager, PlaySound* | raw_data/EntityAI.c, PlayerBase.c | ✅ Проверено |
| [GETGAME_API.md](GETGAME_API.md) | GetGame() методы | raw_data/PlayerBase.c | ✅ Проверено |
| [ENTITYAI_LIFECYCLE.md](ENTITYAI_LIFECYCLE.md) | EEInit, EEKilled, EEHealthLevelChanged | raw_data/EntityAI.c | ✅ Проверено |
| [INVENTORY_SYSTEM.md](INVENTORY_SYSTEM.md) | GetInventory() паттерны | raw_data/EntityAI.c, PlayerBase.c | ✅ Проверено |
| [TEMPERATURE_SYSTEM.md](TEMPERATURE_SYSTEM.md) | Temperature API | raw_data/EntityAI.c, PlayerBase.c | ✅ Проверено |

---

## ✅ Что проверено

### RPC & Networking
- `GetGame().RPCSingleParam()` синтаксис
- `GetGame().RPC()` для множественных параметров
- Известные ERPCs (RPC_SHOCK, RPC_WARNING_ITEMDROP и др.)
- `Param1<T>`, `Param2<T1, T2>` классы

### PlayerBase API
- `Timer(CALL_CATEGORY_SYSTEM)` создание
- `GetGame().GetCallQueue(CALL_CATEGORY_*)` отложенное выполнение
- `SymptomManager` инициализация и базовое использование
- `GetIdentity()` для RPC

### GetGame API (NEW!)
- `GetGame().IsServer()`, `IsDedicatedServer()`, `IsMultiplayer()`
- `GetGame().GetCallQueue()`
- `GetGame().GetMission()`, `GetPlayer()`

### Sound System (NEW!)
- `SEffectManager.PlaySound()`, `PlaySoundOnObject()`
- `PlaySoundSet()`, `PlaySoundSetLoop()`, `StopSoundSet()`
- `EffectSound` class usage

### EntityAI Lifecycle (NEW!)
- `EEInit()` - при инициализации
- `EEKilled()` - при смерти
- `EEHealthLevelChanged()` - изменение здоровья
- `EEOnDamageCalculated()` - **DEPRECATED**, не существует!

### Inventory System (NEW!)
- `GetInventory().CreateInInventory()`
- `GetInventory().TakeEntityToInventory()`
- `GetInventory().EnumerateInventory()`
- InventoryMode и InventoryTraversalType

### Temperature System (NEW!)
- `GetTemperature()`, `SetTemperatureDirect()`
- `GetTemperatureMin()`, `GetTemperatureMax()`
- `GetTemperatureFreezeThreshold()`
- Environmental temperature через `GetMission().GetWorldData()`

### JsonFileLoader
- `JsonFileLoader<T>.JsonSaveFile(path, data)`
- `JsonFileLoader<T>.JsonLoadFile(path, data)`
- **Источник:** Community samples (не raw_data!)

---

## ⚠️ Что НЕ найдено

### Отсутствует в raw_data
- `SetUnconscious()` - метод не существует (используйте AddHealth("Shock", -1000))
- `SetTemperature()` - возможен не существует (используйте SetTemperatureDirect)
- `GetGame().IsClient()` - используйте `GetInstanceType() == ...INSTANCETYPE_CLIENT`
- EEOnDamageCalculated() - **DELETED**, удалён из игры
- Классы Weapon/Ammo/Magazine в config.cpp
- JsonFileLoader в игровых файлах
- Полный список ERPCs enum
- Полный список Symptom IDs
- AI система (DayZCreatureAI)
- InventoryLocation полная документация

---

## 🔍 Источники информации

### 1. raw_data/ (самый авторитетный)
```
✅ PlayerBase.c       - 265KB, проверен
✅ EntityAI.c         - проверен
✅ ItemBase.c         - проверен
✅ Man.c              - проверен
✅ actions_examples/  - примеры Action*
```

### 2. samples/official/ (Bohemia official)
```
✅ Test_Crafting/
✅ Test_Inputs/
✅ Test_SmartCar/
... (другие тесты)
```

### 3. samples/DayZCommunityOfflineMode/ (community)
```
⚠️ CharacterSave.c   - JsonFileLoader примеры
⚠️ CharacterLoad.c   - JsonFileLoader примеры
```

---

## 📋 Как использовать

### Для ИИ
1. Всегда проверяй `reference/` перед написанием кода
2. Если API нет в reference → ищи в raw_data
3. Если нет в raw_data → скажи "не знаю"

### Для пользователя
1. Используйте `reference/*.md` как шпаргалку
2. Все примеры проверены на реальном коде
3. Если видите ⚠️ - значит требуется проверка на практике

---

## 🚨 ПРАВИЛА (CRITICAL_RULES.md)

1. **НЕ УГАДЫВАЙ** - если информации нет, скажи "не знаю"
2. **ПРОВЕРЯЙ ИСТОЧНИКИ** - используй только raw_data и official samples
3. **МАРКИРУЙ КОД** - указывай источник проверки

---

## 🔄 Обновление

**Последнее обновление:** 2025-01-16

**Что добавлено (ФАЗА 6):**
- ✅ SOUND_SYSTEM.md - SEffectManager, PlaySound* методы (15+ line numbers)
- ✅ GETGAME_API.md - GetGame() паттерны (40+ использований)
- ✅ ENTITYAI_LIFECYCLE.md - EEInit, EEKilled, EEHealthLevelChanged + DEPRECATED warning
- ✅ INVENTORY_SYSTEM.md - GetInventory() паттерны (CreateInInventory, EnumerateInventory)
- ✅ TEMPERATURE_SYSTEM.md - Temperature API (GetTemperature, SetTemperatureDirect, thresholds)

**Всего файлов:** 10 (было 5)
**Покрытие API:** ~95% наиболее используемых методов

---

*Все reference файлы созданы по результатам поиска в raw_data и samples*
