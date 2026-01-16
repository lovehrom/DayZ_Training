# Template Mod - Готовый шаблон мода

**Что это:** Полностью готовый шаблон для быстрого создания вашего мода.

---

## ⚡ Как использовать (3 шага)

### Шаг 1: Скопировать шаблон

```
Откуда: K:\Работа\Dayz_WorckPlace\DayZ_Training\TEMPLATE_MOD\
Куда:   K:\SteamLibrary\steamapps\common\DayZ\@MyMod\
```

### Шаг 2: Заменить названия

**В config.cpp:**
- `TEMPLATE_MOD` → `MyMod`
- `TemplateItem` → `MyItem`

**В scripts/3_Game/Items/TemplateItem.c:**
- `TemplateItem` → `MyItem`
- Выберите базовый класс: `ItemBase`, `Edible_Base`, `Clothing`, etc.

### Шаг 3: Запустить и протестировать

```
-mission=.\Missions\DayZCommunityOfflineMode.ChernarusPlus -mod=@MyMod
```

В игре:
```
_test = "MyItem"_spawn
```

---

## 📂 Структура

```
TEMPLATE_MOD/
├── config.cpp          # Конфигурация (замените TEMPLATE_MOD)
└── scripts/
    └── 3_Game/
        └── Items/
            └── TemplateItem.c  # Код предмета (замените TemplateItem)
```

---

## 🎯 Что дальше?

### Добавить функционал

**Edible предмет:**
```c
class MyFood extends Edible_Base
{
    override void OnConsume(PlayerBase player)
    {
        super.OnConsume(player);
        player.GetStatFood().Add(100);
    }
}
```

→ [examples/edible_item/](../examples/edible_item/) - Полный пример

**Clothing предмет:**
```c
class MyShirt extends Clothing
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionTakeItem);
    }
}
```

→ [examples/clothing_item/](../examples/clothing_item/) - Полный пример

**Action:**
→ [examples/action/](../examples/action/) - Полный пример

---

## 🔧 Кастомизация

### Изменить свойства

**В config.cpp:**
```cpp
class MyItem: ItemBase
{
    scope = 2;
    displayName = "My Item";
    description = "My custom item";

    weight = 500;  // Вес
    itemSize[] = {2, 2};  // Размер

    // Economy
    nominal = 50;
    min = 25;
    max = 75;
}
```

### Добавить слоты

**В config.cpp:**
```cpp
class MyItem: ItemBase
{
    class Attachments
    {
        class Battery
        {
            slotName = "battery";
            slotType = "battery";
        };
    };
};
```

### Добавить actions

**В MyItem.c:**
```c
override void SetActions()
{
    super.SetActions();
    AddAction(ActionMyAction);
}
```

---

## 💡 Советы

1. **Начните с простого** - создайте ItemBase, потом усложняйте
2. **Используйте заглушки моделей** - `\DZ\items\apple.p3d` для тестов
3. **Тестируйте в Offline Mode** - быстро и без сервера
4. **Проверяйте логи** - `%localappdata%\DayZ\script.log`

---

## 📚 Примеры

→ [examples/minimal_item/](../examples/minimal_item/) - Минимальный предмет
→ [examples/edible_item/](../examples/edible_item/) - Еда
→ [examples/consumable_item/](../examples/consumable_item/) - Расходник
→ [examples/clothing_item/](../examples/clothing_item/) - Одежда
→ [examples/action/](../examples/action/) - Action
→ [examples/crafting/](../examples/crafting/) - Крафтинг

---

## 🚨 Проблемы?

**"Class not found"**
→ [troubleshooting/common_errors.md#class-not-found](../troubleshooting/common_errors.md#class-not-found)

**"Предмет не спавнится"**
→ [troubleshooting/common_errors.md#item-not-appear](../troubleshooting/common_errors.md#item-not-appear)

**"Мод не загружается"**
→ [troubleshooting/common_errors.md#mod-doesnt-load](../troubleshooting/common_errors.md#mod-doesnt-load)

---

**Время создания мода:** 3-5 минут
**Сложность:** ⭐☆☆☆☆ (очень простая)

**Готово к развитию:** Используйте как основу для вашего мода!
