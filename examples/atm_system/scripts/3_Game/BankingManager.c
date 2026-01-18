// ============================================================
// BANKING MANAGER - Core banking system
// ============================================================
// This class handles all banking operations:
// - Deposit money
// - Withdraw money
// - Transfer money
// - Transaction queue management
// - Player account management

class BankingManager
{
    // Singleton instance
    private static ref BankingManager m_Instance;

    // Transaction queue (FIFO)
    private ref array<ref BankingTransaction> m_TransactionQueue;

    // Active player accounts (cached)
    private ref map<string, ref BankingAccount> m_AccountCache;

    // ============================================================
    // GET INSTANCE - Singleton pattern
    // ============================================================
    static BankingManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new BankingManager();
        }

        return m_Instance;
    }

    // ============================================================
    // CONSTRUCTOR - Initialize banking system
    // ============================================================
    void BankingManager()
    {
        m_TransactionQueue = new array<ref BankingTransaction>;
        m_AccountCache = new map<string, ref BankingAccount>;

        // Start transaction queue processor
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ProcessTransactionQueue, 1000, true);

        Print("[ATM System] BankingManager initialized.");
    }

    // ============================================================
    // GET PLAYER ACCOUNT - Load from cache or file
    // ============================================================
    BankingAccount GetPlayerAccount(string playerID, string playerName = "")
    {
        // Check cache first
        if (m_AccountCache.Contains(playerID))
        {
            return m_AccountCache.Get(playerID);
        }

        // Load from file
        BankingAccount account = BankingAccount.Load(playerID, playerName);

        // Add to cache
        m_AccountCache.Set(playerID, account);

        return account;
    }

    // ============================================================
    // SAVE PLAYER ACCOUNT - Save to file and update cache
    // ============================================================
    void SavePlayerAccount(string playerID)
    {
        if (m_AccountCache.Contains(playerID))
        {
            BankingAccount account = m_AccountCache.Get(playerID);
            account.Save();
        }
    }

    // ============================================================
    // DEPOSIT MONEY - Convert physical currency to bank balance
    // ============================================================
    bool DepositMoney(PlayerBase player, int amount)
    {
        if (!player)
        {
            Print("[ATM System] Error: Player is null");
            return false;
        }

        // Validate transaction amount
        if (!GetBankingConfig().IsValidTransactionAmount(amount))
        {
            NotificationSystem.Notify(player, "Invalid transaction amount!");
            return false;
        }

        // Get player account
        string playerID = player.GetIdentity().GetPlainId();
        BankingAccount account = GetPlayerAccount(playerID, player.GetIdentity().GetName());

        // Check if player has enough physical currency
        if (!HasEnoughCurrency(player, amount))
        {
            NotificationSystem.Notify(player, "You don't have enough currency in your inventory!");
            return false;
        }

        // Check if deposit would exceed max balance
        if (!account.CanDeposit(amount))
        {
            NotificationSystem.Notify(player, "Deposit would exceed maximum balance!");
            return false;
        }

        // Remove physical currency from inventory
        RemoveCurrencyFromPlayer(player, amount);

        // Add to bank balance
        account.Deposit(amount);

        // Create transaction record
        BankingTransaction transaction = new BankingTransaction(playerID, player.GetIdentity().GetName(), amount, "deposit");
        account.AddTransactionToHistory(transaction);

        // Save account
        SavePlayerAccount(playerID);

        // Send updated account to client
        SendAccountToClient(player, account);

        // Show notification
        if (GetBankingConfig().ShowBalanceOnTransaction)
        {
            NotificationSystem.Notify(player, "Deposited: " + amount.ToString() + ". New Balance: " + account.Balance.ToString());
        }
        else
        {
            NotificationSystem.Notify(player, "Deposited: " + amount.ToString());
        }

        return true;
    }

    // ============================================================
    // WITHDRAW MONEY - Convert bank balance to physical currency
    // ============================================================
    bool WithdrawMoney(PlayerBase player, int amount)
    {
        if (!player)
        {
            Print("[ATM System] Error: Player is null");
            return false;
        }

        // Validate transaction amount
        if (!GetBankingConfig().IsValidTransactionAmount(amount))
        {
            NotificationSystem.Notify(player, "Invalid transaction amount!");
            return false;
        }

        // Get player account
        string playerID = player.GetIdentity().GetPlainId();
        BankingAccount account = GetPlayerAccount(playerID, player.GetIdentity().GetName());

        // Check if player has enough balance
        if (!account.CanWithdraw(amount))
        {
            NotificationSystem.Notify(player, "Insufficient funds!");

            // Show remaining daily withdrawal limit
            int remaining = account.GetRemainingDailyWithdrawal();
            if (remaining >= 0)
            {
                NotificationSystem.Notify(player, "Daily withdrawal limit remaining: " + remaining.ToString());
            }

            return false;
        }

        // Deduct from bank balance
        account.Withdraw(amount);

        // Spawn physical currency in inventory
        SpawnCurrencyInInventory(player, amount);

        // Create transaction record
        BankingTransaction transaction = new BankingTransaction(playerID, player.GetIdentity().GetName(), amount, "withdraw");
        account.AddTransactionToHistory(transaction);

        // Save account
        SavePlayerAccount(playerID);

        // Send updated account to client
        SendAccountToClient(player, account);

        // Show notification
        if (GetBankingConfig().ShowBalanceOnTransaction)
        {
            NotificationSystem.Notify(player, "Withdrew: " + amount.ToString() + ". New Balance: " + account.Balance.ToString());
        }
        else
        {
            NotificationSystem.Notify(player, "Withdrew: " + amount.ToString());
        }

        return true;
    }

    // ============================================================
    // TRANSFER MONEY - Send money to another player
    // ============================================================
    bool TransferMoney(PlayerBase sender, string targetID, int amount)
    {
        if (!sender)
        {
            Print("[ATM System] Error: Sender is null");
            return false;
        }

        // Validate transaction amount
        if (!GetBankingConfig().IsValidTransactionAmount(amount))
        {
            NotificationSystem.Notify(sender, "Invalid transaction amount!");
            return false;
        }

        // Get sender account
        string senderID = sender.GetIdentity().GetPlainId();
        BankingAccount senderAccount = GetPlayerAccount(senderID, sender.GetIdentity().GetName());

        // Calculate fee
        int fee = GetBankingConfig().CalculateFee(amount);

        // Check if sender has enough balance (including fee)
        if (!senderAccount.CanTransfer(amount, fee))
        {
            NotificationSystem.Notify(sender, "Insufficient funds (including transaction fee)!");
            return false;
        }

        // Deduct from sender
        senderAccount.Transfer(amount, fee);

        // Create transaction record
        BankingTransaction transaction = new BankingTransaction(senderID, sender.GetIdentity().GetName(), amount, "transfer");
        transaction.SetTarget(targetID, "Unknown");  // Will update when target found
        transaction.SetFee(fee);
        senderAccount.AddTransactionToHistory(transaction);

        // Save sender account
        SavePlayerAccount(senderID);

        // Get or create target account
        BankingAccount targetAccount = GetPlayerAccount(targetID);
        targetAccount.Deposit(amount);
        SavePlayerAccount(targetID);

        // Update transaction with target name
        transaction.SetTarget(targetID, targetAccount.PlayerName);

        // Send updated account to sender
        SendAccountToClient(sender, senderAccount);

        // Notify sender
        if (GetBankingConfig().ShowTransactionFee && fee > 0)
        {
            NotificationSystem.Notify(sender, "Transferred: " + amount.ToString() + " (Fee: " + fee.ToString() + ")");
        }
        else
        {
            NotificationSystem.Notify(sender, "Transferred: " + amount.ToString());
        }

        if (GetBankingConfig().ShowBalanceOnTransaction)
        {
            NotificationSystem.Notify(sender, "New Balance: " + senderAccount.Balance.ToString());
        }

        // Notify target (if online)
        PlayerBase targetPlayer = FindPlayerByID(targetID);
        if (targetPlayer && GetBankingConfig().NotifyOnReceive)
        {
            NotificationSystem.Notify(targetPlayer, "Received: " + amount.ToString() + " from " + sender.GetIdentity().GetName());
            SendAccountToClient(targetPlayer, targetAccount);
        }

        return true;
    }

    // ============================================================
    // GET BALANCE - Return player's current balance
    // ============================================================
    int GetBalance(string playerID)
    {
        BankingAccount account = GetPlayerAccount(playerID);
        return account.Balance;
    }

    // ============================================================
    // CHECK IF PLAYER HAS ENOUGH CURRENCY IN INVENTORY
    // ============================================================
    private bool HasEnoughCurrency(PlayerBase player, int amount)
    {
        int totalValue = 0;

        // Check all items in inventory
        array<ItemBase> items = new array<ItemBase>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

        foreach (ItemBase item : items)
        {
            // Check if item is accepted currency
            if (GetBankingConfig().IsCurrencyAccepted(item.GetType()))
            {
                // Get item value (assuming item has a "value" property)
                // This is a simplified version - in production, you'd check for specific properties
                int value = GetCurrencyValue(item);
                totalValue += value;

                if (totalValue >= amount)
                {
                    return true;  // Early exit
                }
            }
        }

        return totalValue >= amount;
    }

    // ============================================================
    // GET CURRENCY VALUE - Return value of currency item
    // ============================================================
    private int GetCurrencyValue(ItemBase item)
    {
        // This is a simplified version
        // In production, you'd check for specific properties or use a mapping

        string className = item.GetType();

        // Example: Coin_10 = 10, Coin_50 = 50, Coin_100 = 100
        if (className.Contains("Coin_"))
        {
            string valueStr = className.Substring(5, className.Length() - 5);
            return valueStr.ToInt();
        }

        // Example: Money_Ruble = 1000, Money_Dollar = 100
        if (className == "Money_Ruble")
        {
            return 1000;
        }
        else if (className == "Money_Dollar")
        {
            return 100;
        }

        return 0;  // Unknown currency
    }

    // ============================================================
    // REMOVE CURRENCY FROM PLAYER INVENTORY
    // ============================================================
    private void RemoveCurrencyFromPlayer(PlayerBase player, int amount)
    {
        int remaining = amount;

        // Remove currency items until amount is reached
        while (remaining > 0)
        {
            array<ItemBase> items = new array<ItemBase>;
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

            bool foundCurrency = false;

            foreach (ItemBase item : items)
            {
                if (GetBankingConfig().IsCurrencyAccepted(item.GetType()))
                {
                    int value = GetCurrencyValue(item);

                    if (value <= remaining)
                    {
                        // Remove entire item
                        player.GetInventory().RemoveItem(item);
                        GetGame().ObjectDelete(item);
                        remaining -= value;
                        foundCurrency = true;
                        break;
                    }
                    else
                    {
                        // This is more complex - you'd need to split the item
                        // For simplicity, we just remove it
                        player.GetInventory().RemoveItem(item);
                        GetGame().ObjectDelete(item);
                        remaining -= value;
                        foundCurrency = true;
                        break;
                    }
                }
            }

            if (!foundCurrency)
            {
                break;  // No more currency to remove
            }
        }
    }

    // ============================================================
    // SPAWN CURRENCY IN PLAYER INVENTORY
    // ============================================================
    private void SpawnCurrencyInInventory(PlayerBase player, int amount)
    {
        int remaining = amount;

        // Spawn 100-coin items (largest denomination)
        while (remaining >= 100)
        {
            ItemBase coin = ItemBase.Cast(player.GetHumanInventory().CreateInInventory("Coin_100"));
            if (!coin)
            {
                // Inventory full, drop on ground
                vector pos = player.GetPosition();
                coin = ItemBase.Cast(GetGame().CreateObject("Coin_100", pos));
            }
            remaining -= 100;
        }

        // Spawn 50-coin items
        while (remaining >= 50)
        {
            ItemBase coin = ItemBase.Cast(player.GetHumanInventory().CreateInInventory("Coin_50"));
            if (!coin)
            {
                vector pos = player.GetPosition();
                coin = ItemBase.Cast(GetGame().CreateObject("Coin_50", pos));
            }
            remaining -= 50;
        }

        // Spawn 10-coin items
        while (remaining >= 10)
        {
            ItemBase coin = ItemBase.Cast(player.GetHumanInventory().CreateInInventory("Coin_10"));
            if (!coin)
            {
                vector pos = player.GetPosition();
                coin = ItemBase.Cast(GetGame().CreateObject("Coin_10", pos));
            }
            remaining -= 10;
        }

        // If there's a remainder, spawn 10-coin items for it
        if (remaining > 0)
        {
            ItemBase coin = ItemBase.Cast(player.GetHumanInventory().CreateInInventory("Coin_10"));
            if (!coin)
            {
                vector pos = player.GetPosition();
                coin = ItemBase.Cast(GetGame().CreateObject("Coin_10", pos));
            }
        }
    }

    // ============================================================
    // FIND PLAYER BY ID - Find online player by Steam64 ID
    // ============================================================
    private PlayerBase FindPlayerByID(string playerID)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player && player.GetIdentity().GetPlainId() == playerID)
            {
                return player;
            }
        }

        return null;  // Player not online
    }

    // ============================================================
    // SEND ACCOUNT TO CLIENT - Update client with latest account data
    // ============================================================
    private void SendAccountToClient(PlayerBase player, BankingAccount account)
    {
        GetRPCManager().SendRPC("ATM_System", "ReceiveAccountData",
            new Param1<ref BankingAccount>(account), true, player.GetIdentity());
    }

    // ============================================================
    // PROCESS TRANSACTION QUEUE - Process pending transactions
    // ============================================================
    private void ProcessTransactionQueue()
    {
        if (m_TransactionQueue.Count() == 0)
        {
            return;  // No transactions to process
        }

        // Process first transaction in queue (FIFO)
        BankingTransaction transaction = m_TransactionQueue[0];

        // Process transaction (implementation depends on your needs)
        // For now, just mark as completed
        transaction.MarkCompleted();

        // Remove from queue
        m_TransactionQueue.RemoveOrdered(0);

        if (GetBankingConfig().BankingLogs)
        {
            Print("[ATM System] Processed transaction: " + transaction.GetSummary());
        }
    }

    // ============================================================
    // ADD TRANSACTION TO QUEUE
    // ============================================================
    void AddTransactionToQueue(BankingTransaction transaction)
    {
        if (m_TransactionQueue.Count() >= GetBankingConfig().TransactionQueueSize)
        {
            Print("[ATM System] Warning: Transaction queue is full!");
            return;
        }

        m_TransactionQueue.Insert(transaction);

        if (GetBankingConfig().BankingLogs)
        {
            Print("[ATM System] Transaction added to queue: " + transaction.TransactionID);
        }
    }

    // ============================================================
    // CLEAR ACCOUNT CACHE - Free memory
    // ============================================================
    void ClearAccountCache()
    {
        m_AccountCache.Clear();
        Print("[ATM System] Account cache cleared.");
    }

    // ============================================================
    // SAVE ALL ACCOUNTS - Save all cached accounts
    // ============================================================
    void SaveAllAccounts()
    {
        foreach (string playerID, BankingAccount account : m_AccountCache)
        {
            account.Save();
        }

        Print("[ATM System] All accounts saved.");
    }
}
