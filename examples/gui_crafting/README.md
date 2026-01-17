# GUI Crafting System Example

**Status:** ✅ Working Example
**Source:** CraftSystem (Steam Workshop: 3626433444)
**Complexity:** Advanced

---

## ⚡ 30-Second Summary

GUI-based crafting system with JSON configuration, client-server RPC communication, and visual ingredient preview. Supports stackable/non-stackable items, color-coded availability, and hot-reload recipes.

---

## 🎯 Features

### Core Functionality
- ✅ **GUI Recipe Menu** - Visual interface with item previews
- ✅ **JSON Configuration** - Hot-reloadable recipes
- ✅ **Client-Server RPC** - Secure server-side validation
- ✅ **Ingredient Checking** - Stackable (nails, ammo) and non-stackable (tools, food)
- ✅ **Color-Coded UI** - Red (missing) / Green (available)
- ✅ **Sound Feedback** - Craft sound effect
- ✅ **Notifications** - Success/failure messages

### Advanced Features
- ✅ **Item Previews** - 3D preview widgets for ingredients and result
- ✅ **Auto-Refresh** - GUI updates after crafting
- ✅ **Server Authority** - All crafting validated on server
- ✅ **Recipe Sync** - Server sends recipes to all clients on connect

---

## 📁 File Structure

```
gui_crafting/
├── README.md                    ← This file
├── config.cpp                   ← Mod configuration
├── scripts/
│   ├── 3_Game/
│   │   ├── CS_Constants.c       ← RPC IDs, sound paths
│   │   ├── CS_Config.c          ← Recipe classes
│   │   └── CS_Handler.c         ← JSON loading, recipe sync
│   ├── 4_World/
│   │   ├── CS_CraftLogic.c      ← Inventory checking, item removal
│   │   └── CS_PlayerBase.c      ← RPC handlers, craft request
│   └── 5_Mission/
│       └── CS_HUD.c             ← GUI menu, recipe rows, click handling
└── gui/
    └── layouts/
        ├── CS_Main.layout       ← Main menu layout (EXAMPLE)
        └── CS_RecipeRow.layout  ← Recipe row layout (EXAMPLE)
```

---

## 🚀 Installation

### Step 1: Copy Files

Copy to your mod:
```
YourMod/
├── config.cpp
└── scripts/
    └── [Copy all .c files]
```

### Step 2: Create GUI Layouts

Create layout files in `dta/gui` or use placeholder:
```
gui/layouts/
├── CS_Main.layout       ← Main menu with scroll container
└── CS_RecipeRow.layout  ← Recipe row with item previews
```

**Example CS_Main.layout:**
```xml
<!-- Main menu with title, scroll container, close button -->
<Widget>
  <Panel Name="PanelMain" />
  <ScrollWidget Name="ScrollRecipes" />
  <WrapSpacerWidget Name="RecipeContainer" />
  <ButtonWidget Name="ButtonClose" />
  <TextWidget Name="TextTitle" />
</Widget>
```

**Example CS_RecipeRow.layout:**
```xml
<!-- Recipe row with 5 ingredient slots + result + craft button -->
<Widget>
  <Panel Name="PanelItem1">
    <ItemPreviewWidget Name="PreviewItem1" />
    <TextWidget Name="TextName1" />
    <TextWidget Name="TextQty1" />
  </Panel>
  <!-- Repeat for Item2-5 -->
  <ItemPreviewWidget Name="PreviewResult" />
  <ButtonWidget Name="ButtonCraft" />
</Widget>
```

### Step 3: Configure Recipes

First server start creates:
```
$profile:CraftSystem/CS_Recipes.json
```

**Default recipes:**
```json
{
  "Recipes": [
    {
      "DisplayName": "AK-47",
      "ResultItem": "AK74",
      "ResultQuantity": 1,
      "Ingredients": [
        { "Classname": "Nail", "Quantity": 20 },
        { "Classname": "MetalWire", "Quantity": 1 }
      ]
    }
  ]
}
```

### Step 4: Test

1. Start server
2. Join game
3. Press custom input (F7 by default)
4. Crafting menu opens
5. Click craft button

---

## 💡 Key Concepts

### 1. Client-Server RPC Architecture

**Flow:**
```
Client                              Server
  |                                   |
  |--(1) REQUEST_CRAFT--------------->|
  |     (recipeIndex)                 |
  |                                   |--(2) Validate ingredients
  |                                   |--(3) Remove items
  |                                   |--(4) Create result
  |                                   |--(5) Save JSON
  |<--(6) CRAFT_RESULT----------------|
  |     (success, itemName)            |
  |--(7) Show notification            |
```

