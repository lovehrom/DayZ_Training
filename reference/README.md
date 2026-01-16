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

### JsonFileLoader
- `JsonFileLoader<T>.JsonSaveFile(path, data)`
- `JsonFileLoader<T>.JsonLoadFile(path, data)`
- **Источник:** Community samples (не raw_data!)

---

## ⚠️ Что НЕ найдено

### Отсутствует в raw_data
- `SetUnconscious()` - метод не существует
- Классы Weapon/Ammo/Magazine в config.cpp
- JsonFileLoader в игровых файлах
- Полный список ERPCs enum
- Полный список Symptom IDs
- Sound API (PlaySound, SoundSet)
- AI система (DayZCreatureAI)

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

**Что добавлено:**
- RPC & Networking (из PlayerBase.c)
- Timer & CallQueue (из PlayerBase.c)
- SymptomManager базовый (из PlayerBase.c)
- JsonFileLoader (из community samples)

**Что требуется:**
- Полный поиск оружия/патронов (может быть в других PBO)
- AI система поиск
- Sound API поиск
- Deprecated API список

---

*Все reference файлы созданы по результатам поиска в raw_data и samples*
