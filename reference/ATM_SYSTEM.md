# ATM System - Complete Reference

**Status:** ✅ Complete Reference
**Target:** Advanced modders
**Goal:** Comprehensive ATM/banking system with physical ATMs, credit cards, transaction queue, and fees

---

## ⚡ 30-Second Summary

Complete ATM and banking system with physical ATM objects, credit card authentication, transaction queue for concurrency, transaction fees, bank account management, and full UI integration.

---

## 📋 Table of Contents

1. [System Architecture](#system-architecture)
2. [ATM Entity](#atm-entity)
3. [Credit Card System](#credit-card-system)
4. [Bank Account Management](#bank-account-management)
5. [Transaction Queue](#transaction-queue)
6. [Transaction Types](#transaction-types)
7. [Transaction Fees](#transaction-fees)
8. [RPC Networking](#rpc-networking)
9. [Bank UI](#bank-ui)
10. [Complete Implementation](#complete-implementation)
11. [Security Best Practices](#security-best-practices)
12. [Troubleshooting](#troubleshooting)

---

## System Architecture

### System Components

```
ATM System
├── ATMEntity               ← Physical ATM object
├── CreditCardItem          ← Credit card item
├── BankAccountManager     ← Account management
├── TransactionQueue        ← Concurrency control
├── TransactionProcessor    ← Execute transactions
└── BankUI                  ← User interface
```

### Data Flow

```
Player approaches ATM
    ↓
Checks for credit card (if required)
    ↓
Opens bank UI
    ↓
Player selects transaction (deposit/withdraw/transfer)
    ↓
Transaction added to queue
    ↓
Transaction processed (FIFO order)
    ↓
Bank account updated
    ↓
UI refreshed with new balance
```

---

## ATM Entity

### ATM Object Definition

**config.cpp:**

```cpp
class CfgPatches
{
    class ATMSystem
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] =
        {
            "DZ_Data",
            "DZ_Scripts"
        };
    };
};

class CfgVehicles
{
    class House;

    // ATM Entity
    class Land_Atm: House
    {
        scope = 2;
        displayName = "ATM";
        descriptionShort = "Automated Teller Machine for banking";
        model = "DZ\structures\commercial\atm_01.p3d";

        // TraderID for ATM identification
        TraderID = -2;

        // Rotation and placement
        rotationFlags = 16;
        weight = 1000;
    };
};

class CfgPatches
{
    class ATMSystem
    {
        units[] =
        {
            "Land_Atm"
        };
    };
};
```

### ATM Entity Class

```c
class ATMEntity extends House
{
    protected int m_TraderID = -2; // Special ID for ATMs
    protected float m_InteractRadius = 3.0;

    void ATMEntity()
    {
        SetTraderID(m_TraderID);
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionTradePlusBankingMenu);
    }

    override bool IsInventoryVisible()
    {
        return false; // No inventory access
    }

    int GetTraderID()
    {
        return m_TraderID;
    }
}
```

---

## Credit Card System

### Credit Card Item

**config.cpp:**

```cpp
class CfgVehicles
{
    class ItemBase;

    class CreditCardItem: ItemBase
    {
        scope = 2;
        displayName = "Credit Card";
        descriptionShort = "Bank card for ATM transactions";
        weight = 10;

        // Credit card properties
        variant = 0;
        consumable = 0;
    };
}
```

### Credit Card Validation

```c
class CreditCardValidator
{
    static bool HasCreditCard(PlayerBase player)
    {
        if (!GetATMConfig().IsCreditCarNeededForTransaction)
            return true; // No card required

        ItemBase item = player.GetItemInHands();

        if (!item)
        {
            NotificationSystem.Notify(player, "Credit card required!");
            return false;
        }

        if (!item.IsKindOf("CreditCardItem"))
        {
            NotificationSystem.Notify(player, "Must hold credit card!");
            return false;
        }

        return true;
    }
}
```

---

## Bank Account Management

### Bank Account Data Structure

```c
class BankAccountData
{
    string Version = "";
    string SteamID64 = "";      // Player's Steam64 ID
    string Name = "";            // Player name
    int MoneyAmount = 0;         // Current balance
    int MaxAmount = 1000000;     // Maximum balance
    ref TStringArray Licences;   // Licenses (optional)
    ref TIntStringMap Insurances; // Insurances (optional)

    void BankAccountData()
    {
        Licences = new TStringArray;
        Insurances = new TIntStringMap;
    }

    void DefaultAccount(string id, int defaultMaxCurrency, int defaultStartCurrency, string name)
    {
        Version = "";
        SteamID64 = id;
        Name = name;
        MoneyAmount = defaultStartCurrency;
        MaxAmount = defaultMaxCurrency;

        Save(SteamID64);
    }

    void Save(string id)
    {
        string filePath = string.Format("$profile:Banking/%1.json", id);
        JsonFileLoader<BankAccountData>.JsonSaveFile(filePath, this);
    }

    static BankAccountData Load(string id, int defaultMaxCurrency, int defaultStartCurrency, string name = "")
    {
        BankAccountData data = new BankAccountData();

        // Create directory if needed
        string dir = "$profile:Banking";
        if (!FileExist(dir))
            MakeDirectory(dir);

        string filePath = string.Format("$profile:Banking/%1.json", id);

        if (FileExist(filePath))
        {
            JsonFileLoader<BankAccountData>.JsonLoadFile(filePath, data);
            data.CheckVersion(id);
        }
        else
        {
            data.DefaultAccount(id, defaultMaxCurrency, defaultStartCurrency, name);
        }

        return data;
    }

    void CheckVersion(string id)
    {
        if (Version != TRADERPLUS_CURRENT_VERSION && Version != "-1")
        {
            Version = TRADERPLUS_CURRENT_VERSION;
            if (id != SteamID64)
                SteamID64 = id;
            Save(SteamID64);
        }
    }
}
```

### Bank Manager

```c
class BankManager
{
    protected ref map<string, ref BankAccountData> m_Accounts;
    protected static ref BankManager m_Instance;

    static BankManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new BankManager();
        }
        return m_Instance;
    }

    void BankManager()
    {
        m_Accounts = new map<string, ref BankAccountData>;
    }

    BankAccountData GetAccount(string playerID)
    {
        if (!m_Accounts.Contains(playerID))
        {
            BankAccountData account = BankAccountData.Load(
                playerID,
                GetATMConfig().DefaultMaxCurrency,
                GetATMConfig().DefaultStartCurrency
            );
            m_Accounts.Set(playerID, account);
        }

        return m_Accounts.Get(playerID);
    }

    int GetBalance(string playerID)
    {
        BankAccountData account = GetAccount(playerID);
        return account.MoneyAmount;
    }

    void SetBalance(string playerID, int amount)
    {
        BankAccountData account = GetAccount(playerID);

        // Enforce max limit
        if (amount > account.MaxAmount)
            amount = account.MaxAmount;

        account.MoneyAmount = amount;
        account.Save(playerID);
    }

    void Deposit(string playerID, int amount)
    {
        int current = GetBalance(playerID);
        SetBalance(playerID, current + amount);
    }

    void Withdraw(string playerID, int amount)
    {
        int current = GetBalance(playerID);
        SetBalance(playerID, current - amount);
    }

    bool HasBalance(string playerID, int amount)
    {
        return GetBalance(playerID) >= amount;
    }
}
```

---

## Transaction Queue

### Queue System

```c
class BankingTransactionQueue
{
    private ref array<ref BankingTransaction> m_Queue;

    void BankingTransactionQueue()
    {
        m_Queue = new array<ref BankingTransaction>;
    }

    int Count()
    {
        return m_Queue.Count();
    }

    void EnQueue(BankingTransaction transaction)
    {
        m_Queue.Insert(transaction);
    }

    void DeQueue()
    {
        if (m_Queue.Count() == 0)
            return;

        // Shift all elements
        for (int i = 1; i < m_Queue.Count(); i++)
        {
            m_Queue.Set(i - 1, m_Queue[i]);
        }

        m_Queue.RemoveOrdered(m_Queue.Count() - 1);
    }

    BankingTransaction Peek()
    {
        if (m_Queue.Count() == 0)
            return null;

        return m_Queue[0];
    }

    bool HasNextQueue()
    {
        return m_Queue.IsValidIndex(0);
    }
}
```

### Transaction Processing

```c
class TransactionProcessor
{
    protected ref BankingTransactionQueue m_Queue;
    protected float m_ProcessInterval = 0.1; // Process every 100ms
    protected float m_LastProcess;

    void TransactionProcessor()
    {
        m_Queue = new BankingTransactionQueue();
    }

    void Update(float deltaTime)
    {
        m_LastProcess += deltaTime;

        if (m_LastProcess >= m_ProcessInterval)
        {
            m_LastProcess = 0;

            if (m_Queue.HasNextQueue())
            {
                BankingTransaction transaction = m_Queue.Peek();

                // Process transaction
                ProcessTransaction(transaction);

                // Remove from queue
                m_Queue.DeQueue();
            }
        }
    }

    void ProcessTransaction(BankingTransaction transaction)
    {
        switch (transaction.TransactionMode)
        {
            case TP_BANK_TRANSACTION_DEPOSIT:
                ProcessDeposit(transaction);
                break;

            case TP_BANK_TRANSACTION_WITHDRAW:
                ProcessWithdraw(transaction);
                break;

            case TP_BANK_TRANSACTION_TRANSFERT:
                ProcessTransfer(transaction);
                break;
        }
    }
}
```

---

## Transaction Types

### Deposit Transaction

```c
void ProcessDeposit(BankingTransaction transaction)
{
    PlayerBase player = transaction.Customer;
    BankAccountData account = player.GetBankAccount();

    if (!account)
        return;

    // Check if player has enough money
    if (!CheckPlayerHasEnoughMoney(player, transaction.Amount))
    {
        NotificationSystem.Notify(player, "Insufficient funds!");
        return;
    }

    // Calculate amount minus fees
    int fees = transaction.Amount * GetATMConfig().TransactionFees;
    int amountMinusFees = transaction.Amount - fees;

    // Add to bank account
    account.MoneyAmount += amountMinusFees;

    // Remove from player inventory
    RemoveMoneyFromPlayer(player, transaction.Amount);

    // Update account
    account.UpdateAccount(player);

    // Notify player
    NotificationSystem.Notify(player, "Deposited: " + amountMinusFees + " (Fee: " + fees + ")");
}
```

### Withdraw Transaction

```c
void ProcessWithdraw(BankingTransaction transaction)
{
    PlayerBase player = transaction.Customer;
    BankAccountData account = player.GetBankAccount();

    if (!account)
        return;

    // Check if account has enough money
    if (account.MoneyAmount < transaction.Amount)
    {
        NotificationSystem.Notify(player, "Insufficient bank balance!");
        return;
    }

    // Deduct from account
    account.MoneyAmount -= transaction.Amount;

    // Add money to player inventory
    AddMoneyToPlayer(player, transaction.Amount);

    // Update account
    account.UpdateAccount(player);

    // Notify player
    NotificationSystem.Notify(player, "Withdrew: " + transaction.Amount);
}
```

### Transfer Transaction

```c
void ProcessTransfer(BankingTransaction transaction)
{
    PlayerBase sender = transaction.Customer;
    BankAccountData senderAccount = sender.GetBankAccount();

    if (!senderAccount)
        return;

    // Check if sender has enough
    if (senderAccount.MoneyAmount < transaction.Amount)
    {
        NotificationSystem.Notify(sender, "Insufficient funds for transfer!");
        return;
    }

    // Check if destination account exists
    string destFilePath = string.Format("$profile:Banking/%1.json", transaction.DestinationIDAccount);

    if (!FileExist(destFilePath))
    {
        NotificationSystem.Notify(sender, "Destination account not found!");
        return;
    }

    // Calculate fees
    int fees = transaction.Amount * GetATMConfig().TransactionFees;
    int totalDeduct = transaction.Amount + fees;

    // Deduct from sender
    senderAccount.MoneyAmount -= totalDeduct;
    senderAccount.UpdateAccount(sender);

    // Load destination account
    BankAccountData destAccount = BankAccountData.Load(
        transaction.DestinationIDAccount,
        GetATMConfig().DefaultMaxCurrency,
        0 // Don't create new account
    );

    if (destAccount)
    {
        // Add to destination (enforce max)
        if (destAccount.MoneyAmount + transaction.Amount > destAccount.MaxAmount)
            destAccount.MoneyAmount = destAccount.MaxAmount;
        else
            destAccount.MoneyAmount += transaction.Amount;

        destAccount.Save(transaction.DestinationIDAccount);

        // Notify sender
        NotificationSystem.Notify(sender, "Transferred " + transaction.Amount + " to " + transaction.DestinationIDAccount);
    }
}
```

---

## Transaction Fees

### Fee Configuration

```c
class ATMConfig
{
    float TransactionFees = 0.05; // 5%

    int CalculateDepositFee(int amount)
    {
        return Math.Round(amount * TransactionFees);
    }

    int CalculateWithdrawFee(int amount)
    {
        return Math.Round(amount * TransactionFees);
    }

    int CalculateTransferFee(int amount)
    {
        return Math.Round(amount * TransactionFees);
    }
}
```

### Fee Examples

| Transaction | Amount | Fee (5%) | Total |
|-------------|--------|----------|-------|
| Deposit | 1000 | 50 | 1050 deducted, 1000 deposited |
| Withdraw | 1000 | 0 | 1000 withdrawn |
| Transfer | 1000 | 50 | 1050 deducted from sender |

---

## RPC Networking

### RPC Setup

```c
class ATMRPCSystem
{
    void InitRPC()
    {
        if (GetGame().IsServer())
        {
            // Server-side RPCs
            GetRPCManager().AddRPC("ATMSystem", "ATMRequest", this, SingeplayerExecutionType.Client);
            GetRPCManager().AddRPC("ATMSystem", "ATMWithdraw", this, SingeplayerExecutionType.Client);
            GetRPCManager().AddRPC("ATMSystem", "ATMDeposit", this, SingeplayerExecutionType.Client);
            GetRPCManager().AddRPC("ATMSystem", "ATMTransfer", this, SingeplayerExecutionType.Client);
        }
        else
        {
            // Client-side RPCs
            GetRPCManager().AddRPC("ATMSystem", "GetATMConfig", this, SingeplayerExecutionType.Server);
            GetRPCManager().AddRPC("ATMSystem", "GetBankAccount", this, SingeplayerExecutionType.Server);
        }
    }
}
```

### Server RPC Handlers

```c
void ATMRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (!GetGame().IsServer())
        return;

    PlayerBase player = PlayerBase.Cast(GetGame().PlayerByID(sender.GetUID()));

    if (!player)
        return;

    BankAccountData account = player.GetBankAccount();

    if (account)
    {
        // Enforce max limit
        if (account.MoneyAmount > account.MaxAmount)
            account.MoneyAmount = account.MaxAmount;

        // Send account info to client
        GetRPCManager().SendRPC("ATMSystem", "GetBankAccount",
            new Param1<ref BankAccountData>(account),
            true,
            sender);
    }
}

void ATMWithdraw(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (!GetGame().IsServer())
        return;

    Param1<int> data;
    if (!ctx.Read(data))
        return;

    int amount = data.param1;

    PlayerBase player = PlayerBase.Cast(GetGame().PlayerByID(sender.GetUID()));

    if (!player)
        return;

    // Create transaction
    BankingTransaction transaction = new BankingTransaction();
    transaction.Customer = player;
    transaction.Amount = amount;
    transaction.TransactionMode = TP_BANK_TRANSACTION_WITHDRAW;

    // Add to queue
    TransactionProcessor.GetInstance().EnQueue(transaction);
}
```

---

## Bank UI

### Bank Menu Class

```c
class BankMenu extends UIScriptedMenu
{
    private ButtonWidget m_WithdrawButton;
    private ButtonWidget m_DepositButton;
    private ButtonWidget m_TransferButton;
    private TextWidget m_BalanceText;
    private EditBoxWidget m_AmountInput;
    private EditBoxWidget m_TransferIDInput;
    private ProgressBarWidget m_BalanceProgressBar;

    private int m_Balance;
    private int m_MaxBalance;

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("DayZ_Gui/Layouts/BankMenu.layout");

        m_WithdrawButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("WithdrawButton"));
        m_DepositButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("DepositButton"));
        m_TransferButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("TransferButton"));
        m_BalanceText = TextWidget.Cast(layoutRoot.FindAnyWidget("BalanceText"));
        m_AmountInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("AmountInput"));
        m_TransferIDInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("TransferIDInput"));
        m_BalanceProgressBar = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("BalanceProgressBar"));

        return layoutRoot;
    }

    override void OnShow()
    {
        super.OnShow();

        // Request account info
        GetRPCManager().SendRPC("ATMSystem", "ATMRequest", NULL, true, NULL);

        // Update UI
        UpdateBalanceDisplay();
    }

    void UpdateBalanceDisplay()
    {
        if (!m_BalanceText)
            return;

        m_BalanceText.SetText("Balance: " + m_Balance.ToString());

        if (m_BalanceProgressBar)
        {
            m_BalanceProgressBar.SetCurrent(m_Balance);
            m_BalanceProgressBar.SetMax(m_MaxBalance);
        }
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_WithdrawButton)
        {
            OnWithdrawClick();
            return true;
        }

        if (w == m_DepositButton)
        {
            OnDepositClick();
            return true;
        }

        if (w == m_TransferButton)
        {
            OnTransferClick();
            return true;
        }

        return super.OnClick(w, x, y, button);
    }

    void OnWithdrawClick()
    {
        string amountStr = m_AmountInput.GetText();
        int amount = amountStr.ToInt();

        if (amount <= 0)
        {
            NotificationSystem.Notify("Invalid amount!");
            return;
        }

        // Send withdraw request
        GetRPCManager().SendRPC("ATMSystem", "ATMWithdraw",
            new Param1<int>(amount),
            true,
            NULL);
    }

    void OnDepositClick()
    {
        string amountStr = m_AmountInput.GetText();
        int amount = amountStr.ToInt();

        if (amount <= 0)
        {
            NotificationSystem.Notify("Invalid amount!");
            return;
        }

        // Send deposit request
        GetRPCManager().SendRPC("ATMSystem", "ATMDeposit",
            new Param1<int>(amount),
            true,
            NULL);
    }

    void OnTransferClick()
    {
        string amountStr = m_AmountInput.GetText();
        string destID = m_TransferIDInput.GetText();

        int amount = amountStr.ToInt();

        if (amount <= 0)
        {
            NotificationSystem.Notify("Invalid amount!");
            return;
        }

        if (destID == "")
        {
            NotificationSystem.Notify("Enter destination account ID!");
            return;
        }

        // Send transfer request
        GetRPCManager().SendRPC("ATMSystem", "ATMTransfer",
            new Param2<int, string>(amount, destID),
            true,
            NULL);
    }
}
```

---

## Complete Implementation

### File Structure

```
atm_system/
├── config.cpp                    ← ATM & Credit Card definitions
├── banking_config.json           ← ATM settings
└── scripts/
    ├── 1_Core/
    │   ├── ATMConfig.c           ← Configuration
    │   └── ATMConstants.c        ← Constants
    ├── 3_Game/
    │   ├── BankManager.c         ← Account management
    │   ├── BankAccountData.c     ← Account data
    │   ├── TransactionProcessor.c ← Process transactions
    │   └── BankingTransactionQueue.c ← Queue system
    ├── 4_World/
    │   └── Classes/
    │       ├── ATMEntity.c        ← Physical ATM
    │       ├── CreditCard.c       ← Credit card item
    │       └── Actions/
    │           └── ActionTradePlusBankingMenu.c
    └── 5_Mission/
        └── BankMenu.c            ← UI
```

---

## Security Best Practices

### Input Validation

```c
bool ValidateTransaction(PlayerBase player, int amount)
{
    // Validate amount
    if (amount <= 0)
        return false;

    // Validate player
    if (!player || !player.GetIdentity())
        return false;

    // Validate credit card (if required)
    if (!CreditCardValidator.HasCreditCard(player))
        return false;

    return true;
}
```

### Server-Side Authority

```c
void ProcessWithdraw(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    // Always validate on server
    if (!GetGame().IsServer())
        return;

    // Server-side validation
    PlayerBase player = GetGame().PlayerByID(sender.GetUID());

    if (!player)
        return;

    // Server-side balance check
    BankAccountData account = BankManager.GetInstance().GetAccount(sender.GetPlainId());

    if (!account || account.MoneyAmount < amount)
        return;

    // Execute transaction on server
    ExecuteWithdraw(player, amount);
}
```

---

## Troubleshooting

### ATM not interactable

**Check:**
1. ATM object spawned correctly
2. TraderID set to -2
3. Player within interaction radius
4. ActionTradePlusBankingMenu registered

### Transactions not processing

**Check:**
1. TransactionQueue updating
2. Processor.Update() being called
3. RPC registered correctly
4. Server-side validation passing

### Bank account not saving

**Check:**
1. $profile:Banking/ directory exists
2. Write permissions
3. Steam64 ID valid
4. JSON serialization working

---

*Last updated: 2025-01-18*
