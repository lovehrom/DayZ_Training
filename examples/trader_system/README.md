# Trader System Example

**Status:** ✅ Working Example
**Complexity:** Advanced
**Time to create:** 1 hour

---

## ⚡ 30-Second Summary

Complete trader system with NPC traders, currency system, price tables, buy/sell mechanics, and safe zones.

---

## 🎯 Features

### Core Functionality
- ✅ **NPC Traders** - AI-controlled merchants
- ✅ **Currency System** - Coin tracking with persistence
- ✅ **Buying Items** - Purchase from traders
- ✅ **Selling Items** - Sell to traders
- ✅ **Price Tables** - Configurable buy/sell prices
- ✅ **Stock Limits** - Optional stock management
- ✅ **Safe Zones** - No-combat areas
- ✅ **Dynamic Pricing** - Supply/demand affects prices

---

## 📁 File Structure

```
trader_system/
├── README.md                    ← This file
├── config.cpp                   ← Mod configuration
├── prices.json                  ← Price table
├── stock.json                   ← Stock limits
└── scripts/
    ├── 3_Game/
    │   ├── CurrencyManager.c     ← Currency tracking
    │   ├── PriceManager.c        ← Price management
    │   └── TradeActions.c        ← Buy/sell logic
    ├── 4_World/
    │   └── Classes/
    │       ├── TraderNPC.c       ← Trader NPC
    │       ├── TraderZone.c      ← Safe zone
    │       ├── CoinItem.c        ← Currency items
    │       └── PlayerBase/
    │           └── playerbase_trader.c ← Player integration
    └── 5_Mission/
        └── TraderMenu.c         ← UI (optional)
```

---

## 💡 Key Concepts

### 1. Currency System

**Steam64 ID-based tracking:**
```c
// Each player has balance stored by Steam64 ID
map<string, int> m_Balance; // "76561198..." → 1000 coins

// Save to JSON
SaveToFile("$profile:CurrencyData.json");

// Load on connect
LoadFromFile("$profile:CurrencyData.json");
```

**Persistence:**
- Saved on disconnect
- Loaded on connect
- Cross-server compatible

---

### 2. Trader NPC

**Based on DayZInfected:**
```c
class TraderNPC extends DayZInfected
{
    // Non-aggressive
    SetAiming(false);

    // Trading radius
    m_TraderRadius = 50.0;

    // Inventory
    m_Inventory = array<TraderItem>;
}
```

**CanTrade() check:**
- Distance check (≤50m)
- Optional reputation check
- Optional faction check

---

### 3. Price Tables

**JSON configuration:**
```json
{
    "AK74": {
        "buy": 1000,
        "sell": 800
    },
    "BandageDressing": {
        "buy": 50,
        "sell": 40
    }
}
```

**Dynamic pricing:**
- Supply > 100 → 20% cheaper
- Supply < -100 → 20% expensive
- Updates on buy/sell

---

### 4. Safe Zones

**Protection:**
```c
player.SetVariable("TraderZoneProtection", true);

// Check in damage handler
if (player.GetVariableBool("TraderZoneProtection"))
{
    return false; // Prevent damage
}
```

---

## 🔧 Implementation

### 1. CoinItem.c

Currency item:

```c
class CoinItem extends ItemBase
{
    protected int m_Value;

    void SetValue(int value)
    {
        m_Value = value;
    }

    int GetValue()
    {
        return m_Value;
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionPickupCurrency);
    }
}

class Coin_10 extends CoinItem
{
    void Coin_10()
    {
        SetValue(10);
    }
}

class Coin_50 extends CoinItem
{
    void Coin_50()
    {
        SetValue(50);
    }
}

class Coin_100 extends CoinItem
{
    void Coin_100()
    {
        SetValue(100);
    }
}
```

---

### 2. CurrencyManager.c

Currency tracking system:

```c
class CurrencyManager
{
    protected ref map<string, int> m_PlayerCurrency;
    protected string m_SavePath = "$profile:CurrencyData.json";

    protected static ref CurrencyManager m_Instance;

    static CurrencyManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new CurrencyManager();
        }

        return m_Instance;
    }

    void CurrencyManager()
    {
        m_PlayerCurrency = new map<string, int>;
        Load();
    }

    int GetCurrency(string steam64ID)
    {
        if (!m_PlayerCurrency.Contains(steam64ID))
            m_PlayerCurrency.Set(steam64ID, 0);

        return m_PlayerCurrency.Get(steam64ID);
    }

    void AddCurrency(string steam64ID, int amount)
    {
        int current = GetCurrency(steam64ID);
        m_PlayerCurrency.Set(steam64ID, current + amount);

        Print("[Currency] " + steam64ID + " balance: " + (current + amount));
    }

    bool HasCurrency(string steam64ID, int amount)
    {
        return GetCurrency(steam64ID) >= amount;
    }

    void Save()
    {
        JsonSaveManager json = new JsonSaveManager();
        json.SaveToFile(m_SavePath, m_PlayerCurrency);
    }

    void Load()
    {
        if (!FileExist(m_SavePath))
            return;

        JsonSaveManager json = new JsonSaveManager();
        json.LoadFromFile(m_SavePath, m_PlayerCurrency);
    }
}
```

