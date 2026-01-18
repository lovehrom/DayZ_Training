# Central Economy Integration

**Status:** ✅ Complete Reference
**Source:** Vanilla DayZ + TraderPlus
**Complexity:** Intermediate
**Related:** [Trader System](../../topics/27_trader_systems.md) | [Config Guide](../../topics/02_config.md)

---

## ⚡ 30-Second Summary

Central Economy (CE) is DayZ's vanilla system for controlling loot spawns, traders, and dynamic events through XML configuration files. Mods can extend CE to add custom items, prices, and spawn rules.

---

## 🎯 Key Concepts

### What is Central Economy?

**Central Economy** = Vanilla DayZ's loot/economy system

**Components:**
- **cfgeconomycore.xml** - Main CE configuration
- **types.xml** - Item definitions and categories
- **cfgspawnabletypes.xml** - Loot spawn rules
- **dynamic events** - Helicopter crashes, contaminated areas
- **Trader integration** - Custom economy for traders

### Why Use Central Economy?

**Benefits:**
- ✅ Vanilla system (built into DayZ)
- ✅ No scripting required (XML only)
- ✅ Performance optimized
- ✅ Easy to balance (edit XML, restart server)
- ✅ Integrates with traders

**Drawbacks:**
- ❌ XML format (verbose)
- ❌ Server restart required for changes
- ❌ Limited flexibility (compared to script)

---

## 📁 File Structure

```
DayZ_Server/
├── mpmissions/
│   └── dayzOffline.chernarusplus/
│       └── cfgeconomycore.xml    ← Main CE config
├── central_economy/
│   ├── types.xml                 ← Item categories/tags
│   └── config.xml                ← CE loader
└── @MyMod/
    └── config.cpp                ← Mod items
```

---

## 🔧 Implementation Guide

### Step 1: Create cfgeconomycore.xml

**Location:** `mpmissions/dayzOffline.chernarusplus/cfgeconomycore.xml`

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<economycore>
    <!-- Root classes for economy -->
    <classes>
        <rootclass name="DefaultWeapon" />
        <rootclass name="DefaultMagazine" />
        <rootclass name="Inventory_Base" />
        <rootclass name="HouseNoDestruct" reportMemoryLOD="no" />
        <rootclass name="SurvivorBase" act="character" reportMemoryLOD="no" />
        <rootclass name="DZ_LightAI" act="character" reportMemoryLOD="no" />
        <rootclass name="CarScript" act="car" reportMemoryLOD="no" />
    </classes>

    <!-- Default values -->
    <defaults>
        <default name="dyn_radius" value="30" />
        <default name="dyn_smin" value="0" />
        <default name="dyn_smax" value="0" />
        <default name="dyn_dmin" value="1" />
        <default name="dyn_dmax" value="5" />
        <default name="log_ce_loop" value="false"/>
        <default name="save_events_startup" value="true"/>
        <default name="save_types_startup" value="true"/>
    </defaults>

    <!-- My Mod CE -->
    <ce folder="MyMod_ce">
        <file name="MyMod_types.xml" type="types" />
    </ce>
</economycore>
```

**Explanation:**
- `<classes>` - Root class types (what items can spawn)
- `<defaults>` - Default settings for dynamic events
- `<ce>` - Custom economy folders

---

### Step 2: Create types.xml

**Location:** `central_economy/MyMod_types.xml`

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<types>
    <!-- Custom item categories -->
    <category name="food">
        <tag name="food" />
    </category>

    <category name="drinks">
        <tag name="drink" />
    </category>

    <category name="weapons">
        <tag name="weapon" />
    </category>

    <!-- Specific items with values -->
    <type name="Apple">
        <value name="food" />
        <value name="nutrition" value="100" />
    </type>

    <type name="SodaCan_Cola">
        <value name="drink" />
        <value name="hydration" value="200" />
    </type>

    <type name="AK74">
        <value name="weapon" />
        <value name="rarity" value="rare" />
    </type>
</types>
```

**Explanation:**
- `<category>` - Item categories (food, drinks, weapons, etc.)
- `<tag>` - Category tags
- `<type>` - Specific item class names
- `<value>` - Item properties (nutrition, rarity, etc.)

---

### Step 3: Dynamic Pricing (Trader Integration)

**Create trader price file:**

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<prices>
    <!-- Weapons -->
    <item name="AK74" buyPrice="10000" sellPrice="5000" />
    <item name="M4A1" buyPrice="15000" sellPrice="7500" />

    <!-- Food -->
    <item name="Apple" buyPrice="10" sellPrice="5" />
    <item name="SodaCan_Cola" buyPrice="20" sellPrice="10" />

    <!-- Ammo -->
    <item name="Mag_AKM_30Rnd" buyPrice="100" sellPrice="50" />
    <item name="Mag_STANAG_30Rnd" buyPrice="150" sellPrice="75" />
</prices>
```

**Load prices in trader system:**

```c
class TraderPrices
{
    map<string, int> BuyPrices;
    map<string, int> SellPrices;

    void LoadPrices()
    {
        string filePath = "$profile:Trader/prices.xml";

        if (!FileExist(filePath))
            return;

        // Parse XML (simplified)
        // In real implementation, use XML parser
        BuyPrices.Set("AK74", 10000);
        SellPrices.Set("AK74", 5000);
    }

    int GetBuyPrice(string itemClass)
    {
        if (BuyPrices.Contains(itemClass))
            return BuyPrices.Get(itemClass);
        return 0;  // Free or not found
    }

    int GetSellPrice(string itemClass)
    {
        if (SellPrices.Contains(itemClass))
            return SellPrices.Get(itemClass);
        return 0;
    }
}
```

---

## 🔗 Integration Examples

### Example 1: Custom Currency

**Define currency items in types.xml:**

```xml
<type name="Coin_10">
    <value name="currency" />
    <value name="value" value="10" />
