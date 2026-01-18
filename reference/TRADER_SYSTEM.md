# Trader System - Complete Reference

**Status:** ✅ Complete Reference
**Source:** Community Framework + Economy Integration Examples
**Complexity:** Advanced

---

## ⚡ 30-Second Summary

Complete trader system implementation guide. Covers NPC traders, currency system, price tables, trading zones, buy/sell mechanics, UI integration, and network synchronization.

---

## 🎯 Overview

### What is a Trader System?

A trader system allows players to:
- Buy items from NPC traders using currency
- Sell items to traders for currency
- Trade in safe zones (no combat)
- Access dynamic pricing based on supply/demand

### Key Components

1. **Currency System** - Money/coins tracking
2. **Trader NPC** - AI-controlled merchants
3. **Price Tables** - Buy/sell prices
4. **Trading Actions** - Buy/sell mechanics
5. **Trader Zones** - Safe trading areas
6. **UI System** - Player interaction interface

---

## 📁 File Structure

```
trader_system/
├── config.cpp                   ← Trader definitions, currency items
├── prices.json                  ← Price table
├── stock.json                   ← Stock limits
└── scripts/
    ├── 3_Game/
    │   ├── TraderManager.c      ← Main trader manager
    │   ├── CurrencySystem.c     ← Currency tracking
    │   ├── PriceManager.c       ← Price management
    │   └── TradeActions.c        ← Buy/sell logic
    ├── 4_World/
    │   ├── TraderNPC.c          ← Trader NPC class
    │   ├── TraderZone.c         ← Safe zone management
    │   ├── CoinItem.c           ← Currency items
    │   └── CurrencyItem.c       ← Base currency class
    └── 5_Mission/
        └── TraderMenu.c         ← UI interaction
```

---

## 💰 Currency System

### Currency Item Class

```c
class CurrencyItem extends ItemBase
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
```

### Coin Items (Different Denominations)

```c
class Coin_10 extends CurrencyItem
{
    void Coin_10()
    {
        SetValue(10);
    }
}

class Coin_50 extends CurrencyItem
{
    void Coin_50()
    {
        SetValue(50);
    }
}

class Coin_100 extends CurrencyItem
{
    void Coin_100()
    {
        SetValue(100);
    }
}
```

### Currency Manager

```c
class CurrencyManager
{
    protected ref map<string, int> m_PlayerCurrency; // Steam64 ID → Balance
    protected string m_SavePath = "$profile:CurrencyData.json";

    static ref CurrencyManager m_Instance;

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

    // Get player balance
    int GetCurrency(string steam64ID)
    {
        if (!m_PlayerCurrency.Contains(steam64ID))
            m_PlayerCurrency.Set(steam64ID, 0); // Start with 0

        return m_PlayerCurrency.Get(steam64ID);
    }

    // Add currency
    void AddCurrency(string steam64ID, int amount)
    {
        int current = GetCurrency(steam64ID);
        m_PlayerCurrency.Set(steam64ID, current + amount);

        Print("[Currency] " + steam64ID + " balance: " + (current + amount));
    }

    // Check if player has enough
    bool HasCurrency(string steam64ID, int amount)
    {
        return GetCurrency(steam64ID) >= amount;
    }

    // Save to JSON
    void Save()
    {
        JsonSaveManager json = new JsonSaveManager();
        json.SaveToFile(m_SavePath, m_PlayerCurrency);
    }

    // Load from JSON
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

## 🏪 Trader NPC

### Trader NPC Class

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

        // Set trader behavior
        SetBehaviorMode("Trader");

        // Don't attack players
        SetAiming(false);
    }

    void LoadInventory()
    {
        // Add items to sell
        m_Inventory.Insert(new TraderItem("AK74", 1000, 1200)); // Buy 1000, Sell 1200
        m_Inventory.Insert(new TraderItem("Mag_STANAG_30Rnd", 100, 120));
        m_Inventory.Insert(new TraderItem("BandageDressing", 50, 60));
        m_Inventory.Insert(new TraderItem("BakedBeans_Can", 30, 40));
        m_Inventory.Insert(new TraderItem("SodaCan_Cola", 25, 35));
    }

    // Check if player can trade
    bool CanTrade(PlayerBase player)
    {
        // Check distance
        if (vector.Distance(player.GetPosition(), GetPosition()) > m_TraderRadius)
        {
            NotificationSystem.Notify(player, "Too far from trader!");
            return false;
        }

        // Add additional checks (reputation, etc.)
        return true;
    }

    // Buy item from trader
    bool BuyItem(PlayerBase player, string itemType)
    {
        if (!CanTrade(player))
            return false;

        // Find item in inventory
        TraderItem traderItem = FindItem(itemType);
        if (!traderItem)
        {
            NotificationSystem.Notify(player, "Trader doesn't sell: " + itemType);
            return false;
        }

        // Check player currency
        CurrencyManager currency = CurrencyManager.GetInstance();
        if (!currency.HasCurrency(player.GetIdentity().GetId(), traderItem.BuyPrice))
        {
            NotificationSystem.Notify(player, "Not enough currency! Need: " + traderItem.BuyPrice);
            return false;
        }

        // Create item and give to player
        EntityAI item = GetGame().CreateObject(itemType, player.GetPosition());
        if (!item)
        {
            NotificationSystem.Notify(player, "Failed to create item!");
            return false;
        }

        // Add to player inventory
        if (!player.GetInventory().AddInInventory(item))
        {
            item.Delete();
            NotificationSystem.Notify(player, "Inventory full!");
            return false;
        }

        // Deduct currency
        currency.AddCurrency(player.GetIdentity().GetId(), -traderItem.BuyPrice);

        // Notify
        NotificationSystem.Notify(player, "Bought " + itemType + " for " + traderItem.BuyPrice + " coins");

        return true;
    }

    // Sell item to trader
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

        // Notify
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
```

