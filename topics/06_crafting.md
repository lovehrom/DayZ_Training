# Crafting: Рецепты крафта

**Aliases:** crafting | крафт | рецепты | ActionCombination | CanCombine | combine
**Related:** [Items](topics/01_items.md)

---

## ⚡ 30-Second Summary

Crafting (крафтинг) в DayZ реализуется через:
1. **ActionCombination** - комбайнинг в руках (2 предмета → 1)
2. **Recipes** - рецепты через specialised workbench (крафт-станции)

Для простого крафта используйте ActionCombination.

---

## 🎯 5-Minute Solution: Создать рецепт крафта

### Вариант 1: Комбайнинг в руках

**Задача:** Мука + Вода = Тесто

```cpp
modded class ActionCombination
{
    override bool CanCombine(ItemBase item1, ItemBase item2)
    {
        // Проверить что это мука + вода
        if (item1.IsInherited(Flour) && item2.IsInherited(WaterBottle))
            return true;

        if (item1.IsInherited(WaterBottle) && item2.IsInherited(Flour))
            return true;

        return false;
    }

    override void OnCombine(ItemBase item1, ItemBase item2)
    {
        PlayerBase player = GetGame().GetPlayer();
        ItemBase dough = player.SpawnEntityOnGroundPos("Dough", player.GetPosition());

        // Удалить ингредиенты
        item1.Delete();
        item2.Delete();
    }
};
```

---

## 📚 Deep Dive

### Система крафта DayZ

**ActionCombination:**
- Комбайнинг в руках
- 2 предмета → 1 результат
- Выполняется игроком

**Recipes:**
- Через workbench (станции)
- Сложные рецепты
- Много ингредиентов

---

### ActionCombination методы

#### CanCombine() - Проверка возможности комбинации

```c
modded class ActionCombination
{
    override bool CanCombine(ItemBase item1, ItemBase item2)
    {
        // Проверить типы предметов
        if (item1.IsInherited(Item1_Type) && item2.IsInherited(Item2_Type))
            return true;

        // Проверить условия
        if (item1.GetQuantity() > 0 && item2.GetQuantity() > 0)
            return true;

        return false;
    }
};
```

#### OnCombine() - Логика комбинации

```c
modded class ActionCombination
{
    override void OnCombine(ItemBase item1, ItemBase item2)
    {
        PlayerBase player = GetGame().GetPlayer();

        // Создать результат
        ItemBase result = player.SpawnEntityOnGroundPos("ResultItem", player.GetPosition());

        // Удалить ингредиенты
        item1.Delete();
        item2.Delete();
    }
};
```

#### CombineRemaining() - Оставшиеся предметы

```c
override void CombineRemaining(ItemBase item1, ItemBase item2)
{
    // Оставшийся предмет после крафта
    // Например: пустая банка после консервов
    if (item1.IsInherited(Jar))
        item1.SetQuantity(0);  // Опустошить
    else
        item1.Delete();  // Или удалить
}
```

---

## 💡 Examples

### Example 1: Создать стрелу

**Дерево + Перо = Стрела**

```c
modded class ActionCombination
{
    override bool CanCombine(ItemBase item1, ItemBase item2)
    {
        if (item1.IsInherited(WoodenStick) && item2.IsInherited(ChickenFeather))
            return true;

        if (item1.IsInherited(ChickenFeather) && item2.IsInherited(WoodenStick))
            return true;

        return false;
    }

    override void OnCombine(ItemBase item1, ItemBase item2)
    {
        PlayerBase player = GetGame().GetPlayer();
        player.SpawnEntityOnGroundPos("Arrow", player.GetPosition());

        item1.Delete();
        item2.Delete();
    }
};
```

### Example 2: Рецепт через workbench

→ [examples/crafting/](../examples/crafting/)

---

## 🔗 Related Topics

→ [Items: Создание предметов](topics/01_items.md)

---

## 🔍 Common Tasks

### Как добавить проверку количества?

```c
override bool CanCombine(ItemBase item1, ItemBase item2)
{
    if (item1.GetQuantity() >= 1 && item2.GetQuantity() >= 1)
        return true;
    return false;
}
```

---

### Как создать крафт с несколькими результатами?

```c
override void OnCombine(ItemBase item1, ItemBase item2)
{
    PlayerBase player = GetGame().GetPlayer();
    vector pos = player.GetPosition();

    player.SpawnEntityOnGroundPos("Result1", pos);
    player.SpawnEntityOnGroundPos("Result2", pos);

    item1.Delete();
    item2.Delete();
}
```

---

### Как сохранить один из ингредиентов?

```c
override void CombineRemaining(ItemBase item1, ItemBase item2)
{
    // Сохранить item1 (не удалять)
    // item2 будет удалён автоматически
}
```

---

## 🚨 Troubleshooting

**Проблема:** Крафт не работает
- **Решение:** Проверьте `CanCombine()` возвращает `true`

**Проблема:** Предметы не удаляются
- **Решение:** Вызовите `.Delete()` для обоих предметов

---

## 📖 Additional Resources

- [DayZ-Samples/Test_Crafting](../samples/official/Test_Crafting/) - Примеры крафтинга

---

## 🎓 Best Practices

1. **Проверяйте** оба порядка предметов (item1+item2 И item2+item1)
2. **Проверяйте** `GetQuantity()` для стакаемых предметов
3. **Используйте** `OnCombine()` для создания результата
4. **Используйте** `CombineRemaining()` для оставшихся предметов
5. **Тестируйте** рецепт в игре