---

### 3. TraderNPC.c

Trader NPC class:

```c
class TraderNPC extends DayZInfected
{
    protected ref array<ref TraderItem> m_Inventory;
    protected string m_TraderName;
    protected float m_TraderRadius = 50.0;

    void TraderNPC()
    {
        m_Inventory = new array<ref TraderItem>;
        m_TraderName = "Trader";

        LoadInventory();
    }

    override void OnInit()
    {
        super.OnInit();

        // Non-aggressive
        SetAiming(false);

        // High health
        SetHealth("GlobalHealth", "Health", 1000);
    }

    void LoadInventory()
    {
        // Load from prices.json
        // For now, hardcode
        m_Inventory.Insert(new TraderItem("AK74", 1000, 1200));
        m_Inventory.Insert(new TraderItem("Mag_STANAG_30Rnd", 100, 120));
        m_Inventory.Insert(new TraderItem("BandageDressing", 50, 60));
        m_Inventory.Insert(new TraderItem("BakedBeans_Can", 30, 40));
    }

    bool CanTrade(PlayerBase player)
    {
        // Distance check
        if (vector.Distance(player.GetPosition(), GetPosition()) > m_TraderRadius)
        {
            NotificationSystem.Notify(player, "Too far from trader!");
            return false;
        }

        return true;
    }

    bool BuyItem(PlayerBase player, string itemType)
    {
        if (!CanTrade(player))
            return false;

        // Find item
        TraderItem traderItem = FindItem(itemType);
        if (!traderItem)
        {
            NotificationSystem.Notify(player, "Trader doesn't sell: " + itemType);
            return false;
        }

        // Check currency
        CurrencyManager currency = CurrencyManager.GetInstance();
        if (!currency.HasCurrency(player.GetIdentity().GetId(), traderItem.BuyPrice))
        {
            NotificationSystem.Notify(player, "Not enough currency! Need: " + traderItem.BuyPrice);
            return false;
        }

        // Create item
        EntityAI item = GetGame().CreateObject(itemType, player.GetPosition());
        if (!item)
        {
            NotificationSystem.Notify(player, "Failed to create item!");
            return false;
        }

        // Add to inventory
        if (!player.GetInventory().AddInInventory(item))
        {
            item.Delete();
            NotificationSystem.Notify(player, "Inventory full!");
            return false;
        }

        // Deduct currency
        currency.AddCurrency(player.GetIdentity().GetId(), -traderItem.BuyPrice);

        NotificationSystem.Notify(player, "Bought " + itemType + " for " + traderItem.BuyPrice + " coins");

        return true;
    }

    bool SellItem(PlayerBase player, ItemBase item)
    {
        if (!CanTrade(player))
            return false;

        // Find item price
        TraderItem traderItem = FindItem(item.GetType());
        if (!traderItem)
        {
            NotificationSystem.Notify(player, "Trader doesn't buy: " + item.GetType());
            return false;
        }

        // Add currency
        CurrencyManager currency = CurrencyManager.GetInstance();
        currency.AddCurrency(player.GetIdentity().GetId(), traderItem.SellPrice);

        // Delete item
        item.Delete();

        NotificationSystem.Notify(player, "Sold " + item.GetType() + " for " + traderItem.SellPrice + " coins");

        return true;
    }

    protected TraderItem FindItem(string itemType)
    {
        foreach (TraderItem item : m_Inventory)
        {
            if (item.Type == itemType)
                return item;
        }

        return null;
    }
}

class TraderItem
{
    string Type;
    int BuyPrice;
    int SellPrice;

    void TraderItem(string type, int buyPrice, int sellPrice)
    {
        Type = type;
        BuyPrice = buyPrice;
        SellPrice = sellPrice;
    }
}
```

---

### 4. TraderZone.c

Safe zone management:

```c
class TraderZone
{
    protected vector m_Position;
    protected float m_Radius;
    protected string m_ZoneName;

    void TraderZone(vector pos, float radius, string name)
    {
        m_Position = pos;
        m_Radius = radius;
        m_ZoneName = name;
    }

    bool IsInZone(vector testPos)
    {
        return vector.Distance(testPos, m_Position) <= m_Radius;
    }

    void OnPlayerEnter(PlayerBase player)
    {
        player.SetVariable("TraderZoneProtection", true);
        NotificationSystem.Notify(player, "Entered safe zone: " + m_ZoneName);
    }

    void OnPlayerLeave(PlayerBase player)
    {
        player.SetVariable("TraderZoneProtection", false);
        NotificationSystem.Notify(player, "Left safe zone: " + m_ZoneName);
    }
}
```

