# Actions: Действия игрока

**Aliases:** action | действие | ActionBase | ActionSingle | player action | создать действие
**Related:** [PlayerBase](topics/04_playerbase.md)
**Examples:** [Custom Action](../examples/action/)

---

## ⚡ 30-Second Summary

Actions (действия) - это то, что игрок может сделать через контекстное меню (Mouse2).

Чтобы создать кастомное действие:
1. Создать класс, наследуемый от `ActionBase` (или `ActionSingle`, `ActionContinuous`)
2. Переопределить методы (`CreateConditionComponents`, `ActionCondition`)
3. Зарегистрировать действие в модифицированном классе предмета/игрока

---

## 🎯 5-Minute Solution: Создать простое действие

**Файл:** `scripts/3_Game/Actions/ActionWaveHello.c`

```c
class ActionWaveHello: ActionSingleBase
{
    void ActionWaveHello()
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
        return player.IsAlive();  // Только живые игроки
    }

    override void OnExecuteServer(ActionData action_data)
    {
        player = action_data.m_Player;
        // Логика действия на сервере
    }
};
```

**Зарегистрировать действие:**

```c
modded class PlayerBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionWaveHello);
    }
};
```

---

## 📚 Deep Dive

### Иерархия Action классов

```
ActionBase (базовый класс для всех действий)
├── ActionSingleBase (действие, выполняемое один раз)
│   ├── ActionSingle
│   └── ... (многие другие)
├── ActionContinuousBase (действие, выполняемое пока удерживается)
│   ├── ActionContinuous
│   └── ...
└── ... (другие типы)
```

---

### Типы Actions

| Тип | Описание | Когда использовать |
|-----|----------|-------------------|
| `ActionSingleBase` | Действие выполняется один раз при нажатии | Помахать, открыть дверь |
| `ActionContinuousBase` | Действие выполняется пока удерживается кнопка | Пить из реки, есть |
| `ActionBase` | Базовый класс (редко используется напрямую) | - |

---

### Ключевые методы Action

#### CreateConditionComponents()

**Что делает:** Определяет условия выполнения действия

```c
override void CreateConditionComponents()
{
    // Условия для предмета в руках
    m_ConditionItem = new CCINone;  // Без условий
    // ИЛИ
    m_ConditionItem = new CCINonRuined;  // Предмет не сломан
    // ИЛИ
    m_ConditionItem = new CCINonRuinedAndHasQuantity;  // Не сломан + имеет количество

    // Условия для цели (куда смотрит игрок)
    m_ConditionTarget = new CCTNone;  // Без условий
    // ИЛИ
    m_ConditionTarget = new CCTObject;  // Цель - объект
    // ИЛИ
    m_ConditionTarget = new CCTMan;  // Цель - человек
}
```

**Варианты ConditionItem:**
- `CCINone` - без условий
- `CCINonRuined` - предмет не сломан
- `CCINonRuinedAndHasQuantity` - не сломан + имеет количество
- `CCIItemPresent` - предмет присутствует
- И многие другие...

**Варианты ConditionTarget:**
- `CCTNone` - без условий
- `CCTObject` - цель = объект в мире
- `CCTMan` - цель = человек (игрок/зомби)
- `CCTBase` - базовая цель
- И многие другие...

---

#### ActionCondition()

**Что делает:** Дополнительная проверка условий (код)

```c
override bool ActionCondition(PlayerBase player, ActionTarget target)
{
    // Проверка условий
    if (!player.IsAlive()) return false;  // Игрок должен быть жив
    if (player.IsRestrained()) return false;  // Не связан
    if (player.IsUnconscious()) return false;  // Не в обмороке

    // Дополнительные проверки
    Object targetObject = target.GetObject();
    if (!targetObject) return false;  // Цель должна существовать

    return true;  // Все условия выполнены
}
```

---

#### OnExecuteServer() / OnExecuteClient()

**Что делает:** Логика выполнения действия

```c
// Выполняется на сервере
override void OnExecuteServer(ActionData action_data)
{
    PlayerBase player = action_data.m_Player;
    Object targetObject = action_data.m_Target.GetObject();

    // Логика на сервере
    player.GetStatEnergy().Add(100);  // Восстановить энергию
}

// Выполняется на клиенте (визуальные эффекты)
override void OnExecuteClient(ActionData action_data)
{
    // Визуальные эффекты на клиенте
    // Звуки, частицы, и т.д.
}
```

