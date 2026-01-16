# Crafting Example - Пример крафта

## Что это

Пример создания системы крафта (crafting) в DayZ. Показывает два подхода:
1. **ActionContinuousBase** - длительное действие (крафт камней)
2. **ActionCombinationBase** - комбинирование предметов (крафт верёвки)

## Как использовать

1. **Скопировать мод:**
   - Создайте папку `@CraftingExample/` рядом с `@DayZServer/`
   - Скопируйте файлы мода в `@CraftingExample/`

2. **Запустить сервер:**
   - Добавьте параметр запуска: `-mod=@CraftingExample`
   - Запустите сервер

3. **Проверить в игре:**
   - Зайдите на сервер
   - **Крафт камней:** используйте 2x SmallStone → крафт Stone
   - **Крафт верёвки:** комбинируйте 2x Rag → крафт Rope_Custom

## Файлы

- `config.cpp` - Конфигурация мода (определение предметов)
- `scripts/3_Game/Actions/ActionCraftStone.c` - Крафт камней
- `scripts/3_Game/Actions/ActionCraftRope.c` - Крафт верёвки

## Структура

```
@CraftingExample/
├── config.cpp
└── scripts/
    └── 3_Game/
        └── Actions/
            ├── ActionCraftStone.c
            └── ActionCraftRope.c
```

## Как это работает

### Подход 1: ActionContinuousBase (крафт камней)

```c
modded class ActionCraftStone: ActionContinuousBase
{
    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        // Проверка ингредиентов
        int stoneCount = player.CountItemType("SmallStone");
        if (stoneCount < 2)
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;

        // Удалить ингредиенты
        player.RemoveItemOfType("SmallStone", 2);

        // Создать результат
        player.GetInventory().CreateInInventory("Stone");
    }
};
```

### Подход 2: ActionCombinationBase (крафт верёвки)

```c
modded class ActionCraftRopeCustom: ActionCombinationBase
{
    override bool CanCombine(ItemBase item1, ItemBase item2)
    {
        // Проверка: оба предмета - лоскуты ткани
        return item1.GetType() == "Rag" && item2.GetType() == "Rag";
    }

    override void OnExecuteServer(ActionData action_data)
    {
        // Удалить ингредиенты
        action_data.m_MainItem.Delete();
        action_data.m_TargetItem.Delete();

        // Создать результат
        player.GetInventory().CreateInInventory("Rope_Custom");
    }
};
```

## Разница между подходами

| Подход | Когда использовать | Пример |
|--------|-------------------|--------|
| **ActionContinuousBase** | Крафт из инвентаря (без выбора цели) | 2x Stone → 1x BigStone |
| **ActionCombinationBase** | Комбинирование 2 предметов (перетаскивание) | Rag + Rag → Rope |

## Основные методы

### ActionCondition()
Проверка условий выполнения крафта:
```c
override bool ActionCondition(PlayerBase player, ActionTarget target)
{
    // Проверить ингредиенты
    // Проверить состояние игрока
    return true;  // Разрешить крафт
}
```

### CanCombine() (только для ActionCombinationBase)
Проверка возможности комбинирования:
```c
override bool CanCombine(ItemBase item1, ItemBase item2)
{
    // Проверить типы предметов
    return item1.GetType() == "Item1" && item2.GetType() == "Item2";
}
```

### OnExecuteServer()
Логика крафта на сервере:
```c
override void OnExecuteServer(ActionData action_data)
{
    // 1. Удалить ингредиенты
    // 2. Создать результат
    // 3. Сообщение игроку
}
```

## Модификация рецептов

### Изменить ингредиенты

**ActionContinuousBase:**
```c
int ingredientCount = player.CountItemType("MyIngredient");
if (ingredientCount < 3)  // Нужно 3 ингредиента
    return false;
```

**ActionCombinationBase:**
```c
return item1.GetType() == "Ingredient1" && item2.GetType() == "Ingredient2";
```

### Изменить результат

```c
// Создать другой предмет
player.GetInventory().CreateInInventory("MyResultItem");

// Создать несколько предметов
player.GetInventory().CreateInInventory("Result1");
player.GetInventory().CreateInInventory("Result2");
```

### Добавить звук

```c
void ActionCraftStone()
{
    m_Sound = "craft_universal_0";  // Звук крафта
}
```

## Связанные темы

→ [../../topics/06_crafting.md](../../topics/06_crafting.md) - Система крафта
→ [../../topics/03_actions.md](../../topics/03_actions.md) - Действия игрока
→ [../action/](../action/) - Пример действия

## Известные ограничения

- Не реализована проверка количества предметов для ActionCombinationBase
- Не добавлена прогресс-бар для длительного крафта
- Используются заглушки моделей

## Следующие шаги

После изучения этого примера:
1. Добавьте прогресс-бар (m_CompleteActionTime)
2. Создайте сложный рецепт (3+ ингредиента)
3. Добавьте проверку инструментов (нож, молоток и т.д.)

---
*Последнее обновление: 2025-01-16*
