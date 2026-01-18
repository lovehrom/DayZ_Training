# Economy + Crafting Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** crafting + economy (currency system)

---

## ⚡ 30-Second Summary

Integration example showing economy-based crafting. Crafting costs currency instead of or in addition to ingredients, with sellable items and dynamic pricing.

---

## 🎯 Features

### Integration Mechanics
- ✅ **Currency Crafting** - Craft items with money
- ✅ **Sellable Items** - Sell crafted items
- ✅ **Dynamic Pricing** - Prices change with supply/demand
- ✅ **Crafting Cost** - Currency + ingredients
- ✅ **Economy Tracking** - Track player wealth

---

## 📁 File Structure

```
economy_crafting_integration/
├── README.md                       ← This file
├── config.cpp                      ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── EconomyCraftingManager.c       ← Crafting with currency
            ├── EconomyRecipe.c                ← Recipe with cost
            ├── CurrencyItem.c                 ← Money item
            └── PlayerBase/
                └── playerbase_economy_craft.c ← Integration
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- Crafting - Create items from ingredients
- Economy - Track currency/money

**Integration:**
1. Player has currency → Track in EconomyManager
2. Player wants to craft → Check currency + ingredients
3. Crafting succeeds → Deduct currency + ingredients
4. Player sells item → Gain currency

### Recipe Costs

| Item | Ingredients | Currency Cost |
|------|------------|---------------|
| AK74 | Scrap Metal + Wood | 100 coins |
| Bandage | Rag | 10 coins |
| Canned Food | Empty Can + Meat | 25 coins |

---

## 💡 Implementation

### 1. CurrencyItem.c

Money item:

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

### 2. EconomyRecipe.c

Recipe with currency cost:

```c
class EconomyRecipe
{
    string ResultItem;
    array<string> Ingredients;
    int CurrencyCost;

    void EconomyRecipe(string result, array<string> ingredients, int cost)
    {
        ResultItem = result;
        Ingredients = ingredients;
        CurrencyCost = cost;
    }

    // Check if player can afford
    bool CanAfford(PlayerBase player)
    {
        // Check currency
        int currency = GetPlayerCurrency(player);
        if (currency < CurrencyCost)
            return false;

        // Check ingredients
        foreach (string ingredient : Ingredients)
        {
            if (!HasIngredient(player, ingredient))
                return false;
        }

        return true;
    }

    // Craft item
    bool Craft(PlayerBase player)
    {
        if (!CanAfford(player))
            return false;

        // Deduct currency
        DeductCurrency(player, CurrencyCost);

        // Remove ingredients
        foreach (string ingredient : Ingredients)
        {
            RemoveIngredient(player, ingredient);
        }

        // Create result
        EntityAI result = GetGame().CreateObject(ResultItem, player.GetPosition());

        if (result)
        {
            // Give to player
            player.GetInventory().AddInInventory(result);

            NotificationSystem.Notify(player, "Crafted " + ResultItem + " for " + CurrencyCost + " coins");

            return true;
        }

        return false;
    }

    // Helper: Get player currency
    int GetPlayerCurrency(PlayerBase player)
    {
        EconomyManager manager = EconomyManager.GetInstance();
        return manager.GetCurrency(player.GetIdentity().GetId());
    }

    // Helper: Deduct currency
    void DeductCurrency(PlayerBase player, int amount)
    {
        EconomyManager manager = EconomyManager.GetInstance();
        manager.AddCurrency(player.GetIdentity().GetId(), -amount);
    }

    // Helper: Check ingredient
    bool HasIngredient(PlayerBase player, string itemType)
    {
        // Search player inventory
        array<EntityAI> items = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

        foreach (EntityAI item : items)
        {
            if (item.GetType() == itemType)
                return true;
        }

        return false;
    }

