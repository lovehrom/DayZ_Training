# Trader Systems - Introduction

**Status:** ✅ Complete Guide
**Target:** Beginner to intermediate modders
**Prerequisites:** [Config](02_config.md), [Items](01_items.md), [AI](20_zombie_ai.md)

---

## ⚡ 30-Second Summary

Trader systems allow NPCs to buy and sell items from players using currency. This guide covers the basics of setting up a trader system in DayZ.

---

## 🎯 What is a Trader System?

A trader system enables:
- **NPC Traders** - AI-controlled merchants
- **Currency** - Coins/money for trading
- **Buying** - Players purchase items
- **Selling** - Players sell items for currency
- **Safe Zones** - No-combat trading areas

**Popular in:** DayZ Epoch, Exile, Expansion Mod

---

## 📁 Basic Structure

```
YourMod/
├── config.cpp           ← Define currency items
└── scripts/
    └── 4_World/
        ├── TraderNPC.c  ← Trader NPC
        └── CoinItem.c    ← Currency item
```

---

## 💡 Quick Start: 5-Minute Trader

### Step 1: Create Currency Item

**config.cpp:**
```cpp
class CfgVehicles
{
    class ItemBase;
    class CoinItem: ItemBase
    {
        scope = 2;
        displayName = "Coin";
        descriptionShort = "A valuable coin used for trading";
        weight = 1;
    };
};
```

### Step 2: Create Trader NPC

**scripts/4_World/TraderNPC.c:**
```c
class TraderNPC extends DayZInfected
{
    void TraderNPC()
    {
        // Don't attack players
        SetAiming(false);
    }

    bool CanTrade(PlayerBase player)
    {
        // Check distance (50m max)
        if (vector.Distance(player.GetPosition(), GetPosition()) > 50)
            return false;

        return true;
    }
}
```

### Step 3: Add Trading Action

**scripts/4_World/Actions/ActionTrade.c:**
```c
class ActionTrade extends ActionSingleBase
{
    void ActionTrade()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACT;
        m_Text = "Trade";
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        TraderNPC trader = TraderNPC.Cast(target.GetObject());

        if (!trader)
            return false;

        return trader.CanTrade(player);
    }

    override void Start(ActionData action_data)
    {
        super.Start(action_data);

        NotificationSystem.Notify(action_data.m_Player, "Trading with NPC!");
    }
}
```

---

## 💰 Currency System

### Currency Items

**Different denominations:**
```cpp
class CfgVehicles
{
    class ItemBase;

    class CoinItem: ItemBase
    {
        scope = 2;
        displayName = "Coin";
        weight = 1;
    };

    class Coin_10: CoinItem
    {
        displayName = "10 Coins";
        value = 10;
    };

    class Coin_50: CoinItem
    {
        displayName = "50 Coins";
        value = 50;
    };

    class Coin_100: CoinItem
    {
        displayName = "100 Coins";
        value = 100;
    };
};
```

### Currency Manager

```c
class CurrencyManager
{
    protected map<string, int> m_Balance;

    static CurrencyManager GetInstance()
    {
        if (!m_Instance)
            m_Instance = new CurrencyManager();

        return m_Instance;
    }

    int GetBalance(string playerID)
    {
        if (!m_Balance.Contains(playerID))
            m_Balance.Set(playerID, 0);

        return m_Balance.Get(playerID);
    }

    void AddCurrency(string playerID, int amount)
    {
        int current = GetBalance(playerID);
        m_Balance.Set(playerID, current + amount);
    }

    bool HasCurrency(string playerID, int amount)
    {
        return GetBalance(playerID) >= amount;
    }
}
```

---

## 🏪 Trader NPC

### Basic Trader

```c
class TraderNPC extends DayZInfected
{
    protected array<string> m_ItemsForSale;

    void TraderNPC()
    {
        m_ItemsForSale = new array<string>;

        // Add items to sell
        m_ItemsForSale.Insert("AK74");
        m_ItemsForSale.Insert("BandageDressing");
        m_ItemsForSale.Insert("BakedBeans_Can");
    }

    override void OnInit()
    {
        super.OnInit();

        // Don't attack
        SetAiming(false);
        SetHealth("GlobalHealth", "Health", 1000);
    }

    bool BuyItem(PlayerBase player, string itemType)
    {
        // Check if trader sells this item
        if (!m_ItemsForSale.Find(itemType))
            return false;

        // Create item
        EntityAI item = GetGame().CreateObject(itemType, player.GetPosition());

        if (!item)
            return false;

        // Give to player
        if (!player.GetInventory().AddInInventory(item))
        {
            item.Delete();
            return false;
        }

        NotificationSystem.Notify(player, "Bought: " + itemType);
        return true;
    }
}
```

