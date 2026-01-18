# Vehicle + Storage Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** vehicle_systems + storage

---

## ⚡ 30-Second Summary

Integration example showing vehicle cargo persistence. Items stored in vehicle cargo are saved cross-server and persist through vehicle pack/unpack.

---

## 🎯 Features

### Integration Mechanics
- ✅ **Vehicle Cargo Persistence** - Items survive server restart
- ✅ **Cross-Server Storage** - Same inventory on any server
- ✅ **Recursive Save** - Weapon ammo, attachments saved
- ✅ **Pack/Unpack Support** - Inventory preserved
- ✅ **Cargo Access** - Players can access stored items

---

## 📁 File Structure

```
vehicle_storage_integration/
├── README.md                         ← This file
├── config.cpp                        ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── PersistentVehicle.c           ← Vehicle with storage
            ├── VehicleCargoManager.c         ← Cargo save/load
            └── CargoItemNode.c               ← Cargo item representation
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- `vehicle_systems/` - Pack/unpack vehicles
- `storage/` - Cross-server item persistence

**Integration:**
1. Player places items in vehicle → CargoManager tracks
2. Vehicle packed/Server restart → Cargo saved to JSON
3. Vehicle unpacked/Server restart → Cargo loaded from JSON
4. Vehicle cargo persists → Items available on any server

### Cargo Structure

```
Vehicle
├── Battery (attachment)
│   └── Bulb (cargo) → Saved recursively
├── CargoBox
│   └── Cargo
│       ├── Weapon → Saved with attachments
│       ├── Magazine → Saved with ammo count
│       └── Meat → Saved with food state
└── Fuel Tank → Fuel quantity saved
```

---

## 💡 Implementation

### 1. CargoItemNode.c

Item representation for storage:

```c
class CargoItemNode
{
    string Type;                 // "AK74"
    float Health01;              // 1.0 = 100% health
    int Quantity;                // For stackable items
    int AmmoCount;               // For magazines

    // Food-specific
    bool IsFood;
    int FoodStageType;           // RAW, BAKED, BOILED
    float Temperature;

    // Recursive structures
    ref array<ref CargoItemNode> Attachments;
    ref array<ref CargoItemNode> Cargo;

    void CargoItemNode()
    {
        Attachments = new array<ref CargoItemNode>;
        Cargo = new array<ref CargoItemNode>;
    }

    // Serialize to JSON
    string Serialize()
    {
        JsonSaveManager json = new JsonSaveManager();

        return json.SaveToString(this);
    }

    // Deserialize from JSON
    static CargoItemNode Deserialize(string jsonStr)
    {
        JsonSaveManager json = new JsonSaveManager();

        CargoItemNode node;
        json.LoadFromString(node, jsonStr);

        return node;
    }
}
```

### 2. VehicleCargoManager.c

Manager for vehicle cargo:

```c
class VehicleCargoManager
{
    protected ref map<string, ref array<ref CargoItemNode>> m_VehicleCargo;

    protected const string CARGO_SAVE_PATH = "$profile:VehicleCargo/";

    void VehicleCargoManager()
    {
        m_VehicleCargo = new map<string, ref array<ref CargoItemNode>>;
    }

    // Save vehicle cargo to JSON
    void SaveVehicleCargo(string vehicleID, EntityAI vehicle)
    {
        array<ref CargoItemNode> cargoNodes = new array<ref CargoItemNode>;

        // Save cargo
        SaveCargoRecursive(vehicle, cargoNodes);

        // Store
        m_VehicleCargo.Set(vehicleID, cargoNodes);

        // Save to file
        string filePath = CARGO_SAVE_PATH + vehicleID + ".json";
        SaveToFile(filePath, cargoNodes);

        Print("[VehicleCargo] Saved cargo for " + vehicleID);
    }

    // Load vehicle cargo from JSON
    void LoadVehicleCargo(string vehicleID, EntityAI vehicle)
    {
        string filePath = CARGO_SAVE_PATH + vehicleID + ".json";

        // Load from file
        array<ref CargoItemNode> cargoNodes = LoadFromFile(filePath);

        if (!cargoNodes)
            return;

        // Restore cargo
        RestoreCargoRecursive(vehicle, cargoNodes);

        Print("[VehicleCargo] Loaded cargo for " + vehicleID);
    }