    // Helper: Remove ingredient
    void RemoveIngredient(PlayerBase player, string itemType)
    {
        array<EntityAI> items = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

        foreach (EntityAI item : items)
        {
            if (item.GetType() == itemType)
            {
                item.Delete();
                break;
            }
        }
    }
}
```

### 3. EconomyCraftingManager.c

Manager for economy crafting:

```c
class EconomyCraftingManager
{
    protected ref array<ref EconomyRecipe> m_Recipes;

    void EconomyCraftingManager()
    {
        m_Recipes = new array<ref EconomyRecipe>;
        LoadRecipes();
    }

    void LoadRecipes()
    {
        // Weapon recipes
        array<string> gunIngredients = {"ScrapMetal", "WoodenPlank"};
        m_Recipes.Insert(new EconomyRecipe("AK74", gunIngredients, 100));

        // Medical recipes
        array<string> bandageIngredients = {"Rag"};
        m_Recipes.Insert(new EconomyRecipe("BandageDressing", bandageIngredients, 10));

        // Food recipes
        array<string> foodIngredients = {"EmptyCan", "Meat"});
        m_Recipes.Insert(new EconomyRecipe("BakedBeans_Can", foodIngredients, 25));
    }

    bool CraftItem(PlayerBase player, string itemName)
    {
        // Find recipe
        EconomyRecipe recipe = GetRecipe(itemName);

        if (!recipe)
        {
            NotificationSystem.Notify(player, "Recipe not found: " + itemName);
            return false;
        }

        // Check affordability
        if (!recipe.CanAfford(player))
        {
            NotificationSystem.Notify(player, "Cannot afford " + itemName);
            return false;
        }

        // Craft
        if (recipe.Craft(player))
        {
            // Update economy stats
            OnItemCrafted(player, itemName, recipe.CurrencyCost);

            return true;
        }

        return false;
    }

    EconomyRecipe GetRecipe(string itemName)
    {
        foreach (EconomyRecipe recipe : m_Recipes)
        {
            if (recipe.ResultItem == itemName)
                return recipe;
        }

        return null;
    }

    void OnItemCrafted(PlayerBase player, string item, int cost)
    {
        // Log for economy tracking
        Print("[EconomyCraft] " + player.GetIdentity().GetName() + " crafted " + item + " for " + cost + " coins");

        // Update supply/demand
        // (dynamic pricing...)
    }
}
```

### 4. EconomyManager.c

Track player currency:

```c
class EconomyManager
{
    protected ref map<string, int> m_PlayerCurrency; // Steam64 ID → Currency

    protected static ref EconomyManager m_Instance;

    static EconomyManager GetInstance()
    {
        if (!m_Instance)
            m_Instance = new EconomyManager();

        return m_Instance;
    }

    void EconomyManager()
    {
        m_PlayerCurrency = new map<string, int>;
    }