---

## 📊 Price Tables

### Simple Price Table

```c
class PriceTable
{
    protected map<string, int> m_Prices;

    int GetPrice(string itemType)
    {
        return m_Prices.Get(itemType, 0);
    }

    void PriceTable()
    {
        m_Prices = new map<string, int>;

        // Weapons
        m_Prices.Set("AK74", 1000);
        m_Prices.Set("M4A1", 1200);

        // Medical
        m_Prices.Set("BandageDressing", 50);
        m_Prices.Set("Antibiotic", 200);

        // Food
        m_Prices.Set("BakedBeans_Can", 30);
        m_Prices.Set("SodaCan_Cola", 25);
    }
}
```

---

## 🛡️ Trader Zones

### Safe Zone

```c
class TraderZone
{
    protected vector m_Position;
    protected float m_Radius = 100.0;

    bool IsInSafeZone(vector pos)
    {
        return vector.Distance(pos, m_Position) <= m_Radius;
    }
}

// Check on player update
modded class PlayerBase
{
    protected TraderZone m_TraderZone;

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Check if in trader zone
        if (m_TraderZone && m_TraderZone.IsInSafeZone(GetPosition()))
        {
            // Disable combat
            SetVariable("TraderZoneProtection", true);
        }
    }
}
```

---

## 🎮 Player Integration

### Currency Tracking

```c
modded class PlayerBase
{
    protected CurrencyManager m_CurrencyManager;

    override void OnConnect()
    {
        super.OnConnect();

        m_CurrencyManager = CurrencyManager.GetInstance();
        m_CurrencyManager.Load();
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        if (m_CurrencyManager)
            m_CurrencyManager.Save();
    }
}
```

---

## 📝 Usage Examples

### Example 1: Buy Item with Currency

```c
bool BuyItem(PlayerBase player, string itemType, int price)
{
    CurrencyManager currency = CurrencyManager.GetInstance();

    // Check balance
    if (!currency.HasCurrency(player.GetIdentity().GetId(), price))
    {
        NotificationSystem.Notify(player, "Not enough currency!");
        return false;
    }

    // Create item
    EntityAI item = GetGame().CreateObject(itemType, player.GetPosition());

    if (!item || !player.GetInventory().AddInInventory(item))
    {
        NotificationSystem.Notify(player, "Failed to buy item!");
        return false;
    }

    // Deduct currency
    currency.AddCurrency(player.GetIdentity().GetId(), -price);

    NotificationSystem.Notify(player, "Bought " + itemType);
    return true;
}
```

### Example 2: Sell Item for Currency

```c
bool SellItem(PlayerBase player, ItemBase item, int price)
{
    if (!item)
        return false;

    // Check if player has item
    if (!player.GetInventory().HasItem(item))
        return false;

    // Add currency
    CurrencyManager currency = CurrencyManager.GetInstance();
    currency.AddCurrency(player.GetIdentity().GetId(), price);

    // Delete item
    item.Delete();

    NotificationSystem.Notify(player, "Sold for " + price + " coins");
    return true;
}
```

---

## ⚠️ Common Issues

### Issue: Trader doesn't sell item

**Solution:** Check item is in trader's inventory:
```c
m_ItemsForSale.Insert("AK74"); // Make sure item is listed
```

### Issue: Currency not saving

**Solution:** Save on disconnect:
```c
override void OnDisconnect()
{
    super.OnDisconnect();
    m_CurrencyManager.Save(); // Don't forget!
}
```

### Issue: Players attack in safe zone

**Solution:** Check zone protection:
```c
if (player.GetVariableBool("TraderZoneProtection"))
{
    // Prevent damage
    return false;
}
```

---

## 🔗 Related Topics

→ [Trader System Complete](../reference/TRADER_SYSTEM.md) - Full documentation
→ [Economy](05_economy.md) - Currency basics
→ [AI](20_zombie_ai.md) - NPC creation
→ [Integration: Economy+Crafting](../examples/integration/economy_crafting_integration/) - Examples

---

## 🚀 Next Steps

1. **Read:** [Trader System Complete](../reference/TRADER_SYSTEM.md)
2. **Example:** [Economy+Crafting Integration](../examples/integration/economy_crafting_integration/)
3. **Try:** Create your first trader NPC

---

*Last updated: 2025-01-18*