**RPC IDs (CS_Constants.c:3-5):**
```c
RPC_SEND_RECIPES   = 556677  // Server → Client: recipes
RPC_REQUEST_CRAFT  = 556678  // Client → Server: craft
RPC_CRAFT_RESULT   = 556679  // Server → Client: result
```

---

### 2. Stackable vs Non-Stackable Items

**Logic (CS_CraftLogic.c:41-76):**

**Stackable items** (nails, ammo, rags):
- Check `ConfigGetBool("canBeSplit")`
- Count by **quantity** (20 nails = 1 item)
- Remove by quantity from single stack

**Non-stackable items** (food, tools, bottles):
- Count by **whole items**
- Only count if >80% quantity/condition
- Delete entire items

**Example:**
```c
// Stackable: Remove 20 nails from 1 stack
if (itemBase.ConfigGetBool("canBeSplit"))
{
    itemBase.AddQuantity(-20); // Reduce quantity
}

// Non-stackable: Delete 1 bandage
else
{
    GetGame().ObjectDelete(item); // Delete item
}
```

---

### 3. GUI Recipe Row System

**Architecture (CS_HUD.c:1-394):**

Each recipe creates a **CS_RecipeRow** with:
- **5 ingredient slots** (only show needed)
- **1 result preview** (crafted item)
- **1 craft button** (enabled/disabled)
- **Color-coded backgrounds** (red/green)

**Flow:**
1. **Create widget** from layout
2. **Setup item previews** for ingredients + result
3. **Check availability** (count items in inventory)
4. **Set colors** (red if missing, green if available)
5. **Enable/disable button** based on availability

**Item Preview (CS_HUD.c:364-378):**
```c
EntityAI SetupItemPreview(ItemPreviewWidget preview, string classname)
{
    // Create temporary entity for preview
    EntityAI itemEntity = EntityAI.Cast(GetGame().CreateObject(classname, vector.Zero, true, false, true));

    if (itemEntity)
    {
        preview.SetItem(itemEntity);
        preview.SetView(itemEntity.GetViewIndex());
        preview.SetModelPosition(Vector(0, 0, 0.5));
        m_PreviewEntities.Insert(itemEntity); // Store for cleanup
    }
    return itemEntity;
}
```

**Cleanup (CS_HUD.c:70-77):**
```c
void ~CS_RecipeRow()
{
    // Delete temporary preview entities
    foreach (EntityAI ent : m_PreviewEntities)
    {
        if (ent) GetGame().ObjectDelete(ent);
    }
}
```

---

### 4. JSON Hot-Reload System

**Auto-reload (CS_Handler.c:51-66):**
```c
void LoadSettings()
{
    // Call every 60 seconds
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ReloadSettings, 60000, true);
}

void ReloadSettings()
{
    string path = "$profile:CraftSystem/CS_Recipes.json";

    if (FileExist(path))
    {
        JsonFileLoader<CS_Settings>.JsonLoadFile(path, m_Settings);

        // Send to all players
        if (GetGame().IsServer())
        {
            SendRecipesToAllPlayers();
        }
    }
}
```

**Usage:**
1. Edit `CS_Recipes.json`
2. Wait 60 seconds OR restart server
3. All clients receive new recipes

---

## 📝 API Reference

### CS_CraftLogic (Static Helper)

**Location:** `scripts/4_World/CS_CraftLogic.c`

```c
// Check if player has all ingredients
static bool CanCraft(PlayerBase player, CS_Recipe recipe)

// Execute crafting (server only)
static bool DoCraft(PlayerBase player, CS_Recipe recipe)

// Count items in inventory (stackable support)
static int CountItemInInventory(PlayerBase player, string classname)

// Remove items from inventory
static void RemoveItemsFromInventory(PlayerBase player, string classname, int quantity)

// Create result item
static void SpawnResultItem(PlayerBase player, string classname, int quantity)
```

---

### CS_Handler (Manager)

**Location:** `scripts/3_Game/CS_Handler.c`

```c
// Get recipe settings
CS_Settings GetSettings()

// Reload recipes from JSON
void ReloadSettings()

// Send recipes to all players
void SendRecipesToAllPlayers()

// Check if loaded
bool IsLoaded()
```

**Global instance:**
```c
ref CS_Handler g_CS_Handler;

// Usage:
if (g_CS_Handler && g_CS_Handler.GetSettings())
{
    // Access recipes
}
```

---

### CS_Recipe (Data Class)

**Location:** `scripts/3_Game/CS_Config.c:15-29`