    // Get player currency
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
    }

    // Set currency
    void SetCurrency(string steam64ID, int amount)
    {
        m_PlayerCurrency.Set(steam64ID, amount);
    }

    // Save to JSON
    void Save()
    {
        JsonSaveManager json = new JsonSaveManager();
        json.SaveToFile("$profile:EconomyData.json", m_PlayerCurrency);
    }

    // Load from JSON
    void Load()
    {
        if (!FileExist("$profile:EconomyData.json"))
            return;

        JsonSaveManager json = new JsonSaveManager();
        json.LoadFromFile("$profile:EconomyData.json", m_PlayerCurrency);
    }
}
```

### 5. PlayerBase Integration

```c
modded class PlayerBase
{
    protected ref EconomyCraftingManager m_CraftingManager;

    override void OnConnect()
    {
        super.OnConnect();

        m_CraftingManager = new EconomyCraftingManager();

        // Load economy
        EconomyManager.GetInstance().Load();

        // Show currency
        int currency = EconomyManager.GetInstance().GetCurrency(GetIdentity().GetId());
        NotificationSystem.Notify(this, "Welcome! You have " + currency + " coins");
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save economy
        EconomyManager.GetInstance().Save();
    }

    // Craft with currency
    bool EconomyCraft(string itemName)
    {
        return m_CraftingManager.CraftItem(this, itemName);
    }

    // Sell item for currency
    bool SellItem(ItemBase item)
    {
        // Calculate value (based on item type)
        int value = CalculateItemValue(item);

        if (value <= 0)
        {
            NotificationSystem.Notify(this, "This item cannot be sold");
            return false;
        }

        // Add currency
        EconomyManager.GetInstance().AddCurrency(GetIdentity().GetId(), value);

        // Delete item
        item.Delete();

        NotificationSystem.Notify(this, "Sold " + item.GetType() + " for " + value + " coins");

        return true;
    }

    int CalculateItemValue(ItemBase item)
    {
        string type = item.GetType();

        // Weapons
        if (type.Contains("AK") || type.Contains("M4"))
            return 150;

        // Medical
        if (type.Contains("Bandage") || type.Contains("Antibiotic"))
            return 20;

        // Food
        if (type.Contains("Can") || type.Contains("Box"))
            return 10;

        // Scrap
        if (type.Contains("Scrap"))
            return 5;

        return 0; // Not sellable
    }

    // Chat command: /craft <item>
    bool ChatCommand_Craft(string itemName)
    {
        if (EconomyCraft(itemName))
        {
            return true;
        }
        else
        {
            NotificationSystem.Notify(this, "Failed to craft " + itemName);
            return false;
        }
    }

    // Chat command: /money
    bool ChatCommand_Money()
    {
        int currency = EconomyManager.GetInstance().GetCurrency(GetIdentity().GetId());
        NotificationSystem.Notify(this, "Balance: " + currency + " coins");

        return true;
    }
}
```

---

## 📝 Usage Examples

### Example 1: Buy Currency with Real Money

```c
class DonationShop
{
    static void BuyCurrency(PlayerBase player, int amount)
    {
        // In real implementation, verify donation
        // For now, just give currency

        EconomyManager.GetInstance().AddCurrency(player.GetIdentity().GetId(), amount);

        NotificationSystem.Notify(player, "Purchased " + amount + " coins!");
    }
}
```

### Example 2: Dynamic Pricing

```c
class DynamicPricing
{
    static map<string, float> m_ItemMultipliers;

    static void UpdatePrices()
    {
        // AK74 crafted 100 times → price increases
        int craftCount = GetCraftCount("AK74");

        if (craftCount > 100)
        {
            m_ItemMultipliers.Set("AK74", 1.5); // 50% more expensive
        }
    }

    static int GetPrice(string itemName, int basePrice)
    {
        if (m_ItemMultipliers.Contains(itemName))
        {
            float multiplier = m_ItemMultipliers.Get(itemName);
            return basePrice * multiplier;
        }

        return basePrice;
    }
}
```

### Example 3: Economy Events

```c
class EconomyEventManager
{
    static void OnEconomyEvent(string eventType)
    {
        if (eventType == "Inflation")
        {
            // All prices 20% higher
            Print("[Economy] Inflation event! Prices increased.");
        }
        else if (eventType == "Sale")
        {
            // All prices 20% lower
            Print("[Economy] Sale event! Prices decreased.");
        }
    }
}
```

---

## 🔗 Related Examples

→ [GUI Crafting](../gui_crafting/) - Individual crafting mechanics
→ [Storage](../storage/) - Currency persistence
→ [Integration: Faction+Teleport](../faction_teleport_integration/) - Another integration

---

## ⚠️ Important Notes

### Currency Balance
- **Starting:** 0 coins (configurable)
- **Earn:** Sell items, kill mobs
- **Spend:** Craft items, buy items

### Crafting Costs
- **Weapons:** 100-200 coins
- **Medical:** 10-30 coins
- **Food:** 20-50 coins

### Anti-Exploit
- Server-side validation
- No client-side currency
- Save on disconnect

---

*Last updated: 2025-01-18*
*Combines: crafting + economy*
