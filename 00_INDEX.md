# DayZ Modding Knowledge Base

## ⚡ Quick Start (30 seconds)

**Хотите создать мод?** → [Начните здесь](topics/01_items.md)

**У вас есть ошибка?** → [Ищите решение](troubleshooting/common_errors.md)

**Нужно понять систему?** → [Карта тем](#all-topics)

---

## 🛠️ Tools & Setup

| Задача | Где найти | Время |
|--------|-----------|-------|
| **Быстрый старт (3 мин)** | [QUICKSTART.md](QUICKSTART.md) | 3 min ⭐ |
| **Диагностика системы** | [DIAGNOSTICS.md](DIAGNOSTICS.md) | 2 min |
| **Тестирование модов** | [TESTING.md](TESTING.md) | 2 min |
| **Готовый шаблон** | [TEMPLATE_MOD/](TEMPLATE_MOD/) | 1 min |
| **Workbench настройка** | [tools/WORKBENCH_SETUP.md](tools/WORKBENCH_SETUP.md) | 15 min |
| **Офлайн тестирование** | [tools/OFFLINE_MODE.md](tools/OFFLINE_MODE.md) | 5 min |
| Создать предмет | [topics/01_items.md](topics/01_items.md) | 5 min |
| Настроить config.cpp | [topics/02_config.md](topics/02_config.md) | 3 min |
| Добавить действие | [topics/03_actions.md](topics/03_actions.md) | 10 min |
| Настроить экономику | [topics/05_economy.md](topics/05_economy.md) | 5 min |
| Создать рецепт | [topics/06_crafting.md](topics/06_crafting.md) | 10 min |

---

## 📋 All Topics

### Core (80% задач)
1. [Items: Создание предметов](topics/01_items.md)
   - Edible, Consumable, Clothing, Weapon
   - Properties, slots, attachments

2. [Config: Конфигурация](topics/02_config.md)
   - CfgPatches, CfgVehicles
   - Наследование классов
   - Models, animations

3. [Economy: Настройка спавна](topics/05_economy.md)
   - types.xml структура
   - Nominal, min, max
   - Spawn zones

4. [Crafting: Рецепты крафта](topics/06_crafting.md)
   - ActionCombination
   - CanCombine, recipes

### Intermediate (15% задач)
5. [Actions: Действия игрока](topics/03_actions.md)
   - ActionBase, ActionSingle
   - CreateActionComponent
   - ActionCondition

6. [PlayerBase: Модификация игрока](topics/04_playerbase.md)
   - modded class
   - OnInit, OnUpdate
   - Health, states

7. [Events: ScriptInvoker](topics/07_events.md)
   - Создание событий
   - OnPlayerSpawned, etc.

### Expert (5% задач)
8. [Network: RPC и синхронизация](topics/08_network.md)
   - Server/Client RPC
   - Synchronized variables

9. [UI: Интерфейс и HUD](topics/09_ui.md)
   - Виджеты
   - Layouts

10. [AI: Зомби и животные](topics/10_ai.md)
    - ZombieBase, AnimalBase
    - AI поведения

---

## 💡 Examples

Полные рабочие моды:
- [examples/minimal_item/](examples/minimal_item/) - Минимальный предмет
- [examples/edible_item/](examples/edible_item/) - Еда
- [examples/consumable_item/](examples/consumable_item/) - Расходник
- [examples/clothing_item/](examples/clothing_item/) - Одежда
- [examples/weapon/](examples/weapon/) - Оружие ⭐ НОВЫЙ
- [examples/crafting/](examples/crafting/) - Крафтинг
- [examples/action/](examples/action/) - Кастомное действие

**Шаблоны:**
- [TEMPLATE_MOD/](TEMPLATE_MOD/) - Готовый шаблон для вашего мода ⭐ НОВЫЙ

---

## 🔧 Troubleshooting & Help

**Диагностика:**
- [DIAGNOSTICS.md](DIAGNOSTICS.md) - Проверка готовности системы ⭐ НОВЫЙ
- [TESTING.md](TESTING.md) - Быстрое тестирование модов ⭐ НОВЫЙ

**Решение проблем:**
- [FAQ.md](FAQ.md) - Частые вопросы ⭐ НОВЫЙ
- [troubleshooting/common_errors.md](troubleshooting/common_errors.md) - Типичные ошибки
- [troubleshooting/debug_checklist.md](troubleshooting/debug_checklist.md) - Чеклист отладки
- [troubleshooting/performance.md](troubleshooting/performance.md) - Оптимизация

**Справка:**
- [GLOSSARY.md](GLOSSARY.md) - Словарь терминов ⭐ НОВЫЙ

---

## 🔍 Search Guide

→ [Как искать информацию](01_SEARCH_GUIDE.md)

**Быстрый поиск:**
1. Нажмите Ctrl+F
2. Введите английский ИЛИ русский термин
3. Используйте якорные ссылки (#heading)

**Примеры поиска:**
- "создать предмет" → topics/01_items.md
- "быстрый старт" → QUICKSTART.md
- "диагностика" → DIAGNOSTICS.md
- "тестирование" → TESTING.md
- "Class not found" → troubleshooting/common_errors.md#class-not-found
- "RPC" → topics/08_network.md
- "offline mode" → tools/OFFLINE_MODE.md
- "workbench" → tools/WORKBENCH_SETUP.md
- "глоссарий" → GLOSSARY.md
- "FAQ" → FAQ.md

---

## 📦 Raw Data

Сырые файлы из игры (для reference):
- [raw_data/PATTERNS_ANALYSIS.md](raw_data/PATTERNS_ANALYSIS.md) - **Анализ паттернов** ⭐
- [raw_data/game_config.cpp](raw_data/game_config.cpp) - Полный config
- [raw_data/EntityAI.c](raw_data/EntityAI.c) - Базовый класс
- [raw_data/ItemBase.c](raw_data/ItemBase.c) - Базовый предмет
- [raw_data/PlayerBase.c](raw_data/PlayerBase.c) - Игрок
- [raw_data/Man.c](raw_data/Man.c) - Человек
- [raw_data/actions_examples/](raw_data/actions_examples/) - Action* примеры

## 🔧 External Samples

Дополнительные примеры и инструменты:
- [samples/DayZCommunityOfflineMode/](samples/DayZCommunityOfflineMode/) - Офлайн режим для тестирования
- [samples/ZenTemplate/](samples/ZenTemplate/) - Шаблон мода с Workbench setup
- [samples/official/](samples/official/) - Официальные DayZ-Samples от Bohemia

---

*Последнее обновление: 2025-01-16*
*Версия DayZ: 1.26 (estimated)*