</type>

<type name="Coin_50">
    <value name="currency" />
    <value name="value" value="50" />
</type>

<type name="Coin_100">
    <value name="currency" />
    <value name="value" value="100" />
</type>
```

**Load in banking system:**

```c
int GetCurrencyValue(ItemBase item)
{
    // Check if item is currency
    if (!item.IsKindOf("Coin_10") && !item.IsKindOf("Coin_50") && !item.IsKindOf("Coin_100"))
        return 0;

    // Get value from CE
    // In real implementation, query CE types.xml
    if (item.IsKindOf("Coin_10"))
        return 10;
    else if (item.IsKindOf("Coin_50"))
        return 50;
    else if (item.IsKindOf("Coin_100"))
        return 100;

    return 0;
}
```

---

### Example 2: Unified Currency System

**Multiple items as currency:**

```xml
<type name="Coin_10">
    <value name="currency" />
    <value name="value" value="10" />
    <value name="trader" />  <!-- Accepted by traders -->
    <value name="banking" />  <!-- Accepted by ATMs -->
</type>

<type name="Money_Ruble">
    <value name="currency" />
    <value name="value" value="1000" />
    <value name="trader" />
    <value name="banking" />
</type>

<type name="Money_Dollar">
    <value name="currency" />
    <value name="value" value="100" />
    <value name="trader" />
    <value name="banking" />
</type>
```

**Usage in systems:**

```c
// Banking system accepts all currency items
bool IsCurrency(ItemBase item)
{
    // Check CE tags
    // In real implementation, query CE data
    array<string> currencyItems = {"Coin_10", "Coin_50", "Coin_100", "Money_Ruble", "Money_Dollar"};

    foreach (string currencyClass : currencyItems)
    {
        if (item.IsKindOf(currencyClass))
            return true;
    }

    return false;
}

// Trader system accepts specific currencies
bool IsAcceptedByTrader(ItemBase item)
{
    // Check CE "trader" tag
    // Coins + Rubles + Dollars
    return IsCurrency(item);  // Simplified
}
```

---

### Example 3: Dynamic Loot Spawns

**Control spawn rates via types.xml:**

```xml
<type name="AK74">
    <value name="rarity" value="rare" />
    <value name="spawn_chance" value="0.01" />  <!-- 1% chance -->
</type>

<type name="Apple">
    <value name="rarity" value="common" />
    <value name="spawn_chance" value="0.5" />  <!-- 50% chance -->
</type>

<type name="SodaCan_Cola">
    <value name="rarity" value="uncommon" />
    <value name="spawn_chance" value="0.1" />  <!-- 10% chance -->
</type>
```

**CE automatically spawns items based on:**
- `spawn_chance` - Probability
- `rarity` - Spawn location tier
- `nominal` - Target count in area
- `min`/`max` - Minimum/maximum count

---

## 📊 CE Data Structure

### Loading CE Data

```c
class CentralEconomyLoader
{
    map<string, int> ItemValues;
    map<string, string> ItemCategories;

    void LoadCEData()
    {
        string cePath = "$profile:CentralEconomy/";

        // Load types.xml
        string typesPath = cePath + "types.xml";

        if (FileExist(typesPath))
        {
            ParseTypesXML(typesPath);
        }

        Print("[CE] Loaded " + ItemValues.Count() + " items");
    }

    void ParseTypesXML(string filePath)
    {
        // Parse XML (simplified example)
        // In real implementation, use XML parser

        ItemValues.Set("Apple", 10);
        ItemValues.Set("AK74", 10000);
        ItemCategories.Set("Apple", "food");
        ItemCategories.Set("AK74", "weapons");
    }

    int GetItemValue(string itemClass)
    {
        if (ItemValues.Contains(itemClass))
            return ItemValues.Get(itemClass);
        return 0;
    }

    string GetItemCategory(string itemClass)
    {
        if (ItemCategories.Contains(itemClass))
            return ItemCategories.Get(itemClass);
        return "misc";
    }
}
```

---

## 🛡️ Security Considerations

### 1. Validate CE Data

```c
bool ValidateCEData(string itemClass, int value)
{
    // Check value is reasonable
    if (value < 0 || value > 100000000)
    {
        Print("[CE] WARNING: Invalid value for " + itemClass + ": " + value);
        return false;
    }

    // Check item exists
    if (!GetGame().IsKindOf(itemClass, "ItemBase"))
    {
        Print("[CE] WARNING: Invalid item class: " + itemClass);
        return false;
    }

    return true;
}
```

### 2. Prevent CE Exploits

```c
// Server-side validation
bool CanSellItem(PlayerBase player, ItemBase item)
{
    // Check item is in player inventory
    if (!player.GetInventory().HasItemInInventory(item))
    {
        Print("[Security] Player tried to sell item not in inventory!");
        return false;
    }

    // Check item is allowed to be sold
    string category = GetItemCategory(item.GetType());
    if (category == "forbidden")
    {
        Print("[Security] Player tried to sell forbidden item: " + item.GetType());
        return false;
    }

    return true;
}
```

---

## 📝 Quick Reference

### cfgeconomycore.xml Structure

```xml
<economycore>
    <classes>
        <rootclass name="ClassName" />
    </classes>
    <defaults>
        <default name="setting" value="value" />
    </defaults>
    <ce folder="folder_name">
        <file name="file.xml" type="types" />
    </ce>
</economycore>
```

### types.xml Structure

```xml
<types>
    <category name="category_name">
        <tag name="tag_name" />
    </category>
    <type name="ItemClass">
        <value name="property" value="value" />
    </type>
</types>
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** See [Trader System](../../topics/27_trader_systems.md) for trader integration
