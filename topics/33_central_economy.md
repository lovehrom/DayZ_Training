# Central Economy for Beginners

**Status:** ✅ Beginner Guide
**Difficulty:** Beginner
**Time to Learn:** 30 minutes
**Related:** [Central Economy Reference](../../reference/CENTRAL_ECONOMY.md)

---

## ⚡ 30-Second Summary

Central Economy (CE) is DayZ's vanilla system for controlling loot spawns and trader prices using XML configuration files. No scripting required - just edit XML and restart server!

---

## 🎯 What You'll Learn

1. **What is Central Economy?** - XML-based configuration system
2. **cfgeconomycore.xml** - Main CE configuration
3. **types.xml** - Item definitions
4. **Trader Integration** - Dynamic pricing
5. **Common Issues** - Troubleshooting

---

## 💡 What is Central Economy?

**Central Economy** = DayZ's vanilla loot/economy system

**Controls:**
- 🎁 Loot spawns (what items spawn where)
- 💰 Trader prices (buy/sell prices)
- 🚁 Dynamic events (helicopter crashes, etc.)
- 📦 Spawn rates (how often items appear)

**Why use CE?**
- ✅ Vanilla system (built into DayZ)
- ✅ No programming required (XML only)
- ✅ Easy to balance (edit numbers, restart server)

---

## 🔧 Basic Setup

### Step 1: Create cfgeconomycore.xml

**Location:** `mpmissions/dayzOffline.chernarusplus/cfgeconomycore.xml`

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<economycore>
    <!-- Root classes -->
    <classes>
        <rootclass name="Inventory_Base" />
        <rootclass name="SurvivorBase" act="character" />
        <rootclass name="CarScript" act="car" />
    </classes>

    <!-- Defaults -->
    <defaults>
        <default name="dyn_radius" value="30" />
    </defaults>

    <!-- Your mod -->
    <ce folder="MyMod_ce">
        <file name="MyMod_types.xml" type="types" />
    </ce>
</economycore>
```

---

### Step 2: Create types.xml

**Location:** `central_economy/MyMod_types.xml`

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<types>
    <!-- Food items -->
    <type name="Apple">
        <value name="food" />
        <value name="nutrition" value="100" />
    </type>

    <!-- Weapons -->
    <type name="AK74">
        <value name="weapon" />
        <value name="rarity" value="rare" />
    </type>
</types>
```

---

### Step 3: Restart Server

**Changes take effect after:**
1. Edit XML files
2. Restart server
3. CE loads new configuration

---

## 🎮 Usage Examples

### Define Trader Prices

```xml
<type name="AK74">
    <value name="buyPrice" value="10000" />
    <value name="sellPrice" value="5000" />
</type>

<type name="Apple">
    <value name="buyPrice" value="10" />
    <value name="sellPrice" value="5" />
</type>
```

### Define Currency Items

```xml
<type name="Coin_10">
    <value name="currency" />
    <value name="value" value="10" />
    <value name="trader" />
    <value name="banking" />
</type>

<type name="Money_Ruble">
    <value name="currency" />
    <value name="value" value="1000" />
    <value name="trader" />
</type>
```

### Control Spawn Rates

```xml
<type name="AK74">
    <value name="nominal" value="5" />     <!-- Target count -->
    <value name="min" value="1" />         <!-- Minimum count -->
    <value name="max" value="3" />         <!-- Maximum count -->
    <value name="spawn_chance" value="0.01" />  <!-- 1% chance -->
</type>
```

---

## ⚠️ Common Issues

### Issue 1: Changes Not Taking Effect

**Cause:** Server not restarted

**Solution:**
1. Edit XML files
2. **Restart server** (required!)
3. CE reloads on startup

### Issue 2: Items Not Spawning

**Cause:** Wrong root class or spawn rate too low

**Solution:**
```xml
<!-- Check root class -->
<classes>
    <rootclass name="Inventory_Base" />  <!-- Make sure your item inherits this -->
</classes>

<!-- Check spawn chance -->
<type name="MyItem">
    <value name="spawn_chance" value="0.5" />  <!-- Not too low! -->
</type>
```

---

## 📝 Quick Reference

### Basic Structure

```xml
<economycore>
    <classes>
        <rootclass name="ClassName" />
    </classes>
    <ce folder="folder">
        <file name="file.xml" type="types" />
    </ce>
</economycore>
```

### Type Definition

```xml
<type name="ItemClass">
    <value name="property" value="value" />
</type>
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete for Beginners
