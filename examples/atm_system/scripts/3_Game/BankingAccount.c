// ============================================================
// BANKING ACCOUNT DATA STRUCTURE
// ============================================================
// This class represents a player's bank account
// Persists to JSON file in $profile:Banking/

class BankingAccount
{
    string SteamID64;           // Player's unique Steam64 ID
    string PlayerName;          // Player's name (for display)
    int Balance;                // Current bank balance
    int MaxBalance;             // Maximum allowed balance
    int DailyWithdrawn;         // Amount withdrawn today (for daily limit)
    int LastWithdrawReset;      // Last time daily withdrawal was reset (Unix timestamp)
    ref array<ref BankingTransaction> TransactionHistory;  // Transaction log

    // ============================================================
    // CONSTRUCTOR - Initialize new account
    // ============================================================
    void BankingAccount()
    {
        Balance = 0;
        MaxBalance = GetBankingConfig().DefaultMaxBalance;
        DailyWithdrawn = 0;
        LastWithdrawReset = 0;
        TransactionHistory = new array<ref BankingTransaction>;
    }

    // ============================================================
    // CAN DEPOSIT - Check if deposit is allowed
    // ============================================================
    bool CanDeposit(int amount)
    {
        if (amount <= 0)
        {
            return false;  // Invalid amount
        }

        if (Balance + amount > MaxBalance)
        {
            return false;  // Would exceed max balance
        }

        return true;
    }

    // ============================================================
    // DEPOSIT - Add money to account
    // ============================================================
    bool Deposit(int amount)
    {
        if (!CanDeposit(amount))
        {
            return false;
        }

        Balance += amount;

        if (GetBankingConfig().BankingLogs)
        {
            Print("[ATM System] Deposit: " + PlayerName + " deposited " + amount.ToString() + " (New Balance: " + Balance.ToString() + ")");
        }

        return true;
    }

    // ============================================================
    // CAN WITHDRAW - Check if withdrawal is allowed
    // ============================================================
    bool CanWithdraw(int amount)
    {
        if (amount <= 0)
        {
            return false;  // Invalid amount
        }

        if (Balance < amount)
        {
            return false;  // Insufficient funds
        }

        // Check daily withdrawal limit
        if (GetBankingConfig().DailyWithdrawalLimit > 0)
        {
            ResetDailyWithdrawalIfNeeded();

            if (DailyWithdrawn + amount > GetBankingConfig().DailyWithdrawalLimit)
            {
                return false;  // Would exceed daily limit
            }
        }

        return true;
    }

    // ============================================================
    // WITHDRAW - Remove money from account
    // ============================================================
    bool Withdraw(int amount)
    {
        if (!CanWithdraw(amount))
        {
            return false;
        }

        Balance -= amount;
        DailyWithdrawn += amount;

        if (GetBankingConfig().BankingLogs)
        {
            Print("[ATM System] Withdraw: " + PlayerName + " withdrew " + amount.ToString() + " (New Balance: " + Balance.ToString() + ")");
        }

        return true;
    }

    // ============================================================
    // CAN TRANSFER - Check if transfer is allowed
    // ============================================================
    bool CanTransfer(int amount, int fee)
    {
        if (amount <= 0)
        {
            return false;  // Invalid amount
        }

        int totalAmount = amount + fee;

        if (Balance < totalAmount)
        {
            return false;  // Insufficient funds (including fee)
        }

        if (Balance - totalAmount < 0)
        {
            return false;  // Would result in negative balance
        }

        return true;
    }

    // ============================================================
    // TRANSFER - Send money to another player
    // ============================================================
    bool Transfer(int amount, int fee)
    {
        if (!CanTransfer(amount, fee))
        {
            return false;
        }

        int totalDeduction = amount + fee;
        Balance -= totalDeduction;

        if (GetBankingConfig().BankingLogs)
        {
            Print("[ATM System] Transfer: " + PlayerName + " transferred " + amount.ToString() + " (Fee: " + fee.ToString() + ") (New Balance: " + Balance.ToString() + ")");
        }

        return true;
    }

    // ============================================================
    // RESET DAILY WITHDRAWAL - Reset daily counter if needed
    // ============================================================
    void ResetDailyWithdrawalIfNeeded()
    {
        int currentTime = GetGame().GetTickTime();
        int oneDayInSeconds = 86400;  // 24 hours

        if (currentTime - LastWithdrawReset >= oneDayInSeconds)
        {
            DailyWithdrawn = 0;
            LastWithdrawReset = currentTime;
        }
    }

    // ============================================================
    // GET REMAINING DAILY WITHDRAWAL - How much can still withdraw today
    // ============================================================
    int GetRemainingDailyWithdrawal()
    {
        if (GetBankingConfig().DailyWithdrawalLimit == 0)
        {
            return -1;  // -1 means unlimited
        }

        ResetDailyWithdrawalIfNeeded();
        return GetBankingConfig().DailyWithdrawalLimit - DailyWithdrawn;
    }