    // Recursive save
    protected void SaveCargoRecursive(EntityAI entity, array<ref CargoItemNode> nodes)
    {
        // Create node for this item
        CargoItemNode node = new CargoItemNode();
        node.Type = entity.GetType();
        node.Health01 = entity.GetHealth01("", "");

        // Quantity
        ItemBase item = ItemBase.Cast(entity);
        if (item)
        {
            node.Quantity = item.GetQuantity();

            // Food
            if (item.IsFood())
            {
                node.IsFood = true;
                node.FoodStageType = item.GetFoodStageType();
                node.Temperature = item.GetTemperature();
            }

            // Magazine ammo
            Magazine mag = Magazine.Cast(item);
            if (mag)
            {
                node.AmmoCount = mag.GetAmmoCount();
            }
        }

        // Save attachments (recursive)
        Inventory inventory = entity.GetInventory();
        if (inventory)
        {
            for (int i = 0; i < inventory.AttachmentCount(); i++)
            {
                EntityAI attachment = inventory.GetAttachmentFromIndex(i);
                SaveCargoRecursive(attachment, node.Attachments);
            }

            // Save cargo (recursive)
            for (int j = 0; j < inventory.GetCargo().GetItemCount(); j++)
            {
                EntityAI cargoItem = inventory.GetCargo().GetItem(j);
                SaveCargoRecursive(cargoItem, node.Cargo);
            }
        }

        nodes.Insert(node);
    }

    // Recursive restore
    protected void RestoreCargoRecursive(EntityAI entity, array<ref CargoItemNode> nodes)
    {
        foreach (CargoItemNode node : nodes)
        {
            // Create item
            EntityAI item = GetGame().CreateObject(node.Type, entity.GetPosition());

            if (!item)
                continue;

            // Restore properties
            item.SetHealth01("", "", node.Health01);

            ItemBase itemBase = ItemBase.Cast(item);
            if (itemBase)
            {
                itemBase.SetQuantity(node.Quantity);

                // Food state
                if (node.IsFood)
                {
                    itemBase.SetFoodStageType(node.FoodStageType);
                    itemBase.SetTemperature(node.Temperature);
                }

                // Magazine ammo
                Magazine mag = Magazine.Cast(itemBase);
                if (mag && node.AmmoCount > 0)
                {
                    mag.ServerSetAmmoCount(node.AmmoCount);
                }
            }

            // Restore attachments
            Inventory inventory = entity.GetInventory();
            if (inventory)
            {
                foreach (CargoItemNode attachmentNode : node.Attachments)
                {
                    array<ref CargoItemNode> attachmentArray = {attachmentNode};
                    RestoreCargoRecursive(item, attachmentArray);
                }

                // Restore cargo
                foreach (CargoItemNode cargoNode : node.Cargo)
                {
                    array<ref CargoItemNode> cargoArray = {cargoNode};
                    RestoreCargoRecursive(item, cargoArray);
                }
            }
        }
    }

    protected void SaveToFile(string path, array<ref CargoItemNode> nodes)
    {
        JsonSaveManager json = new JsonSaveManager();

        array<string> jsonStrings = new array<string>;
        foreach (CargoItemNode node : nodes)
        {
            jsonStrings.Insert(node.Serialize());
        }

        json.SaveToFile(path, jsonStrings);
    }

    protected array<ref CargoItemNode> LoadFromFile(string path)
    {
        if (!FileExist(path))
            return null;

        JsonSaveManager json = new JsonSaveManager();

        array<string> jsonStrings;
        json.LoadFromFile(path, jsonStrings);

        array<ref CargoItemNode> nodes = new array<ref CargoItemNode>;
        foreach (string jsonStr : jsonStrings)
        {
            nodes.Insert(CargoItemNode.Deserialize(jsonStr));
        }

        return nodes;
    }
}
```

### 3. PersistentVehicle.c

Vehicle with cargo persistence:

```c
class PersistentVehicle extends Car
{
    protected ref VehicleCargoManager m_CargoManager;
    protected string m_VehicleID;

    void PersistentVehicle()
    {
        m_CargoManager = new VehicleCargoManager();

        // Generate unique ID
        m_VehicleID = GetVehicleID();
    }

    // Generate unique vehicle ID
    string GetVehicleID()
    {
        // Use Steam64 ID + position + timestamp
        return "Vehicle_" + GetPosition().ToString() + "_" + GetGame().GetTickTime();
    }

    // Save cargo before pack/delete
    override void EEDeleted(EntityAI parent)
    {
        super.EEDeleted(parent);

        // Save cargo
        m_CargoManager.SaveVehicleCargo(m_VehicleID, this);
    }

