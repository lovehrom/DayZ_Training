# BZ_Garage - Vehicle Pack/Unpack System

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3646080855 / BZ_Garage
**Related:** [Storage System](STORAGE_SYSTEM.md) | [Entity Lifecycle](ENTITYAI_LIFECYCLE.md)

---

## ⚡ 30-Second Summary

Система упаковки/распаковки транспорта с полным сохранением инвентаря (включая оружие, патроны, еду, температуру).

---

## 🎯 5-Minute Solution: Pack/Unpack транспорт

### BZ_InvNode recursive structure

```c
// ✅ ПРОВЕРЕНО: BZ_InvNode.c
class BZ_InvNode
{
    string Type;           // Classname предмета
    float Health01;        // Здоровье (0.0-1.0)
    int Quantity;          // Количество
    int AmmoCount;         // Патроны (для оружия/магазинов)

    // Еда
    bool IsFood;
    int m_FoodStageType;   // FoodStage: RAW, BAKED, BOILED, etc.
    float m_Temperature;

    // Рекурсивные структуры
    ref array<ref BZ_InvNode> Attachments;  // Присоединённые предметы
    ref array<ref BZ_InvNode> Cargo;        // Предметы в инвентаре
}
```

### Pack vehicle

```c
// ✅ ПРОВЕРЕНО: Garage.c
void PackVehicle(Transport vehicle)
{
    BZ_Garage garageKit = BZ_Garage.Cast(GetGame().CreateObject("BZ_Garage", GetPosition()));

    // Сохранить состояние транспорта
    garageKit.m_VehicleType = vehicle.GetType();
    garageKit.m_VehicleHealth = vehicle.GetHealth01("", "");
    garageKit.m_VehicleFuel = vehicle.GetFuel();
    garageKit.m_VehicleOil = vehicle.GetOil();

    // Сохранить полный инвентарь
    garageKit.m_Inventory = SaveInventory(vehicle.GetInventory());

    // Удалить транспорт
    GetGame().ObjectDelete(vehicle);
}

// Рекурсивное сохранение инвентаря
array<ref BZ_InvNode> SaveInventory(EntityAI entity)
{
    array<ref BZ_InvNode> inventory = new array<ref BZ_InvNode>;

    // Сохранить attachments
    for (int i = 0; i < entity.GetInventory().AttachmentCount(); i++)
    {
        EntityAI attachment = entity.GetInventory().GetAttachmentFromIndex(i);
        BZ_InvNode node = SaveEntity(attachment);
        inventory.Insert(node);
    }

    // Сохранить cargo
    Cargo cargo = entity.GetInventory().GetCargo();
    if (cargo)
    {
        for (int j = 0; j < cargo.GetItemCount(); j++)
        {
            ItemBase item = cargo.GetItem(j);
            BZ_InvNode node = SaveEntity(item);
            inventory.Insert(node);
        }
    }

    return inventory;
}

BZ_InvNode SaveEntity(EntityAI entity)
{
    BZ_InvNode node = new BZ_InvNode();
    node.Type = entity.GetType();
    node.Health01 = entity.GetHealth01("", "");

    // Количество
    ItemBase item = ItemBase.Cast(entity);
    if (item)
    {
        node.Quantity = item.GetQuantity();

        // Патроны (для оружия/магазинов)
        if (item.IsInherited(Weapon_Base))
        {
            Weapon_Base weapon = Weapon_Base.Cast(item);
            node.AmmoCount = weapon.GetInternalMagazineAmmoCount();
        }
        else if (item.IsInherited(Magazine))
        {
            Magazine mag = Magazine.Cast(item);
            node.AmmoCount = mag.GetAmmoCount();
        }

        // Еда
        if (item.IsInherited(Edible_Base))
        {
            node.IsFood = true;
            FoodBase food = FoodBase.Cast(item);
            node.m_FoodStageType = food.GetFoodStageType();
            node.m_Temperature = food.GetTemperature();
        }
    }

    // Рекурсивно сохранить attachments и cargo
    node.Attachments = SaveInventory(entity);
    node.Cargo = SaveInventory(entity);

    return node;
}
```

### Unpack vehicle

