# Installation Guide - GUI Crafting System

**Step-by-step guide to implement the crafting system**

---

## Step 1: Create Mod Structure

Create folder structure:
```
DayZ Mods/
└── CraftSystem/
    ├── config.cpp
    └── Scripts/
        ├── 3_Game/
        │   ├── CS_Constants.c
        │   ├── CS_Config.c
        │   └── CS_Handler.c
        ├── 4_World/
        │   ├── CS_CraftLogic.c
        │   └── CS_PlayerBase.c
        └── 5_Mission/
            └── CS_HUD.c
```

---

## Step 2: Copy Script Files

Copy all `.c` files from this example to your mod.

**Files needed:**
- `scripts/3_Game/CS_Constants.c`
- `scripts/3_Game/CS_Config.c`
- `scripts/3_Game/CS_Handler.c`
- `scripts/4_World/CS_CraftLogic.c`
- `scripts/4_World/CS_PlayerBase.c`
- `scripts/5_Mission/CS_HUD.c`

---

## Step 3: Create config.cpp

Copy `config.cpp` to your mod root.

**Verify paths:**
```cpp
files[] = {"CraftSystem/Scripts/3_Game"}  // Correct
files[] = {"CraftSystem/Scripts/4_World"} // Correct
files[] = {"CraftSystem/Scripts/5_Mission"} // Correct
```

---

## Step 4: Create GUI Layouts

**Option A: Use Example Layouts (Simpler)**

1. Create folder: `dta/gui/CraftSystem/Scripts/GUI/Layouts/`
2. Copy `CS_Main.layout.example` → `CS_Main.layout`
3. Copy `CS_RecipeRow.layout.example` → `CS_RecipeRow.layout`
4. Remove `.example` extension
5. Update paths in `CS_HUD.c:86` to match

**Option B: Create Custom Layouts (Advanced)**

Use DayZ Editor or create manually with same widget names:
- `ScrollRecipes` - Scroll widget
- `RecipeContainer` - Wrap spacer widget
- `ButtonClose` - Close button
- `PreviewItem1-5` - Item preview widgets
- `PreviewResult` - Result preview
- `ButtonCraft` - Craft button

---

## Step 5: Create GUI Input (Optional)

**Create `inputs/inputs.xml`:**
```xml
<inputs>
    <input name="UACraftMenu">
        <key name="F7" />
    </input>
</inputs>
```

**Update config.cpp:**
```cpp
inputs="CraftSystem/inputs/inputs.xml";
```

**Bind in CS_HUD.c (Update method):**
```c
if (GetUApi().GetInputByName("UACraftMenu").LocalPress())
{
    CloseMenu(); // Or open menu
}
```

---

## Step 6: Create JSON Config

**Option A: Auto-Create (Recommended)**

1. Start server
2. Handler creates default config at: `$profile:CraftSystem/CS_Recipes.json`
3. Edit file to add your recipes
4. Wait 60 seconds OR restart server

**Option B: Manual Create**

1. Create folder: `$profile:CraftSystem/`
2. Copy `CS_Recipes.json.example` → `CS_Recipes.json`
3. Remove `.example` extension
4. Edit recipes as needed

---

## Step 7: Add to Server Start Parameters

**Add to launch parameters:**
```
-mods=CraftSystem;
```

**Or use `server.cfg`:**
```
mod=@CraftSystem;
```

---

## Step 8: Test

1. **Start server**
2. **Join game**
3. **Open menu** (via custom key or command)
4. **Verify recipes load**
5. **Click craft button**
6. **Check server.log** for `[CraftSystem]` messages

---

## Verification Steps

### Check 1: Scripts Loaded

**server.log should show:**
```
[CraftSystem] Directory created: $profile:CraftSystem
[CraftSystem] Settings loaded - 2 recipes
```

### Check 2: JSON Created

**File exists:**
```
$profile:CraftSystem/CS_Recipes.json
```

### Check 3: Client Connected

**script.log should show:**
```
[CraftSystem] Recipes received from server - 2 recipes
```

### Check 4: Crafting Works

**Click craft, server.log:**
```
[CraftSystem] Craft processed - Recipe: AK-47 - Success: true
[CraftSystem] Item crafted: AK74 x1
```

---

## Troubleshooting

### Problem: Menu doesn't appear

**Check:**
1. Layout files exist in `dta/gui/CraftSystem/`
2. Layout paths match in `CS_HUD.c`
3. `config.cpp` has `missionScriptModule`
4. No script errors in `script.log`

**Solution:**
```c
// Verify widget names match layout
m_Root.FindAnyWidget("ScrollRecipes")
m_Root.FindAnyWidget("RecipeContainer")
m_Root.FindAnyWidget("ButtonClose")
```

---

### Problem: No recipes show

**Check:**
1. `$profile:CraftSystem/CS_Recipes.json` exists
2. Server has write permissions to `$profile`
3. `g_CS_Handler` is initialized

**Solution:**
```c
// Add debug print
Print("[CraftSystem] Handler loaded: " + g_CS_Handler.IsLoaded());
Print("[CraftSystem] Recipe count: " + g_CS_Handler.GetSettings().Recipes.Count());
```

---

### Problem: Crafting fails silently

**Check:**
1. Server log for `[CraftSystem]` messages
2. Player has ingredients (check inventory)
3. RPC IDs match between client/server

**Solution:**
```c
// Verify RPC IDs
RPC_SEND_RECIPES = 556677
RPC_REQUEST_CRAFT = 556678
RPC_CRAFT_RESULT = 556679
```

---

### Problem: Items don't spawn

**Check:**
1. `SpawnEntityOnGroundPos` is called (server-side)
2. Result item classname is valid
3. Server has permission to spawn items

**Solution:**
```c
// Add debug in CS_CraftLogic.c:163
Print("[CraftSystem] Spawning: " + classname + " at " + player.GetPosition());
```

---

## Advanced Customization

### Change Recipe Limit

**Edit CS_HUD.c:453:**
```c
// Limit to 20 recipes max
if (i >= 20) break;
```

### Add Sound Effects

**Edit config.cpp:60-78:**
```cpp
class CfgSoundSets
{
    class MyCraft_SoundSet
    {
        soundShaders[] = {"MyCraft_SoundShader"};
    };
};
```

**Edit CS_Constants.c:12:**
```c
static const string SOUND_CRAFT = "MyCraft_SoundSet";
```

### Add Notifications

**Edit CS_PlayerBase.c:49:**
```c
NotificationSystem.AddNotificationExtended(
    10,                                    // Duration
    "Craft System",                        // Title
    "You crafted: " + itemName,            // Message
    "CraftSystem\\Scripts\\GUI\\icon.edds" // Icon
);
```

---

## Next Steps

1. ✅ **Test basic functionality**
2. ✅ **Add custom recipes** to JSON
3. ✅ **Create custom GUI layout** (optional)
4. ✅ **Add keyboard shortcut** (optional)
5. ✅ **Customize sounds** (optional)
6. ✅ **Test with multiple players**

---

*Last updated: 2025-01-18*
*Source: examples/gui_crafting/*
