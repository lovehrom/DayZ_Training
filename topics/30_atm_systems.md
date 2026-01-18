# ATM Systems for Beginners

**Status:** ✅ Beginner Guide
**Difficulty:** Beginner → Intermediate
**Time to Implement:** 2-4 hours
**Related:** [ATM System Reference](../reference/ATM_SYSTEM.md) | [Banking Example](../examples/atm_system/)

---

## ⚡ 30-Second Summary

ATM (Automated Teller Machine) systems allow players to store currency in bank accounts and perform transactions through physical ATM objects in the game world. Players can deposit, withdraw, and transfer money using ATMs or credit cards.

---

## 🎯 What You'll Learn

1. **What is an ATM System?** - Understanding banking in DayZ
2. **Basic ATM Setup** - Creating your first ATM
3. **Credit Cards** - How they work
4. **Bank Accounts** - Storing player money
5. **Common Issues** - Troubleshooting

---

## 💡 What is an ATM System?

An ATM system in DayZ consists of:

- **ATM Objects** - Physical machines in the game world (like at traders)
- **Bank Accounts** - Virtual storage for player currency (persisted across server restarts)
- **Credit Cards** - Optional items required for transactions
- **Transaction Queue** - Handles multiple players using ATMs simultaneously

### Why Use an ATM System?