---

### ActionData - данные действия

```c
override void OnExecuteServer(ActionData action_data)
{
    // Игрок
    PlayerBase player = action_data.m_Player;

    // Предмет в руках (если есть)
    ItemBase item = action_data.m_MainItem;

    // Цель (куда смотрит игрок)
    ActionTarget target = action_data.m_Target;
    Object targetObject = target.GetObject();

    // Дополнительные данные
    // ...
}
```

---

## 💡 Examples

### Example 1: Простое действие (помахать)

→ [examples/action/](../examples/action/)

### Example 2: Непрерывное действие (пить из реки)

```c
class ActionDrinkFromPond: ActionContinuousBase
{
    void ActionDrinkFromPond()
    {
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone;
        m_ConditionTarget = new CCTWaterSurface;  // Цель = вода
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        return player.IsAlive() && !player.IsRestrained();
    }

    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;
        player.GetStatWater().Add(50);  // Восстановить воду
    }
};
```

### Example 3: Действие с предметом (использовать бинт)

```c
class ActionUseBandage: ActionSingleBase
{
    void ActionUseBandage()
    {
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINonRuined;  // Бинт не сломан
        m_ConditionTarget = new CCTMan;  // Цель = человек
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        if (!player.IsAlive()) return false;

        // Проверить что в руках бинт
        ItemBase item = player.GetItemInHands();
        if (!item || !item.IsInherited(Bandage_Dressing)) return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;
        PlayerBase target = PlayerBase.Cast(action_data.m_Target.GetObject());

        // Использовать бинт
        player.GetItemInHands().AddQuantity(-1);

        // Вылечить цель
        target.m_ModifiersManager.ActivateModifier("Modifiers_bleeding");  // Убрать кровотечение
    }
};
```

---

## 🔗 Related Topics

→ [PlayerBase: Модификация игрока](topics/04_playerbase.md)
→ [Items: Создание предметов](topics/01_items.md)

---

## 🔍 Common Tasks

### Как добавить действие в контекстное меню предмета?

```c
modded class MyItem extends ItemBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionEat);  // Добавить действие "Есть"
        AddAction(ActionCraftItem);  // Добавить действие "Крафт"
        AddAction(ActionToggleBodyHeat);  // Добавить действие "Согреться"
    }
};
```

---

### Как создать действие только для живых игроков?

```c
override bool ActionCondition(PlayerBase player, ActionTarget target)
{
    return player.IsAlive();  // Только живые
}
```

---

### Как добавить звук к действию?

```c
override void OnExecuteClient(ActionData action_data)
{
    // Проиграть звук на клиенте
    EffectSound sound = SEffectManager.PlaySoundOnObject("My_SoundSet", action_data.m_Player);
}
```

---

### Как добавить анимацию к действию?

```c
class ActionMyAction: ActionSingleBase
{
    void ActionMyAction()
    {
        // UID анимации (из DayZ animations)
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACT;
    }
};
```

---

## 🚨 Troubleshooting

**Проблема:** Действие не появляется в меню
- **Решение:** Проверьте `ActionCondition()` - возможно возвращает `false`

**Проблема:** Действие появляется, но не работает
- **Решение:** Проверьте логику в `OnExecuteServer()` - возможно ошибка в коде

**Подробнее:** → [troubleshooting/common_errors.md](../troubleshooting/common_errors.md)

---

## 📖 Additional Resources

- [raw_data/actions_examples/](../raw_data/actions_examples/) - Примеры Action* классов из игры
- [DayZ-Samples/Test_Inputs](../samples/official/Test_Inputs/) - Примеры действий

---

## 🎓 Best Practices

1. **Используйте** `ActionSingleBase` для быстрых действий
2. **Используйте** `ActionContinuousBase` для длительных действий
3. **Проверяйте** условия в `ActionCondition()` (не в `CreateConditionComponents`)
4. **Разделяйте** логику на серверную и клиентскую
5. **Используйте** `modded class` для добавления действий к существующим предметам
