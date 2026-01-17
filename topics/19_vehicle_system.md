# Vehicle System

**Status:** ⚠️ Mod-derived
**Related:** [Storage](14_storage.md) | [Inventory System](../reference/INVENTORY_SYSTEM.md)

---

## ⚡ 30-Second Summary

Система pack/unpack транспорта с полным сохранением инвентаря и состояния (fuel, health, cargo).

---

## 🎯 5-Minute Solution

### Recursive inventory node

```c
class BZ_InvNode
{
    string Type;
    float Health;
    int Quantity;
    bool IsFood;
    float m_Temperature;

    ref array<ref BZ_InvNode> Attachments;
    ref array<ref BZ_InvNode> Cargo;
}
```

### Pack vehicle

```c
BZ_Garage garageKit = BZ_Garage.Cast(GetGame().CreateObject("BZ_Garage", position));
garageKit.m_VehicleType = vehicle.GetType();
garageKit.m_Inventory = SaveInventory(vehicle.GetInventory());
GetGame().ObjectDelete(vehicle);
```

---

## 💡 Examples

### Пример 1: Unpack vehicle

```c
Car UnpackVehicle(BZ_Garage kit, vector position)
{
    Car car = Car.Cast(GetGame().CreateObject(kit.m_VehicleType, position));
    car.SetHealth01("", "", kit.m_VehicleHealth);
    car.SetFuel(kit.m_VehicleFuel);

    RestoreInventory(car.GetInventory().GetCargo(), kit.m_Inventory);

    GetGame().ObjectDelete(kit);
    return car;
}
```

### Пример 2: Save inventory

```c
array<BZ_InvNode> SaveInventory(EntityAI source)
{
    array<BZ_InvNode> nodes = new array<BZ_InvNode>;

    CargoBase cargo = source.GetInventory().GetCargo();
    if (cargo)
    {
        for (int i = 0; i < cargo.GetItemCount(); i++)
        {
            ItemBase item = cargo.GetItem(i);
            nodes.Insert(SaveItemRecursive(item));
        }
    }

    return nodes;
}
```

---

## 🔗 Related Topics

→ [Storage](14_storage.md) - Cross-server storage
→ [Reference: Vehicle Systems](../reference/VEHICLE_SYSTEMS.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
*Источник: BZ_Garage (3646080855)*