```c
class CS_Recipe
{
    string DisplayName;                    // "AK-47"
    string ResultItem;                     // "AK74"
    int ResultQuantity;                    // 1
    ref array<ref CS_Ingredient> Ingredients;
}
```

---

### CS_Ingredient (Data Class)

**Location:** `scripts/3_Game/CS_Config.c:3-13`

```c
class CS_Ingredient
{
    string Classname;  // "Nail"
    int Quantity;      // 20
}
```

---

## 🎮 Usage Examples

### Example 1: Add Custom Recipe

**Edit `$profile:CraftSystem/CS_Recipes.json`:**
```json
{
  "Recipes": [
    {
      "DisplayName": "Medical Kit",
      "ResultItem": "FirstAidKit",
      "ResultQuantity": 1,
      "Ingredients": [
        { "Classname": "Bandage_Dressing", "Quantity": 3 },
        { "Classname": "Morphine", "Quantity": 1 },
        { "Classname": "Epinephrine", "Quantity": 1 }
      ]
    }
  ]
}
```

---

### Example 2: Open Menu Programmatically

```c
// Open crafting menu
GetGame().GetUIManager().ShowScriptedMenu(CS_HUD, null);
```

---

### Example 3: Check Recipe Availability

```c
PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
CS_Recipe recipe = g_CS_Handler.GetSettings().Recipes.Get(0);

bool canCraft = CS_CraftLogic.CanCraft(player, recipe);

if (canCraft)
{
    Print("Player can craft: " + recipe.DisplayName);
}
```

---

## 🔧 Customization

### Change Craft Sound

**config.cpp:60-78:**
```cpp
class CfgSoundSets
{
    class MyCraft_SoundSet
    {
        soundShaders[] = {"MyCraft_SoundShader"};
    };
};
```

**CS_Constants.c:12:**
```c
static const string SOUND_CRAFT = "MyCraft_SoundSet";
```

---

### Add Keyboard Shortcut

**Create `inputs/inputs.xml`:**
```xml
<inputs>
  <input name="UACraftMenu">
    <key name="F7" />
  </input>
</inputs>
```

**Bind in `CS_HUD.c`:**
```c
override void Update(float timeslice)
{
    super.Update(timeslice);

    if (GetUApi().GetInputByName("UACraftMenu").LocalPress())
    {
        CloseMenu();
    }
}
```

---

## 🐛 Troubleshooting

### GUI Doesn't Show

**Problem:** Menu doesn't appear

**Solutions:**
1. Check layout files exist in correct path
2. Check `config.cpp` has `missionScriptModule`
3. Check script errors in `script.log`

---

### Recipes Not Loading

**Problem:** No recipes show in menu

**Solutions:**
1. Check `$profile:CraftSystem/CS_Recipes.json` exists
2. Check server has permission to write to `$profile`
3. Check `script.log` for JSON errors
4. Manually create JSON if auto-creation fails

---

### Crafting Fails Silently

**Problem:** Click craft, nothing happens

**Solutions:**
1. Check server log for `[CraftSystem]` messages
2. Verify `CS_Handler` is initialized on server
3. Check RPC IDs match between client/server
4. Verify player has ingredients (check inventory)

---

### Item Previews Don't Show

**Problem:** No 3D models in GUI

**Solutions:**
1. Check `ItemPreviewWidget` names match layout
2. Check `SetupItemPreview()` creates entities successfully
3. Check `preview.SetItem()` is called
4. Verify widget cleanup isn't deleting previews too early

---

## 🔗 Related Topics

→ [Crafting GUI](../topics/25_crafting_gui.md) - GUI crafting mechanics
→ [GUI Systems](../topics/16_gui_systems.md) - Widget creation
→ [Network](../topics/08_network.md) - RPC communication
→ [JSON Config](../topics/10_json_config.md) - JSON persistence
→ [Reference: Crafting GUI](../reference/CRAFTING_GUI_SYSTEMS.md) - Full documentation

---

## 📊 Performance Notes

### Memory Usage
- **Per recipe row:** ~5-10 KB (5 item preview entities)
- **10 recipes:** ~50-100 KB
- **Cleanup:** Entities deleted when menu closes

### Network Traffic
- **Recipe sync:** ~5-10 KB per player (one-time on connect)
- **Craft RPC:** ~50 bytes per craft request
- **Result RPC:** ~100 bytes per response

### Recommendations
- Limit to 20-30 recipes max (GUI performance)
- Use icon sprites instead of 3D previews for better performance
- Cache recipe checks (don't re-check every frame)

---

*Последнее обновление: 2025-01-18*
*Источник: CraftSystem (3626433444)*