### Trader Item Class

```c
class TraderItem
{
    string Type;           // Item type (e.g., "AK74")
    int BuyPrice;          // Price to buy from trader
    int SellPrice;         // Price to sell to trader
    int Stock;             // Current stock (-1 = unlimited)

    void TraderItem(string type, int buyPrice, int sellPrice, int stock = -1)
    {
        Type = type;
        BuyPrice = buyPrice;
        SellPrice = sellPrice;
        Stock = stock;
    }
}
```

---

## 📊 Price Management

### Price Manager

```c
class PriceManager
{
    protected ref map<string, int> m_BasePrices;
    protected ref map<string, float> m_PriceMultipliers;
    protected ref map<string, int> m_SupplyDemand;

    void PriceManager()
    {
        m_BasePrices = new map<string, int>;
        m_PriceMultipliers = new map<string, float>;
        m_SupplyDemand = new map<string, int>;

        LoadPrices();
    }

    void LoadPrices()
    {
        // Example: Load from JSON
        m_BasePrices.Set("AK74", 1000);
        m_BasePrices.Set("Mag_STANAG_30Rnd", 100);
        m_BasePrices.Set("BandageDressing", 50);
        m_BasePrices.Set("BakedBeans_Can", 30);

        // Default multipliers
        m_PriceMultipliers.Set("AK74", 1.0);
        m_PriceMultipliers.Set("Mag_STANAG_30Rnd", 1.0);
        m_PriceMultipliers.Set("BandageDressing", 1.0);
        m_PriceMultipliers.Set("BakedBeans_Can", 1.0);

        // Supply/demand tracking
        m_SupplyDemand.Set("AK74", 0);
        m_SupplyDemand.Set("Mag_STANAG_30Rnd", 0);
        m_SupplyDemand.Set("BandageDressing", 0);
        m_SupplyDemand.Set("BakedBeans_Can", 0);
    }

    int GetBuyPrice(string itemType)
    {
        int basePrice = m_BasePrices.Get(itemType, 0);
        float multiplier = m_PriceMultipliers.Get(itemType, 1.0);

        return basePrice * multiplier;
    }

    int GetSellPrice(string itemType)
    {
        // Sell price is typically 80% of buy price
        return GetBuyPrice(itemType) * 0.8;
    }

    void OnItemBought(string itemType)
    {
        // Decrease supply (increase demand)
        int current = m_SupplyDemand.Get(itemType, 0);
        m_SupplyDemand.Set(itemType, current - 1);

        UpdatePriceMultiplier(itemType);
    }

    void OnItemSold(string itemType)
    {
        // Increase supply (decrease demand)
        int current = m_SupplyDemand.Get(itemType, 0);
        m_SupplyDemand.Set(itemType, current + 1);

        UpdatePriceMultiplier(itemType);
    }

    protected void UpdatePriceMultiplier(string itemType)
    {
        int supply = m_SupplyDemand.Get(itemType, 0);
        float multiplier = 1.0;

        // High supply → lower price
        if (supply > 100)
            multiplier = 0.8;
        // Low supply → higher price
        else if (supply < -100)
            multiplier = 1.2;

        m_PriceMultipliers.Set(itemType, multiplier);

        Print("[Price] " + itemType + " multiplier: " + multiplier + " (supply: " + supply + ")");
    }
}
```

