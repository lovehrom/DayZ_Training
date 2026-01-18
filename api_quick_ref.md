# API Quick Reference - DayZ Modding

**Purpose:** Quick lookup for most commonly used DayZ API methods
**For:** AI to find API methods without searching through 100+ files
**Last Updated:** 2025-01-18 (Phase 7)

---

## 🎯 GETGAME() - Core Game Access

### Player Operations
```c
GetGame().GetPlayer()                    // PlayerBase - Get current player
GetGame().GetPlayers(array)              // Fill array with all players
GetGame().GetPlayerByUID(id)            // PlayerBase - Get player by UID
GetGame().SelectPlayer(uid)             // Man - Switch to player perspective
```

### Time & World
```c
GetGame().GetTickTime()                 // float - Server time in seconds
GetGame().GetTime()                     // int - Current time (hours:minutes)
GetGame().GetDayTime()                  // int - Time in seconds since midnight
GetGame().GetYear()                     // int - In-game year
GetGame().GetMonth()                    // int - Month (1-12)
GetGame().GetDay()                      // int - Day (1-31)
```

### World & Environment
```c
GetGame().SurfaceY(x, z)                // float - Get ground height at position
GetGame().GetWaterLevel(x, z)           // float - Get water height
GetGame().IsServer()                    // bool - True if server
GetGame().IsClient()                    // bool - True if client
GetGame().IsMultiplayer()               // bool - True if multiplayer
```

### Object Creation
```c
GetGame().CreateObject(type, pos)       // EntityAI - Spawn entity
GetGame().CreateObjectEx(type, pos, ...) // EntityAI - Spawn with parameters
GetGame().ObjectDelete(obj)              // bool - Delete object
```

---

## 👤 PLAYERBASE - Player Character

### Identity & Info
```c
player.GetIdentity()                    // PlayerIdentity - Get player identity
player.GetID()                          // string - Get player ID
player.GetName()                        // string - Get player name
player.GetPlainID()                     // string - Get plain ID (Steam64)
```

### Position & Movement
```c
player.GetPosition()                    // vector - Get player position (x, y, z)
player.SetPosition(pos)                  // void - Set player position
player.GetOrientation()                 // vector - Get player orientation
player.SetOrientation(orient)           // void - Set player orientation
```

### Health & Status
```c
player.GetHealth()                      // float - Get overall health (0.0-1.0)
player.SetHealth(value)                 // void - Set overall health
player.GetHealth(zone, health)          // float - Get zone health
player.SetHealth(zone, health, value)   // void - Set zone health

player.GetBlood()                        // float - Get blood (0-5000)
player.SetBlood(value)                   // void - Set blood
player.GetBloodType()                    // EBloodType - Get blood type

player.GetEnergy()                       // float - Get energy (0-100000)
player.GetWater()                        // float - Get water (0-10000)
```

### Inventory Access
```c
player.GetInventory()                   // Inventory - Get player inventory
player.GetHumanInventory()              // HumanInventory - Get human inventory
player.GetStorage()                     // EntityAI - Get storage container
```

---

## 🎒 ENTITYAI - Base Entity Class

### Lifecycle
```c
entity.GetHierarchyRoot()              // EntityAI - Get root of hierarchy
entity.GetHierarchyParent()            // EntityAI - Get direct parent
entity.IsAlive()                        // bool - Check if alive
entity.IsDamageDestroyed()             // bool - Check if destroyed
entity.Delete()                         // void - Delete entity
```

### Position & Transform
```c
entity.GetPosition()                    // vector - Get position
entity.SetPosition(pos)                  // void - Set position
entity.GetOrientation()                 // vector - Get orientation angles
entity.SetOrientation(orient)           // void - Set orientation
entity.GetWorldPosition()              // vector - Get world position matrix
entity.GetYawPitchRoll()                // void - Get yaw, pitch, roll
```

### Properties
```c
entity.GetType()                        // string - Get entity type name
entity.GetConfig()                      // Config - Get config object
entity.GetHealth()                      // float - Get health (0.0-1.0)
entity.GetHealthLevel()                 // string - Get health level
```

---

## 📦 ITEMBASE - Items

### Basic Operations
```c
item.GetQuantity()                      // int - Get stack quantity
item.SetQuantity(qty)                   // void - Set quantity
item.GetHealth()                        // float - Get item condition (0.0-1.0)
item.SetHealth(value)                   // void - Set item condition
item.GetTemperature()                  // float - Get temperature
item.SetTemperature(temp)               // void - Set temperature
item.GetWet()                          // float - Get wetness (0.0-1.0)
item.SetWet(wet)                        // void - Set wetness
```