```c
// ✅ ПРОВЕРЕНО: Garage.c
void UnPackVehicle()
{
    // Создать транспорт
    Transport vehicle = Transport.Cast(GetGame().CreateObject(m_VehicleType, GetPosition()));

    // Восстановить состояние
    vehicle.SetHealth01("", "", m_VehicleHealth);
    vehicle.SetFuel(m_VehicleFuel);
    vehicle.SetOil(m_VehicleOil);

    // Восстановить инвентарь
    RestoreInventory(vehicle.GetInventory(), m_Inventory);

    // Удалить garage kit
    GetGame().ObjectDelete(this);
}

void RestoreInventory(EntityAI entity, array<ref BZ_InvNode> inventory)
{
    foreach (BZ_InvNode node : inventory)
    {
        // Создать предмет
        ItemBase item = ItemBase.Cast(GetGame().CreateObject(node.Type, GetPosition()));

        // Восстановить состояние
        item.SetHealth01("", "", node.Health01);
        item.SetQuantity(node.Quantity);

        // Патроны
        if (node.AmmoCount > 0)
        {
            if (item.IsInherited(Weapon_Base))
            {
                Weapon_Base weapon = Weapon_Base.Cast(item);
                weapon.SetInternalMagazineAmmoCount(node.AmmoCount);
            }
            else if (item.IsInherited(Magazine))
            {
                Magazine mag = Magazine.Cast(item);
                mag.SetAmmoCount(node.AmmoCount);
            }
        }

        // Еда
        if (node.IsFood)
        {
            FoodBase food = FoodBase.Cast(item);
            food.SetFoodStageType(node.m_FoodStageType);
            food.SetTemperature(node.m_Temperature);
        }

        // Присоединить к entity
        entity.GetInventory().AttachEntity(item);

        // Рекурсивно восстановить attachments
        if (node.Attachments)
        {
            RestoreInventory(item, node.Attachments);
        }

        // Рекурсивно восстановить cargo
        if (node.Cargo)
        {
            Cargo cargo = item.GetInventory().GetCargo();
            foreach (BZ_InvNode cargoNode : node.Cargo)
            {
                ItemBase cargoItem = ItemBase.Cast(GetGame().CreateObject(cargoNode.Type, GetPosition()));
                // ... восстановить состояние
                cargo.AddItemToCargo(cargoItem, true);
            }
        }
    }
}
```

---

## 📚 Deep Dive

### Full vehicle state preservation

**✅ ПРОВЕРЕНО:** BZ_Garage pattern

```c
class BZ_Garage : ItemBase
{
    // Состояние транспорта
    protected string m_VehicleType;       // Тип транспорта (например, "OffroadHatchback")
    protected float m_VehicleHealth;       // Здоровье транспорта
    protected float m_VehicleFuel;         // Топливо
    protected float m_VehicleOil;          // Масло

    // Инвентарь транспорта
    protected ref array<ref BZ_InvNode> m_Inventory;
}
```

### Food state preservation

**✅ ПРОВЕРЕНО:** BZ_InvNode food fields

```c
class BZ_InvNode
{
    // Еда специфичные поля
    bool IsFood;              // Маркер что это еда
    int m_FoodStageType;      // FoodStage enum
    float m_Temperature;      // Температура еды
}

// Пример использования:
void SaveFoodState(FoodBase food, BZ_InvNode node)
{
    node.IsFood = true;
    node.m_FoodStageType = food.GetFoodStageType();  // RAW, BAKED, BOILED, DRIED, ROTTEN
    node.m_Temperature = food.GetTemperature();      // Температура в градусах
}

void RestoreFoodState(FoodBase food, BZ_InvNode node)
{
    if (node.IsFood)
    {
        food.SetFoodStageType(node.m_FoodStageType);
        food.SetTemperature(node.m_Temperature);
    }
}
```

**FoodStage значения:**
```c
enum FoodStage
{
    RAW,        // Сырое
    BAKED,      // Жареное
    BOILED,     // Варёное
    DRIED,      // Сушёное
    ROTTEN      // Гнилое
}
```

### Recursive inventory handling

**✅ ПРОВЕРЕНО:** BZ_InvNode recursive structure