---

## 🛡️ Trader Zones

### Trader Zone Class

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
        // Enable protection
        player.SetVariable("TraderZoneProtection", true);

        // Notify
        NotificationSystem.Notify(player, "Entered safe zone: " + m_ZoneName);
    }

    void OnPlayerLeave(PlayerBase player)
    {
        // Disable protection
        player.SetVariable("TraderZoneProtection", false);

        // Notify
        NotificationSystem.Notify(player, "Left safe zone: " + m_ZoneName);
    }
}
```

### Zone Manager

```c
class ZoneManager
{
    protected ref array<ref TraderZone> m_Zones;

    void ZoneManager()
    {
        m_Zones = new array<ref TraderZone>;

        // Create zones
        m_Zones.Insert(new TraderZone(Vector(6500, 0, 2500), 100, "Cherno Trader"));
        m_Zones.Insert(new TraderZone(Vector(5000, 0, 2500), 100, "Electro Trader"));
    }

    void CheckPlayers(PlayerBase player)
    {
        foreach (TraderZone zone : m_Zones)
        {
            if (zone.IsInZone(player.GetPosition()))
            {
                // Player entered zone
                if (!player.GetVariableBool("InTraderZone"))
                {
                    zone.OnPlayerEnter(player);
                    player.SetVariable("InTraderZone", true);
                }
            }
            else
            {
                // Player left zone
                if (player.GetVariableBool("InTraderZone"))
                {
                    zone.OnPlayerLeave(player);
                    player.SetVariable("InTraderZone", false);
                }
            }
        }
    }
}
```

---

## 🎮 PlayerBase Integration

### Currency Integration

```c
modded class PlayerBase
{
    protected CurrencyManager m_CurrencyManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Get currency manager
        m_CurrencyManager = CurrencyManager.GetInstance();

        // Show balance
        int balance = m_CurrencyManager.GetCurrency(GetIdentity().GetId());
        NotificationSystem.Notify(this, "Welcome! Balance: " + balance + " coins");
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save currency
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

    // Get currency manager
    CurrencyManager GetCurrencyManager()
    {
        return m_CurrencyManager;
    }
}
```

### Trading Actions

```c
modded class PlayerBase
{
    // Buy item from nearest trader
    bool BuyFromNearestTrader(string itemType)
    {
        // Find nearest trader
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

        // Buy item
        return nearestTrader.BuyItem(this, itemType);
    }

    // Sell item in hands
    bool SellToNearestTrader()
    {
        ItemBase item = GetItemInHands();

        if (!item)
        {
            NotificationSystem.Notify(this, "No item in hands!");
            return false;
        }

        // Find nearest trader
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
}
```

---

## 📡 RPC Network Synchronization

### RPC IDs

```c
class TRaderRPC
{
    static const int RPC_BUY_ITEM = 5000;
    static const int RPC_SELL_ITEM = 5001;
    static const int RPC_OPEN_MENU = 5002;
    static const int RPC_CLOSE_MENU = 5003;
}
```

### Server-Side RPC Handler

```c
modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        switch (rpc_type)
        {
            case TRaderRPC.RPC_BUY_ITEM:
                OnRPC_BuyItem(sender, ctx);
                break;

            case TRaderRPC.RPC_SELL_ITEM:
                OnRPC_SellItem(sender, ctx);
                break;
        }
    }

    void OnRPC_BuyItem(PlayerIdentity sender, ParamsReadContext ctx)
    {
        // Read parameters
        Param1<string> itemType = new Param1<string>("");
        ctx.Read(itemType);

        // Execute buy
        bool success = BuyFromNearestTrader(itemType.param1);

        // Send result back to client
        Param1<bool> result = new Param1<bool>(success);
        GetGame().RPCSingleParam(this, TRaderRPC.RPC_BUY_ITEM, result, true, sender);
    }

    void OnRPC_SellItem(PlayerIdentity sender, ParamsReadContext ctx)
    {
        // Execute sell
        bool success = SellToNearestTrader();

        // Send result back to client
        Param1<bool> result = new Param1<bool>(success);
        GetGame().RPCSingleParam(this, TRaderRPC.RPC_SELL_ITEM, result, true, sender);
    }
}
```

### Client-Side Action

```c
class ActionBuyItem extends ActionContinuousBase
{
    protected string m_ItemType;
    protected TraderNPC m_Trader;

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        // Find trader
        m_Trader = TraderNPC.Cast(target.GetObject());

