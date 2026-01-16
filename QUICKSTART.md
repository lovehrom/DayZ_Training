# Quick Start - Быстрый старт за 3 минуты

**Хотите создать мод DayZ? Начните здесь.**

---

## ⚡ Вариант 1: Самый быстрый старт (1 минута)

### Готовый мод из примеров

**Цель:** Скопировать готовый пример и проверить его работу

**Шаг 1:** Скопируйте пример
```
K:\Работа\Dayz_WorckPlace\DayZ_Training\examples\edible_item\
↓
K:\SteamLibrary\steamapps\common\DayZ\@EdibleItem\
```

**Шаг 2:** Запустите DayZ с параметром:
```
-mission=.\Missions\DayZCommunityOfflineMode.ChernarusPlus -mod=@EdibleItem
```

**Шаг 3:** В игре (консоль `/`):
```
_test = "ApplePie"_spawn
```

**Готово!** Предмет появился. Поднимите и съешьте его.

---

## 🍎 Вариант 2: Создать свой предмет (3 минуты)

### Создание edible предмета

**Шаг 1: Создать структуру папок**
```
@MyFoodMod/
├── config.cpp
└── scripts/
    └── 3_Game/
        └── Items/
            └── MyFoodItem.c
```

**Шаг 2: Создать config.cpp**
```cpp
class CfgPatches
{
    class MyFoodMod
    {
        units[] = {"MyFoodItem"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Items"};
    };
};

class CfgVehicles
{
    class Edible_Base;
    class MyFoodItem: Edible_Base
    {
        scope = 2;
        displayName = "My Food";
        description = "My custom food item";
        model = "\DZ\items\apple.p3d";

        weight = 100;
        itemSize[] = {1, 1};

        energy = 100;
        water = 50;
        nutritionalValue = 2;

        nominal = 100;
        min = 50;
        max = 150;
        restock = 0;
        lifetime = 7200;
        quantity = 1;
    };
};
```

**Шаг 3: Создать MyFoodItem.c**
```c
class MyFoodItem extends Edible_Base
{
    override void OnInit()
    {
        super.OnInit();
    }

    override void OnConsume(PlayerBase player)
    {
        super.OnConsume(player);
        Print("MyFoodItem consumed by " + player);
    }
}
```

**Шаг 4:** Скопировать мод в игру
```
@MyFoodMod/ → K:\SteamLibrary\steamapps\common\DayZ\@MyFoodMod\
```

**Шаг 5:** Запустить DayZ:
```
-mission=.\Missions\DayZCommunityOfflineMode.ChernarusPlus -mod=@MyFoodMod
```

**Шаг 6:** Протестировать
```
_test = "MyFoodItem"_spawn
```

---

## 🔧 Вариант 3: Добавить действие к предмету (5 минут)

### Создать custom action

**Шаг 1-4:** Как в Варианте 2, но добавьте:

**Дополнительный файл:** `scripts/3_Game/Actions/ActionMyAction.c`
```c
class ActionMyAction: ActionSingleBase
{
    void ActionMyAction()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACT;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone;
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        return player.IsAlive();
    }

    override void OnExecuteServer(ActionData action_data)
    {
        Print("Action executed!");
    }
}
```

**Модифицировать MyFoodItem.c:**
```c
class MyFoodItem extends Edible_Base
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionMyAction);
    }
}
```

---

## 🎯 Что дальше?

**Хотите глубже понять?**
→ [topics/01_items.md](topics/01_items.md) - Теория предметов

**Нужно больше примеров?**
→ [examples/](examples/) - Полные рабочие моды

**Есть ошибка?**
→ [troubleshooting/common_errors.md](troubleshooting/common_errors.md)

**Хотите настроить окружение разработки?**
→ [tools/WORKBENCH_SETUP.md](tools/WORKBENCH_SETUP.md) - Workbench + hot-reload

---

## 📋 Чеклист готовности

Перед началом убедитесь:

- [ ] DayZ установлен: `K:\SteamLibrary\steamapps\common\DayZ`
- [ ] Offline Mode установлен: `K:\SteamLibrary\steamapps\common\DayZ\Missions\`
- [ ] Путь к игре правильный (проверьте свой путь!)

**Нет Offline Mode?**
→ [tools/OFFLINE_MODE.md](tools/OFFLINE_MODE.md) - Инструкция по установке

---

## 💡 Советы

1. **Начните с готовых примеров** - быстрее и надёжнее
2. **Используйте Offline Mode** - не нужен сервер для тестирования
3. **Проверяйте логи** - `%localappdata%\DayZ\script.log`
4. **Используйте заглушки моделей** - `\DZ\items\apple.p3d` для тестов

---

## 🚨 Распространённые ошибки

**"Class not found"**
→ Проверьте что класс есть в config.cpp
→ [troubleshooting/common_errors.md#class-not-found](troubleshooting/common_errors.md#class-not-found)

**"Предмет не спавнится"**
→ Проверьте `scope = 2` в config.cpp
→ [troubleshooting/common_errors.md#item-not-appear](troubleshooting/common_errors.md#item-not-appear)

**"Мод не загружается"**
→ Проверьте параметр `-mod=@MyMod`
→ Проверьте логи на ошибки

---

**Время создания первого мода:** 3-5 минут
**Сложность:** ⭐☆☆☆☆ (очень простая)

**Следующий уровень:** [tools/WORKBENCH_SETUP.md](tools/WORKBENCH_SETUP.md) - профессиональная разработка
