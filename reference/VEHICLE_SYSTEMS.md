# Vehicle Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Storage System](STORAGE_SYSTEM.md) | [Entity Lifecycle](ENTITYAI_LIFECYCLE.md)

---

## ⚡ 30-Second Summary

Системы упаковки/распаковки транспорта с полным сохранением инвентаря и состояния предметов.

---

## 🎯 5-Minute Solution: Pack/Unpack транспорт

### InvNode recursive structure

```c
class BZ_InvNode
{
    string Type;
    float Health01;
    int Quantity;
    int AmmoCount;

    // Еда
    bool IsFood;
    int m_FoodStageType;
    float m_Temperature;

    // Рекурсивные структуры
    ref array<ref BZ_InvNode> Attachments;
    ref array<ref BZ_InvNode> Cargo;
}
```

### Pack vehicle

```c
void PackVehicle(Transport vehicle)
{
    BZ_Garage garageKit = BZ_Garage.Cast(GetGame().CreateObject("BZ_Garage", GetPosition()));

    garageKit.m_VehicleType = vehicle.GetType();
    garageKit.m_VehicleHealth = vehicle.GetHealth01("", "");
    garageKit.m_VehicleFuel = vehicle.GetFuel();

    garageKit.m_Inventory = SaveInventory(vehicle.GetInventory());

    GetGame().ObjectDelete(vehicle);
}
```

---

## 📚 Deep Dive

## BZ_Garage (3646080855) - Vehicle Pack/Unpack

### Full vehicle state preservation

```c
class BZ_Garage : ItemBase
{
    protected string m_VehicleType;
    protected float m_VehicleHealth;
    protected float m_VehicleFuel;
    protected float m_VehicleOil;
    protected ref array<ref BZ_InvNode> m_Inventory;
}
```

### Food state preservation

```c
class BZ_InvNode
{
    // Еда специфичные поля
    bool IsFood;
    int m_FoodStageType;  // RAW, BAKED, BOILED, DRIED, ROTTEN
    float m_Temperature;
}
```

### Recursive inventory handling

```
Transport → Battery (attachment) → Bulbs (cargo)
Transport → CargoBox → Cargo → Meat (food)
```

---

## 💡 Examples

### Example 1: Pack личной машины

```c
modded class PlayerBase
{
    void PackMyCar()
    {
        array<CargoBase> transports = new array<CargoBase>;
        GetGame().GetObjectsAtPosition(GetPosition(), 10, transports, null);

        foreach (CargoBase obj : transports)
        {
            Transport vehicle = Transport.Cast(obj);
            if (vehicle && vehicle.Get CrewMember(0) == this)  // Я водитель
            {
                BZ_Garage garage = BZ_Garage.Cast(GetGame().CreateObject("BZ_Garage", GetPosition()));
                garage.PackVehicle(vehicle);
                break;
            }
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

**Что НЕ проверено:**
- ❌ Полная реализация SaveInventory/RestoreInventory
- ❌ Работает ли без BZ_Garage мода

---

## 🚨 Troubleshooting

**Ошибка:** Предметы теряются
- **Решение:** Проверьте рекурсию attachments и cargo

**Ошибка:** Еда становится сырой
- **Решение:** Проверьте m_FoodStageType и m_Temperature

---

## 💡 Tips

### Best Practices

1. **Рекурсия:** Убедитесь что обработка всех уровней правильная
2. **FoodStage:** Всегда сохраняйте и восстанавливайте FoodStage и Temperature
3. **Патроны:** Используйте правильные методы (SetInternalMagazineAmmoCount/SetAmmoCount)

---

*Последнее обновление: 2025-01-16*
*Источник: BZ_Garage (Workshop ID: 3646080855)*
