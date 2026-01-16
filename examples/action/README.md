# Action Example - Пример действия

## Что это

Пример создания кастомного действия (action) для игрока в DayZ. Action - это действие, которое игрок может выполнить через контекстное меню (Action Menu).

## Как использовать

1. **Скопировать мод:**
   - Создайте папку `@ActionExample/` рядом с `@DayZServer/`
   - Скопируйте файлы мода в `@ActionExample/`

2. **Запустить сервер:**
   - Добавьте параметр запуска: `-mod=@ActionExample`
   - Запустите сервер

3. **Проверить в игре:**
   - Зайдите на сервер
   - Откройте контекстное меню (действие по умолчанию: F/Scroll wheel)
   - Найдите действие "Wave Hello" (или своё действие)

## Файлы

- `config.cpp` - Конфигурация мода (регистрация action)
- `scripts/3_Game/Actions/ActionWaveHello.c` - Код действия

## Структура

```
@ActionExample/
├── config.cpp
└── scripts/
    └── 3_Game/
        └── Actions/
            └── ActionWaveHello.c
```

## Как это работает

### ActionWaveHello.c

```c
class ActionWaveHello: ActionSingleBase
{
    void ActionWaveHello()
    {
        // UID команды (определяет тип действия)
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACT;
        // В каких позициях можно использовать
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
    }

    override void CreateConditionComponents()
    {
        // Условия для предмета (CCINone = без предмета)
        m_ConditionItem = new CCINone;
        // Условия для цели (CCTNone = без цели)
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        // Условие выполнения действия
        return player.IsAlive() && !player.IsRestrained();
    }

    override void OnExecuteServer(ActionData action_data)
    {
        // Логика на сервере
    }

    override void OnExecuteClient(ActionData action_data)
    {
        // Визуальные эффекты на клиенте
    }
}
```

### config.cpp

```cpp
class CfgPatches
{
    class ActionExample
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {
            "DZ_Data",
            "DZ_Actions"
        };
    };
};

// Регистрация action в системе действий
class CfgActions
{
    class ActionWaveHello
    {
        // Параметры действия
    };
};
```

## Модификация для использования

### Добавить действие к предмету

```c
modded class MyItem extends ItemBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionWaveHello);  // Добавить действие
    }
};
```

### Добавить действие к игроку

```c
modded class PlayerBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionWaveHello);  // Добавить действие
    }
};
```

## Связанные темы

→ [../../topics/03_actions.md](../../topics/03_actions.md) - Создание действий
→ [../../topics/04_playerbase.md](../../topics/04_playerbase.md) - Модификация игрока

## Известные ограничения

- Требуется модификация PlayerBase или ItemBase для интеграции
- Анимация помахивания не реализована (только каркас)

## Следующие шаги

После изучения этого примера:
1. Добавьте анимацию через `OnExecuteClient()`
2. Создайте ActionContinuous (длительное действие)
3. Добавьте условия для предметов (CCINonRuined, etc.)

---
*Последнее обновление: 2025-01-16*