        if (!m_Trader)
            return false;

        // Check if player can trade
        return m_Trader.CanTrade(player);
    }

    override void Start(ActionData action_data)
    {
        super.Start(action_data);

        // Send buy request to server
        Param1<string> param = new Param1<string>(m_ItemType);
        GetGame().RPCSingleParam(action_data.m_Player, TRaderRPC.RPC_BUY_ITEM, param, true);
    }
}
```

---

## 📝 JSON Configuration

### prices.json

```json
{
    "items": [
        {
            "type": "AK74",
            "buyPrice": 1000,
            "sellPrice": 800
        },
        {
            "type": "Mag_STANAG_30Rnd",
            "buyPrice": 100,
            "sellPrice": 80
        },
        {
            "type": "BandageDressing",
            "buyPrice": 50,
            "sellPrice": 40
        },
        {
            "type": "BakedBeans_Can",
            "buyPrice": 30,
            "sellPrice": 25
        }
    ]
}
```

### stock.json

```json
{
    "stock": [
        {
            "type": "AK74",
            "maxStock": 10,
            "respawnTime": 3600
        },
        {
            "type": "Mag_STANAG_30Rnd",
            "maxStock": -1,
            "respawnTime": 0
        }
    ]
}
```

---

## ⚠️ Security Best Practices

### Validate All Transactions

```c
bool BuyItem(PlayerBase player, string itemType, int price)
{
    // 1. Validate player
    if (!player)
        return false;

    // 2. Validate item type
    if (itemType == "" || !IsValidItemType(itemType))
        return false;

    // 3. Validate price
    if (price < 0)
        return false;

    // 4. Check player has currency (SERVER-SIDE)
    if (!m_CurrencyManager.HasCurrency(player.GetIdentity().GetId(), price))
        return false;

    // 5. Create item and verify
    EntityAI item = GetGame().CreateObject(itemType, player.GetPosition());
    if (!item)
        return false;

    // 6. Add to inventory and verify
    if (!player.GetInventory().AddInInventory(item))
    {
        item.Delete();
        return false;
    }

    // 7. Deduct currency (SERVER-SIDE)
    m_CurrencyManager.AddCurrency(player.GetIdentity().GetId(), -price);

    // 8. Log transaction
    LogTransaction(player, itemType, price, "BUY");

    return true;
}
```

### Rate Limiting

```c
class TradeRateLimiter
{
    protected map<string, float> m_LastTradeTime;

    bool CanTrade(PlayerBase player, float cooldown)
    {
        string playerID = player.GetIdentity().GetId();
        float currentTime = GetGame().GetTickTime();

        if (m_LastTradeTime.Contains(playerID))
        {
            float lastTime = m_LastTradeTime.Get(playerID);

            if (currentTime - lastTime < cooldown)
            {
                NotificationSystem.Notify(player, "Trade on cooldown!");
                return false;
            }
        }

        m_LastTradeTime.Set(playerID, currentTime);
        return true;
    }
}
```

---

## 🔗 Related Topics

→ [Economy System](../topics/05_economy.md) - Economy basics
→ [AI System](../reference/AI_SYSTEM.md) - NPC creation
→ [RPC Networking](../reference/CF_RPC_SYSTEM.md) - Network communication
→ [Integration: Economy+Crafting](../examples/integration/economy_crafting_integration/) - Economy examples

---

## 📚 Additional Resources

- **Community Framework:** Base framework for traders
- **Expansion Mod:** Advanced trader UI
- **DayZ Epoch:** Classic trader system

---

*Last updated: 2025-01-18*