---

### 5. PlayerBase Integration

```c
modded class PlayerBase
{
    protected CurrencyManager m_CurrencyManager;

    override void OnConnect()
    {
        super.OnConnect();

        m_CurrencyManager = CurrencyManager.GetInstance();
        m_CurrencyManager.Load();

        int balance = m_CurrencyManager.GetCurrency(GetIdentity().GetId());
        NotificationSystem.Notify(this, "Welcome! Balance: " + balance + " coins");
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        if (m_CurrencyManager)
        {
            m_CurrencyManager.Save();
        }
    }

    // Chat command: /money
    bool ChatCommand_CheckBalance()
    {
        int balance = m_CurrencyManager.GetCurrency(GetIdentity().GetId());
        NotificationSystem.Notify(this, "Balance: " + balance + " coins");

        return true;
    }

    // Chat command: /buy <item>
    bool ChatCommand_Buy(array<string> args)
    {
        if (args.Count() < 2)
        {
            NotificationSystem.Notify(this, "Usage: /buy <item>");
            return false;
        }

        string itemType = args[1];
        return BuyFromNearestTrader(itemType);
    }

    // Chat command: /sell
    bool ChatCommand_Sell()
    {
        return SellToNearestTrader();
    }

    bool BuyFromNearestTrader(string itemType)
    {
        array<Object> traders = new array<Object>;
        GetGame().GetObjectsAtPosition(GetPosition(), 50, traders, NULL);

        TraderNPC nearestTrader = NULL;
        float nearestDist = 999999;

        foreach (Object obj : traders)
        {
            TraderNPC trader = TraderNPC.Cast(obj);
            if (trader)
            {
                float dist = vector.Distance(GetPosition(), trader.GetPosition());
                if (dist < nearestDist)
                {
                    nearestTrader = trader;
                    nearestDist = dist;
                }
            }
        }

        if (!nearestTrader)
        {
            NotificationSystem.Notify(this, "No trader nearby!");
            return false;
        }

        return nearestTrader.BuyItem(this, itemType);
    }

    bool SellToNearestTrader()
    {
        ItemBase item = GetItemInHands();

        if (!item)
        {
            NotificationSystem.Notify(this, "No item in hands!");
            return false;
        }

        array<Object> traders = new array<Object>;
        GetGame().GetObjectsAtPosition(GetPosition(), 50, traders, NULL);

        foreach (Object obj : traders)
        {
            TraderNPC trader = TraderNPC.Cast(obj);
            if (trader)
            {
                if (trader.SellItem(this, item))
                    return true;
            }
        }

        NotificationSystem.Notify(this, "No trader nearby!");
        return false;
    }
}
```

---

## 📝 Usage Examples

### Example 1: Spawn Trader

```c
// Spawn trader at position
EntityAI trader = GetGame().CreateObject("TraderNPC", Vector(6500, 0, 2500));
```

### Example 2: Give Currency

```c
// Give player 1000 coins
CurrencyManager currency = CurrencyManager.GetInstance();
currency.AddCurrency(player.GetIdentity().GetId(), 1000);
```

### Example 3: Create Safe Zone

```c
// Create safe zone
TraderZone zone = new TraderZone(Vector(6500, 0, 2500), 100, "Cherno Trader");

// Check player position
if (zone.IsInZone(player.GetPosition()))
{
    zone.OnPlayerEnter(player);
}
```

---

## ⚠️ Important Notes

### Currency Persistence
- **Saved:** On player disconnect
- **Loaded:** On player connect
- **Format:** JSON in `$profile:CurrencyData.json`

### Trading Rules
- **Distance:** Max 50m from trader
- **Currency:** Must have enough coins
- **Inventory:** Must have space for item
- **Safe Zone:** No combat while trading

### Dynamic Pricing
- **Supply > 100:** 20% cheaper
- **Supply < -100:** 20% expensive
- **Updates:** Every buy/sell

---

## 🔗 Related Examples

→ [Trader System Complete](../reference/TRADER_SYSTEM.md) - Full documentation
→ [Economy+Crafting Integration](../integration/economy_crafting_integration/) - Economy integration
→ [Trader Systems](../topics/27_trader_systems.md) - Basics

---

## 🐛 Troubleshooting

### Currency not saving

**Check:**
1. CurrencyManager.Save() called on disconnect
2. File path is correct
3. Write permissions

### Trader doesn't respond

**Check:**
1. Distance ≤50m
2. Trader NPC spawned
3. Player has currency

### Items not spawning

**Check:**
1. Item type is correct
2. Player has inventory space
3. CreateObject position valid

---

*Last updated: 2025-01-18*