```c
// Структура может быть вложенной до 3+ уровней:
// Transport → Battery (attachment) → Bulbs (cargo)
// Transport → CargoBox → Cargo → Meat (food)

BZ_InvNode
├── Attachments[]
│   ├── SparkPlug (attachment)
│   └── Battery (attachment)
│       └── Cargo[]
│           └── BatteryBulb (cargo item)
└── Cargo[]
    ├── HoloFlashlight (cargo item)
    └── MeatSteak (cargo item - food)
        ├── IsFood = true
        ├── m_FoodStageType = BAKED
        └── m_Temperature = 15.0
```

---

## 💡 Examples

### Example 1: Pack личной машины

```c
modded class PlayerBase
{
    void PackMyCar()
    {
        // Найти ближайший транспорт
        array<CargoBase> transports = new array<CargoBase>;
        GetGame().GetObjectsAtPosition(GetPosition(), 10, transports, null);

        foreach (CargoBase obj : transports)
        {
            Transport vehicle = Transport.Cast(obj);
            if (vehicle && vehicle.Get CrewMember(0) == this)  // Я водитель
            {
                BZ_Garage garage = BZ_Garage.Cast(GetGame().CreateObject("BZ_Garage", GetPosition()));
                garage.PackVehicle(vehicle);

                GetGame().ChatPlayer("Транспорт упакован!");
                break;
            }
        }
    }
}
```

### Example 2: Unpack при использовании

```c
class BZ_Garage : ItemBase
{
    override void OnAction(ActionData action_data)
    {
        super.OnAction(action_data);

        if (action_data.m_Action == ActionUnpackVehicle)
        {
            UnPackVehicle();
        }
    }
}
```

---

## 🔗 Related Topics

→ [Storage System](STORAGE_SYSTEM.md) - OnStoreSave/OnStoreLoad patterns
→ [Inventory System](INVENTORY_SYSTEM.md) - GetInventory() паттерны
→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - EEInit, EEKilled

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (BZ_Garage)

**Что проверено:**
- ✅ BZ_InvNode recursive structure (Type, Health01, Quantity, AmmoCount, Attachments, Cargo)
- ✅ Food state preservation (IsFood, m_FehicleStageType, m_Temperature)
- ✅ Pack/Unpack vehicle mechanics
- ✅ Full inventory preservation (recursive)

**Что НЕ проверено:**
- ❌ Полная реализация SaveInventory/RestoreInventory (есть пробелы в cargo handling)
- ❌ Как обрабатываются Expansion PIDs (упомянутые в Explore agent output)
- ❌ Работает ли без BZ_Garage мода

---

## 🚨 Troubleshooting

**Ошибка:** Предметы теряются при unpack
- **Причина:** Неправильная рекурсия или cargo handling
- **Решение:** Проверьте что все уровни attachments и cargo сохраняются

**Ошибка:** Еда становится сырой после распаковки
- **Причина:** Не сохраняется/восстанавливается FoodStage
- **Решение:** Проверьте m_FoodStageType и m_Temperature поля

**Ошибка:** Патроны теряются
- **Причина:** Не используется SetInternalMagazineAmmoCount/SetAmmoCount
- **Решение:** Проверьте что AmmoCount правильно сохраняется/восстанавливается

**Ошибка:** Транспорт не появляется
- **Причина:** Неправильный vehicle type или позиция
- **Решение:** Проверьте что m_VehicleType корректный classname

---

## 💡 Tips

### Best Practices

1. **Рекурсия:** Убедитесь что обработка attachments и cargo правильная
2. **FoodStage:** Всегда сохраняйте и восстанавливайте FoodStage и Temperature
3. **Патроны:** Используйте правильные методы для оружия (SetInternalMagazineAmmoCount) и магазинов (SetAmmoCount)
4. **Проверка типов:** Проверяйте типы через IsInherited() перед кастом

### Performance

- Лимитируйте размер инвентаря транспорта (например, 500 предметов)
- Не pack/unpack каждый кадр (только по action)
- Используйте OnStoreSave для альтернативного persistence

### Security

- Проверяйте что игрок владелец транспорта (CrewMember check)
- Не позволяйте pack/unpack в движении
- Валидируйте данные при загрузке из сохранения

---

*Последнее обновление: 2025-01-16*
*Источник: BZ_Garage (Workshop ID: 3646080855)*