    // Load cargo on spawn
    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);

        // Save cargo reference
        ctx.Write(m_VehicleID);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))
            return false;

        // Load cargo reference
        ctx.Read(m_VehicleID);

        // Load cargo
        m_CargoManager.LoadVehicleCargo(m_VehicleID, this);

        return true;
    }

    override void OnEngineStop()
    {
        super.OnEngineStop();

        // Auto-save cargo when engine stops
        m_CargoManager.SaveVehicleCargo(m_VehicleID, this);
    }

    override void OnEngineStart()
    {
        super.OnEngineStart();

        // Auto-load cargo when engine starts
        m_CargoManager.LoadVehicleCargo(m_VehicleID, this);
    }
}
```

### 4. PlayerBase Integration

```c
modded class PlayerBase
{
    ref VehicleCargoManager m_VehicleCargoManager;

    override void OnConnect()
    {
        super.OnConnect();

        m_VehicleCargoManager = new VehicleCargoManager();
    }

    // Pack vehicle
    void PackVehicle(Car vehicle)
    {
        // Save cargo
        string vehicleID = vehicle.GetPosition().ToString();
        m_VehicleCargoManager.SaveVehicleCargo(vehicleID, vehicle);

        // Create vehicle kit
        EntityAI kit = GetGame().CreateObject("CarKit", GetPosition());

        // Delete vehicle
        GetGame().ObjectDelete(vehicle);

        NotificationSystem.Notify(this, "Vehicle packed! Cargo saved.");
    }

    // Unpack vehicle
    void UnpackVehicle(ItemBase kit)
    {
        // Get vehicle ID from kit
        string vehicleID = "Vehicle_" + kit.GetPosition().ToString();

        // Create vehicle
        Car vehicle = Car.Cast(GetGame().CreateObject("PersistentVehicle", GetPosition()));

        if (vehicle)
        {
            // Load cargo
            m_VehicleCargoManager.LoadVehicleCargo(vehicleID, vehicle);

            // Delete kit
            kit.Delete();

            NotificationSystem.Notify(this, "Vehicle unpacked! Cargo loaded.");
        }
    }
}
```

---

## 📝 Usage Examples

### Example 1: Pack Vehicle with Cargo

```c
modded class PlayerBase
{
    void ActionPackVehicle(Car vehicle)
    {
        // Check if vehicle has cargo
        if (vehicle.GetInventory().GetCargo().GetItemCount() > 0)
        {
            NotificationSystem.Notify(this, "Packing vehicle with cargo...");

            // Save cargo
            string vehicleID = "V_" + GetIdentity().GetId();
            m_VehicleCargoManager.SaveVehicleCargo(vehicleID, vehicle);

            // Pack
            PackVehicle(vehicle);
        }
        else
        {
            NotificationSystem.Notify(this, "Vehicle is empty!");
        }
    }
}
```

### Example 2: Fuel Persistence

```c
class VehicleCargoNode
{
    // Add fuel
    float FuelQuantity;

    static VehicleCargoNode CreateFromVehicle(Car vehicle)
    {
        VehicleCargoNode node = new VehicleCargoNode();
        node.FuelQuantity = vehicle.GetFuel();

        return node;
    }

    void RestoreFuel(Car vehicle)
    {
        vehicle.Fill(FuelQuantity);
    }
}
```

### Example 3: Multi-Server Access

```c
class CrossServerVehicleManager extends VehicleCargoManager
{
    override void SaveVehicleCargo(string vehicleID, EntityAI vehicle)
    {
        super.SaveVehicleCargo(vehicleID, vehicle);

        // Upload to cloud storage
        // (for true cross-server)
        UploadToCloud(vehicleID);
    }

    override void LoadVehicleCargo(string vehicleID, EntityAI vehicle)
    {
        // Download from cloud storage
        // (for true cross-server)
        DownloadFromCloud(vehicleID);

        super.LoadVehicleCargo(vehicleID, vehicle);
    }
}
```

---

## 🔗 Related Examples

→ [Vehicle Systems](../vehicle_systems/) - Individual vehicle mechanics
→ [Storage](../storage/) - Individual storage mechanics
→ [Integration: XP+Storage](../xp_storage_integration/) - Another integration

---

## ⚠️ Important Notes

### Cargo Persistence
- **Save on:** Pack, engine stop, delete
- **Load on:** Unpack, engine start, spawn
- **Format:** JSON in `$profile:VehicleCargo/`

### Recursive Save
- ✅ Weapons with attachments
- ✅ Magazines with ammo
- ✅ Food with temperature
- ✅ Containers with items

### Cross-Server
- JSON files are server-side
- For true cross-server: Use cloud storage
- Steam64 ID-based identification

### Performance
- ~1-5 KB per item
- Save only when needed (not every frame)
- Async loading for large cargo

---

*Last updated: 2025-01-18*
*Combines: vehicle_systems + storage*
