# Inventory System - Система инвентаря

**✅ ПРОВЕРЕНО:** raw_data/EntityAI.c, PlayerBase.c
**Источник:** Распакованный PBO (игровые файлы)

---

## GetInventory().CreateInInventory()

Создать предмет внутри инвентаря.

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:3910
EntityAI item = GetInventory().CreateInInventory("ClassName");

// Пример:
EntityAI apple = GetInventory().CreateInInventory("Apple");
```

**Возвращает:**
- `EntityAI` - созданный предмет (или `null` если не получилось)

---

## GetInventory().TakeEntityToInventory()

Взять существующий предмет в инвентарь.

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:1866,1868,1872,1876
bool taken = GetInventory().TakeEntityToInventory(InventoryMode, flags, item);

// Примеры из EntityAI.c:
GetInventory().TakeEntityToInventory(InventoryMode.JUNCTURE, flags, item);    // 1866
GetInventory().TakeEntityToInventory(InventoryMode.PREDICTIVE, flags, item);  // 1868
GetInventory().TakeEntityToInventory(InventoryMode.LOCAL, flags, item);       // 1872
GetInventory().TakeEntityToInventory(InventoryMode.SERVER, flags, item);      // 1876
```

**InventoryMode:**
- `InventoryMode.SERVER` - серверная сторона
- `InventoryMode.LOCAL` - локальная (клиент)
- `InventoryMode.PREDICTIVE` - с предсказанием
- `InventoryMode.JUNCTURE` - стыковочный режим

**Возвращает:**
- `bool` - `true` если предмет взят, `false` если нет

---

## GetInventory().EnumerateInventory()

Перебрать все предметы в инвентаре.

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:1451,5833,6453
array<EntityAI> itemsArray = new array<EntityAI>;
GetInventory().EnumerateInventory(InventoryTraversalType, itemsArray);

// Примеры из PlayerBase.c:
GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);    // 1451
GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, items);          // 5833
GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, itemsArray);   // 6453
```

**InventoryTraversalType:**
- `PREORDER` - прямой обход
- `INORDER` - внутрипорядковый
- `LEVELORDER` - по уровням

---

## Template

### Создать предмет в инвентаре

```c
class MyContainer extends ItemBase
{
    void SpawnApple()
    {
        EntityAI apple = GetInventory().CreateInInventory("Apple");

        if (apple)
        {
            Print("[MyContainer] Apple created successfully!");
        }
        else
        {
            Print("[MyContainer] Failed to create apple");
        }
    }
}
```

### Взять предмет в инвентарь

```c
class PlayerBase extends Man
{
    bool TakeItem(EntityAI item)
    {
        InventoryLocation flags = new InventoryLocation;

        if (GetInventory().TakeEntityToInventory(InventoryMode.SERVER, flags, item))
        {
            Print("[PlayerBase] Item taken successfully");
            return true;
        }

        return false;
    }
}
```

### Перебрать все предметы

```c
class MyContainer extends ItemBase
{
    void PrintAllItems()
    {
        array<EntityAI> items = new array<EntityAI>;
        GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, items);

        Print("[MyContainer] Total items: " + items.Count());

        for (int i = 0; i < items.Count(); i++)
        {
            EntityAI item = items.Get(i);
            Print("  - " + item.GetType());
        }
    }
}
```

### Найти конкретный тип предмета

```c
class PlayerBase extends Man
{
    EntityAI FindItemByType(string typeName)
    {
        array<EntityAI> items = new array<EntityAI>;
        GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, items);

        for (int i = 0; i < items.Count(); i++)
        {
            EntityAI item = items.Get(i);
            if (item && item.GetType() == typeName)
            {
                return item;
            }
        }

        return null;
    }

    void UseBandage()
    {
        EntityAI bandage = FindItemByType("BandageDressing");

        if (bandage)
        {
            // Использовать бинт
        }
        else
        {
            Print("No bandage found!");
        }
    }
}
```

---

## InventoryLocation

⚠️ **НЕДОСТАТОЧНО ДАННЫХ**

`InventoryLocation` используется для указания куда класть предмет, но полная документация не найдена.

```c
InventoryLocation flags = new InventoryLocation;
// Требуется дополнительная информация по настройке flags
```

---

## Источники

- **raw_data/EntityAI.c** (строки 1866, 1868, 1872, 1876, 3910)
- **raw_data/PlayerBase.c** (строки 1451, 5833, 6453)

---

## ⚠️ Что НЕ найдено

- Полная документация по `InventoryLocation`
- Все методы `GetInventory()`
- Как переместить предмет между слотами
- Как удалить предмет из инвентаря

---

*Последнее обновление: 2025-01-16*
*Статус: Проверено на raw_data*