### Food & Edible
```c
food.GetEnergy()                        // float - Get food energy
food.GetNutrition()                     // float - Get nutrition value
food.GetVolume()                        // float - Get food volume
food.IsFood()                           // bool - Check if edible
food.GetFoodStage()                     // int - Get food stage
```

### Weapons
```c
weapon.GetChamberAmmoCount()           // int - Get ammo in chamber
weapon.GetMagazineAmmoCount()           // int - Get ammo in magazine
weapon.SpawnAttachedMagazine()          // bool - Spawn magazine
weapon.AttachMagazine(mag)              // void - Attach magazine
weapon.EjectMagazine()                   // void - Eject magazine
weapon.GetMuzzleDeviation()             // float - Get weapon spread
```

### Clothing
```c
clothing.GetInventory()                  // Inventory - Get clothing inventory
clothing.GetCargo()                     // Cargo - Get cargo storage
clothing.GetAttachmentsCount()          // int - Get number of attachments
clothing.GetAttachmentByIndex(idx)      // EntityAI - Get attachment
clothing.FindAttachmentBySlotName(slot) // EntityAI - Find in slot
```

---

## 🎒 INVENTORY - Inventory Management

### Basic Operations
```c
inventory.CreateEntityInInventory(item) // EntityAI - Create item in inventory
inventory.CreateInInventory(type)        // EntityAI - Create by type
inventory.HasEntity(entity)              // bool - Check if has entity
inventory.HasEntityOfType(type)         // bool - Check if has type
inventory.GetEntityByType(type)          // EntityAI - Get first of type
inventory.GetEntityAtIndex(idx)          // EntityAI - Get at index
```

### Count & Capacity
```c
inventory.CountEntity(type)              // int - Count items of type
inventory.GetItemCount()                // int - Get total item count
inventory.GetInventoryMaxCargo()        // float - Get max cargo capacity
inventory.GetCargoSize()                // float - Get current cargo size
inventory.GetFreeCargo()                // float - Get free cargo space
```

### Location & Attachment
```c
inventory.GetInventoryLocation()        // int - Get inventory location type
inventory.GetParent()                    // Object - Get parent object
inventory.FindAttachment(slot)          // EntityAI - Find attachment in slot
inventory.FindEntityAttachment(entity) // int - Find slot of entity
```

---

## 🔌 RPC MANAGER - Network Communication

### Registration
```c
GetRPCManager().AddRPC(modName, funcName, instance, type)
// modName - Module name (string)
// funcName - Function name (string)
// instance - Class instance (usually 'this')
// type - SingleplayerExecutionType (Server, Client, Both)

GetRPCManager().AddRPC("MyMod", "MyFunction", this, SingleplayerExecutionType.Server);
```

### Sending
```c
GetRPCManager().SendRPC(modName, funcName, params, guaranteed, target)
// modName - Module name (string)
// funcName - Function name (string)
// params - Param1-8 (serialized data)
// guaranteed - bool (true = reliable, false = unreliable)
// target - PlayerIdentity (NULL = all, specific = single)

GetRPCManager().SendRPC("MyMod", "Update", new Param1<int>(100), true, player.GetIdentity());
```

### Execution Types
```c
SingleplayerExecutionType.Server   // Execute only on server
SingleplayerExecutionType.Client   // Execute only on client
SingleplayerExecutionType.Both     // Execute on both server and client
```

---

## 📡 PARAMETER TYPES

### Param1 - Param8 (Up to 8 values)
```c
new Param1<int>(value)               // Single int
new Param2<int, string>(100, "text")  // Two values
new Param3<int, string, float>(100, "text", 1.5)  // Three values
new Param4<int, string, float, bool>(100, "text", 1.5, true)  // Four values
// ... up to Param8
```

### Reading Parameters
```c
void MyFunction(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int value1;
    string value2;
    float value3;

    if (!ctx.Read(value1)) return;  // Always check return value!
    if (!ctx.Read(value2)) return;
    if (!ctx.Read(value3)) return;

    // Process values
}
```

---

## 📊 FILE OPERATIONS

### File Paths
```c
"$profile:MyMod/"              // Server profile folder
"$mission:MyMod/"              // Mission folder
"$current:"                    // Current directory
"$saves:"                      // Save directory
```

### File Operations
```c
FileExist(path)                 // bool - Check if file exists
MakeDirectory(path)             // bool - Create directory
DeleteFile(path)                // bool - Delete file
CopyFile(from, to)               // bool - Copy file
```