**Benefits:**
- ✅ Players don't lose all money on death
- ✅ Secure storage (can't be stolen)
- ✅ Convenient money transfer between players
- ✅ Supports economy mods (traders, auctions)

**Drawbacks:**
- ❌ Requires database storage (JSON files)
- ❌ Adds complexity to economy
- ❌ Requires additional UI

---

## 🔧 Basic ATM Setup

### Step 1: Define ATM Entity

Create an ATM object using a building or entity with a special `TraderID = -2`:

```cpp
// config.cpp
class CfgPatches
{
    class MyMod_ATM
    {
        units[] = {"Land_ATM"};
        weapons[] = {};
        requiredVersion = 0.1;
    };
};

class CfgVehicles
{
    class Land_Deutshe;
    class Land_ATM: Land_Deutshe
    {
        scope = 2;  // 0 = hidden, 1 = protected, 2 = public
        displayName = "ATM";
        model = "\dz\structures\commercial\deutshe\deutshe.p3d";

        // CRITICAL: This identifies it as an ATM!
        TraderID = -2;
    };
};
```

**Key Points:**
- `TraderID = -2` tells the game this is an ATM
- Use any building model (Deutshe is common)
- `scope = 2` makes it spawn-able in Eden Editor

---

### Step 2: Create Bank Account Data

Create a class to store player bank accounts:

```cpp
// scripts/4_World/Classes/BankAccountData.c
class BankAccountData
{
    string SteamID64;      // Player's unique ID
    int Balance;           // Current money
    int MaxBalance;        // Maximum allowed (optional)
    string Name;           // Player name (for display)

    void Save(string playerID)
    {
        // Save to JSON file
        string filePath = "$profile:Banking/" + playerID + ".json";
        JsonFileLoader<BankAccountData>.JsonSaveFile(filePath, this);
    }

    static BankAccountData Load(string playerID)
    {
        string filePath = "$profile:Banking/" + playerID + ".json";

        if (!FileExist(filePath))
        {
            // Create new account with default balance
            BankAccountData newAccount = new BankAccountData();
            newAccount.SteamID64 = playerID;
            newAccount.Balance = 0;
            newAccount.MaxBalance = 1000000;
            newAccount.Save(playerID);
            return newAccount;
        }

        // Load existing account
        BankAccountData data = new BankAccountData();
        JsonFileLoader<BankAccountData>.JsonLoadFile(filePath, data);
        return data;
    }
}
```

**Key Points:**
- Uses `SteamID64` for unique player identification
- Saves to `$profile:Banking/` folder
- Auto-creates new account for first-time players
- Maximum balance prevents inflation (optional)

---

### Step 3: Create ATM Interaction Action

Allow players to open the bank menu when near an ATM:

```cpp
// scripts/4_World/Classes/Action/ActionOpenBankMenu.c
class ActionOpenBankMenu: ActionInteractBase
{
    void ActionOpenBankMenu()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_HUDCursorIcon = CursorIcons.CloseHood;
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTObject(10);  // 10 meter range
        m_ConditionItem = new CCINone;          // No item required in hands
    }

    override string GetText()
    {
        return "Open Bank Account";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        // Check if target is an ATM (TraderID = -2)
        BuildingBase building = BuildingBase.Cast(target.GetObject());

        if (building && building.TraderID == -2)
        {
            return true;  // Show action
        }

        return false;  // Don't show action
    }

    override void OnStartClient(ActionData action_data)
    {
        // Open bank menu
        OpenBankMenu();
    }

    void OpenBankMenu()
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        // Get player's bank account
        string playerID = player.GetIdentity().GetPlainId();
        BankAccountData account = BankAccountData.Load(playerID);

        // Send account data to client
        GetRPCManager().SendRPC("MyMod_Banking", "GetBankAccount",
            new Param1<ref BankAccountData>(account), true, player.GetIdentity());

        // Show bank UI
        GetGame().GetUIManager().ShowScriptedMenu(player.m_BankMenu, NULL);
    }
}
```

**Key Points:**
- Only shows action when looking at ATM (TraderID = -2)
- 10-meter interaction range
- Loads player's bank account from JSON
- Opens bank menu UI

---

### Step 4: Register the Action

Add the action to `config.cpp`:

```cpp
class CfgActions
{
    class ActionOpenBankMenu
    {
        type = "interact";
        priority = 0.5;
        condition = "true";
    };
};
```

---

## 💳 Credit Card System (Optional)

### What are Credit Cards?

Credit cards are **optional** items that players must hold to use ATMs. This adds realism and prevents unauthorized access.

### Create Credit Card Item

```cpp
// config.cpp
class CfgVehicles
{
    class ItemBase;
    class CreditCard: ItemBase
    {
        scope = 2;
        displayName = "Credit Card";
        descriptionShort = "A bank card for accessing ATMs";
        weight = 10;  // grams

        // Item appearance
        model = "\dz\weapons\prototypes\card.p3d";

        // Item properties
        consumable = 0;  // Not consumable
        usable = 1;      // Can be used
    };
};
```

### Require Credit Card for ATM Use

Modify the action condition to check for credit card:

```cpp
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
    BuildingBase building = BuildingBase.Cast(target.GetObject());

    if (building && building.TraderID == -2)
    {
        // Check if credit card is required
        if (GetBankingConfig().RequireCreditCard)
        {
            ItemBase itemInHands = player.GetHumanInventory().GetEntityInHands();

            // Must hold credit card
            if (!itemInHands || !itemInHands.IsKindOf("CreditCard"))
            {
                return false;  // Don't show action
            }
        }

        return true;
    }

    return false;
}
```

**Key Points:**
- Set `RequireCreditCard` in config (true/false)
- Player must hold credit card in hands
- Good for roleplay servers

---

## 🏦 Bank Account Management

### Deposit Money

Players convert physical currency items (coins, notes) into bank balance:

```c
// On server
void DepositMoney(PlayerBase player, int amount)
{
    string playerID = player.GetIdentity().GetPlainId();
    BankAccountData account = BankAccountData.Load(playerID);

    // Check if player has enough physical currency
    if (!HasEnoughCurrency(player, amount))
    {
        NotificationSystem.Notify(player, "Not enough money!");
        return;
    }

    // Remove physical currency from inventory
    RemoveCurrencyFromPlayer(player, amount);

    // Add to bank balance
    account.Balance += amount;

    // Save account
    account.Save(playerID);

    // Notify player
    NotificationSystem.Notify(player, "Deposited: " + amount + " coins");
    GetRPCManager().SendRPC("MyMod_Banking", "GetBankAccount",
        new Param1<ref BankAccountData>(account), true, player.GetIdentity());
}
```

### Withdraw Money

Players convert bank balance into physical currency:

```c
// On server
void WithdrawMoney(PlayerBase player, int amount)
{
    string playerID = player.GetIdentity().GetPlainId();
    BankAccountData account = BankAccountData.Load(playerID);

    // Check if enough balance
    if (account.Balance < amount)
    {
        NotificationSystem.Notify(player, "Insufficient funds!");
        return;
    }

    // Deduct from bank balance
    account.Balance -= amount;

    // Spawn physical currency in inventory
    SpawnCurrencyInInventory(player, amount);

    // Save account
    account.Save(playerID);

    // Notify player
    NotificationSystem.Notify(player, "Withdrew: " + amount + " coins");
    GetRPCManager().SendRPC("MyMod_Banking", "GetBankAccount",
        new Param1<ref BankAccountData>(account), true, player.GetIdentity());
}
```

### Transfer Money

Players send money to other players:

```c
// On server
void TransferMoney(PlayerBase sender, string targetPlayerID, int amount)
{
    string senderID = sender.GetIdentity().GetPlainId();
    BankAccountData senderAccount = BankAccountData.Load(senderID);

    // Check if enough balance
    if (senderAccount.Balance < amount)
    {
        NotificationSystem.Notify(sender, "Insufficient funds!");
        return;
    }

    // Calculate transaction fee (optional, e.g., 5%)
    int fee = amount * 0.05;
    int totalDeduction = amount + fee;

    if (senderAccount.Balance < totalDeduction)
    {
        NotificationSystem.Notify(sender, "Insufficient funds (including fee)!");
        return;
    }

    // Deduct from sender
    senderAccount.Balance -= totalDeduction;
    senderAccount.Save(senderID);

    // Add to recipient
    BankAccountData targetAccount = BankAccountData.Load(targetPlayerID);
    targetAccount.Balance += amount;  // No fee for recipient
    targetAccount.Save(targetPlayerID);

    // Notify sender
    NotificationSystem.Notify(sender, "Transferred: " + amount + " coins (fee: " + fee + ")");

    // Notify recipient (if online)
    PlayerBase recipientPlayer = FindPlayerByID(targetPlayerID);
    if (recipientPlayer)
    {
        NotificationSystem.Notify(recipientPlayer, "Received: " + amount + " coins from " + sender.GetIdentity().GetName());
    }
}
```

---

## ⚠️ Common Issues & Troubleshooting

### Issue 1: ATM Action Not Showing

**Symptoms:** Player stands near ATM but "Open Bank" action doesn't appear.

**Solutions:**
- ✅ Check `TraderID = -2` is set in config.cpp
- ✅ Check building model is valid (use vanilla models like `Land_Deutshe`)
- ✅ Check action condition returns `true`
- ✅ Check player is within 10 meters of ATM
- ✅ Check action is registered in `CfgActions`

**Debug Code:**
```c
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
    BuildingBase building = BuildingBase.Cast(target.GetObject());

    if (building)
    {
        Print("[Debug] Building TraderID: " + building.TraderID);

        if (building.TraderID == -2)
        {
            Print("[Debug] This is an ATM!");
            return true;
        }
    }

    return false;
}
```

---

### Issue 2: Bank Account Not Saving

**Symptoms:** Player deposits money, but balance is 0 after reconnect.

**Solutions:**
- ✅ Check `$profile:Banking/` folder exists (create if not)
- ✅ Check file permissions (read/write access)
- ✅ Check `playerID` is consistent (use `GetIdentity().GetPlainId()`)
- ✅ Check `Save()` function is called after transaction
- ✅ Check JSON file is created (should be `SteamID64.json`)

**Debug Code:**
```c
void Save(string playerID)
{
    string filePath = "$profile:Banking/" + playerID + ".json";

    Print("[Debug] Saving bank account to: " + filePath);
    Print("[Debug] Balance: " + Balance);

    if (!FileExist("$profile:Banking/"))
    {
        Print("[Debug] Creating Banking folder");
        MakeDirectory("$profile:Banking/");
    }

    JsonFileLoader<BankAccountData>.JsonSaveFile(filePath, this);
    Print("[Debug] Saved!");
}
```

---

### Issue 3: Transactions Not Working

**Symptoms:** Player clicks Deposit/Withdraw, nothing happens.

**Solutions:**
- ✅ Check RPC is registered on both client and server
- ✅ Check server functions are marked with `[RpcEnum(Server, Client)]`
- ✅ Check client is sending RPC to server
- ✅ Check player identity is valid (`GetIdentity()` not null)
- ✅ Check transaction queue is not blocking

**Debug Code:**
```c
[RpcEnum(Server, Client)]
void DepositMoneyServer(CallType type, ParamsReadContext ctx, PlayerBase sender)
{
    Print("[Debug] DepositMoneyServer called");

    if (type == CallType.Server)
    {
        Print("[Debug] Executing on server");
        // Transaction logic...
    }
}
```

---

### Issue 4: Credit Card Not Working

**Symptoms:** Player holds credit card but ATM action doesn't show.

**Solutions:**
- ✅ Check credit card class is defined (`scope = 2`)
- ✅ Check `IsKindOf("CreditCard")` matches class name
- ✅ Check item is in hands (not in inventory)
- ✅ Check `RequireCreditCard` config is true

**Debug Code:**
```c
ItemBase itemInHands = player.GetHumanInventory().GetEntityInHands();

if (itemInHands)
{
    Print("[Debug] Item in hands: " + itemInHands.GetType());
    Print("[Debug] Is CreditCard: " + itemInHands.IsKindOf("CreditCard"));
}
else
{
    Print("[Debug] No item in hands!");
}
```

---

### Issue 5: Money Not Spawning on Withdraw

**Symptoms:** Withdraw succeeds, but no currency appears in inventory.

**Solutions:**
- ✅ Check currency item class is valid (e.g., `Coin_100`)
- ✅ Check player inventory has space
- ✅ Check `SpawnEntityInInventory()` is called
- ✅ Check currency item is defined in config.cpp

**Debug Code:**
```c
void SpawnCurrencyInInventory(PlayerBase player, int amount)
{
    Print("[Debug] Spawning currency: " + amount);

    // Example: Spawn 100-coin items
    int coinCount = amount / 100;
    int remainder = amount % 100;

    for (int i = 0; i < coinCount; i++)
    {
        ItemBase coin = ItemBase.Cast(player.GetHumanInventory().CreateInInventory("Coin_100"));

        if (coin)
        {
            Print("[Debug] Spawned Coin_100");
        }
        else
        {
            Print("[Debug] Failed to spawn Coin_100!");
        }
    }
}
```

---

## 🎯 Next Steps

### Beginner (You are here)
- ✅ Basic ATM setup
- ✅ Bank account management
- ✅ Deposit/Withdraw/Transfer

### Intermediate
- [ ] Add credit card requirement
- [ ] Add transaction fees
- [ ] Add bank UI
- [ ] Add transaction queue (prevent exploits)

### Advanced
- [ ] Add multiple currency support
- [ ] Add banking logs/transactions history
- [ ] Add ATM network (shared balance)
- [ ] Add loan system

**Recommended Reading:**
- [ATM System Reference](../reference/ATM_SYSTEM.md) - Complete technical guide
- [Economy System](../topics/27_trader_systems.md) - Currency items
- [RPC Networking](../topics/09_networking.md) - Client-server communication

---

## 📝 Quick Reference

### ATM Entity Checklist
- [ ] `TraderID = -2` in config.cpp
- [ ] Building model defined
- [ ] Action condition checks `TraderID == -2`
- [ ] Action registered in CfgActions

### Bank Account Checklist
- [ ] `BankAccountData` class created
- [ ] `Load()` function handles missing files
- [ ] `Save()` function creates directory if needed
- [ ] Uses `SteamID64` for unique identification

### Transaction Checklist
- [ ] Deposit removes physical currency
- [ ] Withdraw spawns physical currency
- [ ] Transfer validates both accounts
- [ ] All transactions save to JSON

### Security Checklist
- [ ] All transactions on server-side
- [ ] Input validation (prevent negative amounts)
- [ ] Maximum balance enforced
- [ ] Rate limiting (prevent spam)

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete for Beginners
**Next:** See [ATM System Reference](../reference/ATM_SYSTEM.md) for advanced features