    // ============================================================
    // ADD TRANSACTION TO HISTORY
    // ============================================================
    void AddTransactionToHistory(BankingTransaction transaction)
    {
        TransactionHistory.Insert(transaction);

        // Limit history to last 100 transactions (prevent file bloat)
        if (TransactionHistory.Count() > 100)
        {
            TransactionHistory.Remove(0);  // Remove oldest
        }
    }

    // ============================================================
    // SAVE ACCOUNT TO JSON FILE
    // ============================================================
    void Save()
    {
        string bankingDir = "$profile:Banking/";

        // Create directory if it doesn't exist
        if (!FileExist(bankingDir))
        {
            MakeDirectory(bankingDir);
        }

        // Create backup if enabled
        if (GetBankingConfig().AutoBackup)
        {
            CreateBackup();
        }

        // Save account to JSON file
        string filePath = bankingDir + SteamID64 + ".json";
        JsonFileLoader<BankingAccount>.JsonSaveFile(filePath, this);

        if (GetBankingConfig().BankingLogs)
        {
            Print("[ATM System] Account saved: " + PlayerName + " (" + SteamID64 + ")");
        }
    }

    // ============================================================
    // CREATE BACKUP OF ACCOUNT FILE
    // ============================================================
    void CreateBackup()
    {
        string bankingDir = "$profile:Banking/";
        string backupDir = bankingDir + "Backups/";

        // Create backup directory if it doesn't exist
        if (!FileExist(backupDir))
        {
            MakeDirectory(backupDir);
        }

        // Backup filename with timestamp
        int currentTime = GetGame().GetTickTime();
        string backupFile = backupDir + SteamID64 + "_" + currentTime.ToString() + ".json";

        // Copy current file to backup
        string currentFile = bankingDir + SteamID64 + ".json";
        if (FileExist(currentFile))
        {
            BankingAccount temp = new BankingAccount();
            JsonFileLoader<BankingAccount>.JsonLoadFile(currentFile, temp);
            JsonFileLoader<BankingAccount>.JsonSaveFile(backupFile, temp);

            // Clean old backups
            CleanOldBackups();
        }
    }

    // ============================================================
    // CLEAN OLD BACKUPS - Remove backups older than retention period
    // ============================================================
    void CleanOldBackups()
    {
        string backupDir = "$profile:Banking/Backups/";
        int retentionDays = GetBankingConfig().BackupRetentionDays;
        int cutoffTime = GetGame().GetTickTime() - (retentionDays * 86400);

        // Find all backup files for this player
        array<string> files = {};
        FindFiles(backupDir, files, "*" + SteamID64 + "_*.json");

        foreach (string file : files)
        {
            // Extract timestamp from filename
            int timestampStart = file.IndexOf("_") + 1;
            int timestampEnd = file.IndexOf(".json");
            string timestampStr = file.Substring(timestampStart, timestampEnd - timestampStart);
            int timestamp = timestampStr.ToInt();

            // Delete if older than retention period
            if (timestamp < cutoffTime)
            {
                DeleteFile(file);
            }
        }
    }

    // ============================================================
    // LOAD ACCOUNT FROM JSON FILE (Static Factory Method)
    // ============================================================
    static ref BankingAccount Load(string playerID, string playerName = "")
    {
        string filePath = "$profile:Banking/" + playerID + ".json";

        BankingAccount account;

        // Check if account file exists
        if (FileExist(filePath))
        {
            // Load existing account
            account = new BankingAccount();
            JsonFileLoader<BankingAccount>.JsonLoadFile(filePath, account);

            if (GetBankingConfig().BankingLogs)
            {
                Print("[ATM System] Account loaded: " + account.PlayerName + " (" + playerID + ")");
            }
        }
        else
        {
            // Create new account
            account = new BankingAccount();
            account.SteamID64 = playerID;
            account.PlayerName = playerName;
            account.Balance = GetBankingConfig().DefaultStartBalance;
            account.MaxBalance = GetBankingConfig().DefaultMaxBalance;
            account.Save();

            if (GetBankingConfig().BankingLogs)
            {
                Print("[ATM System] New account created: " + playerName + " (" + playerID + ") with starting balance: " + account.Balance.ToString());
            }
        }

        return account;
    }

    // ============================================================
    // HELPER - Find all files in directory
    // ============================================================
    void FindFiles(string path, array<string> files, string pattern)
    {
        // Note: DayZ EnScript doesn't have built-in directory traversal
        // This is a placeholder for the logic
        // In production, you'd use a different approach or external tools
    }

    // ============================================================
    // HELPER - Delete file
    // ============================================================
    void DeleteFile(string filePath)
    {
        // Note: DayZ EnScript doesn't have built-in file deletion
        // This is a placeholder
        // In production, you'd use a different approach
    }
}