### File Read/Write
```c
// Open file
File file = OpenFile(path, FileMode.READ);  // or FileMode.WRITE
if (!file) return;

// Write
file.WriteString(data);
file.Close();

// Read
string data;
file.ReadString(data);
file.Close();
```

### File Modes
```c
FileMode.READ                     // Open for reading
FileMode.WRITE                    // Open for writing (overwrites)
FileMode.APPEND                   // Open for append
```

---

## 🔔 NOTIFICATION SYSTEM

### Vanilla Notifications
```c
NotificationSystem.AddNotification(message)           // Show default notification
NotificationSystem.AddNotificationExtended(message, ...)  // Custom notification
NotificationSystem.SetNotificationIconColor(color)    // Set icon color
```

### Custom Notifications
```c
// Create notification with icon
NotificationSystem.AddNotification("Message", "iconName", COLOR_ORANGE);
```

---

## 🎨 COLORS (ARGB Format)

```c
ARGB(a, r, g, b)  // a=alpha (0-255), r=red, g=green, b=blue

// Common colors
ARGB(255, 255, 255, 255)   // White
ARGB(255, 0, 0, 0)         // Red
ARGB(255, 0, 255, 0)         // Green
ARGB(255, 0, 0, 255)         // Blue
ARGB(255, 255, 255, 0)       // Yellow
ARGB(255, 255, 0, 0)         // Orange
ARGB(255, 0, 0, 0)           // Black
ARGB(128, 255, 255, 255)     // Semi-transparent white
```

---

## 🧮 MATH FUNCTIONS

### Random Numbers
```c
Math.RandomFloat01()                // float - 0.0 to 1.0 (exclusive)
Math.RandomFloatInclusive(min, max)  // float - min to max (inclusive)
Math.RandomIntInclusive(min, max)     // int - min to max (inclusive)
```

### Math Operations
```c
Math.Abs(x)                        // Absolute value
Math.Min(a, b)                     // Minimum
Math.Max(a, b)                     // Maximum
Math.Clamp(value, min, max)       // Clamp between min and max
Math.Round(x)                      // Round to nearest int
Math.Floor(x)                      // Round down
Math.Ceil(x)                       // Round up
```

### Trigonometry
```c
Math.Sin(angle)                    // Sine (angle in radians)
Math.Cos(angle)                    // Cosine (angle in radians)
Math.Tan(angle)                    // Tangent (angle in radians)
Math.PI                            // 3.14159...
Math.PI2                           // 6.28318...
```

### Distance & Direction
```c
vector.Distance(pos1, pos2)        // float - Distance between positions
vector.Direction(pos1, pos2)       // vector - Direction from pos1 to pos2
```

---

## 🔧 UTILITY FUNCTIONS

### String Operations
```c
string.IsEmpty()                    // Check if empty
string.Length()                     // Get length
string.Substring(start, len)        // Get substring
string.ToLower()                    // Convert to lowercase
string.ToUpper()                    // Convert to uppercase
string.Split(delimiter)            // Split string into array
```

### Array Operations
```c
array.Count()                       // Get array size
array.Get(index)                    // Get element at index
array.Insert(item)                  // Insert at end
array.Remove(index)                 // Remove at index
array.Clear()                       // Clear all elements
array.Find(item)                    // Find index of item
array.Contains(item)                 // Check if contains
```

### Map Operations
```c
map.Count()                         // Get map size
map.Set(key, value)                 // Set key-value pair
map.Get(key)                        // Get value for key
map.Contains(key)                   // Check if contains key
map.Remove(key)                     // Remove key-value pair
map.Clear()                         // Clear all entries
map.GetElements()                   // Get all keys as array
```

---

## 🎮 COMMON CONSTANTS

### Time Constants
```c
0.016          // ~1 frame (60 FPS)
0.1            // 100 ms
1.0            // 1 second
60.0           // 1 minute
3600.0         // 1 hour
86400.0        // 1 day
```

### Health Values
```c
0.0            // Dead
1.0            // Full health
0.5            // Half health
```

### Inventory Slots
```c
GameConstants.CARGO_SLOTS_PER_ROW     // 5 slots per row
GameConstants.CARGO_MAX_HEIGHT         // 10 rows max
GameConstants.CARGO_GLOBAL_MAX_WIDTH   // 20 slots max width
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Phase 7 (Part 2/5)
**Next:** troubleshooting/mechanics_issues.md
