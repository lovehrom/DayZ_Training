// ============================================================
// BANKING CONFIGURATION
// ============================================================
// This class loads and manages banking system settings
// Settings are stored in banking_config.json

class BankingConfig
{
    // Configuration values (loaded from JSON)
    bool RequireCreditCard;
    float TransactionFee;
    int DefaultStartBalance;
    int DefaultMaxBalance;
    int MinTransactionAmount;
    int MaxSingleTransaction;
    int DailyWithdrawalLimit;
    int TransactionQueueSize;
    bool BankingLogs;
    ref TStringArray CurrenciesAccepted;
    bool AutoBackup;
    int BackupRetentionDays;
    bool ShowBalanceOnTransaction;
    bool ShowTransactionFee;
    bool NotifyOnReceive;
    bool NotifyOnSend;

    // Singleton instance
    private static ref BankingConfig m_Instance;

    // ============================================================
    // GET INSTANCE - Singleton pattern
    // ============================================================
    static BankingConfig GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new BankingConfig();
            m_Instance.Load();
        }

        return m_Instance;
    }

    // ============================================================
    // CONSTRUCTOR - Initialize default values
    // ============================================================
    void BankingConfig()
    {
        // Set default values (in case config file is missing)
        RequireCreditCard = false;
        TransactionFee = 0.05;  // 5%
        DefaultStartBalance = 0;
        DefaultMaxBalance = 1000000;  // 1 million
        MinTransactionAmount = 1;
        MaxSingleTransaction = 50000;
        DailyWithdrawalLimit = 0;  // No limit
        TransactionQueueSize = 100;
        BankingLogs = false;
        CurrenciesAccepted = new TStringArray;
        AutoBackup = true;
        BackupRetentionDays = 7;
        ShowBalanceOnTransaction = true;
        ShowTransactionFee = true;
        NotifyOnReceive = true;
        NotifyOnSend = true;
    }

    // ============================================================
    // LOAD CONFIGURATION FROM JSON FILE
    // ============================================================
    void Load()
    {
        string configPath = "$profile:banking_config.json";

        // Check if config file exists
        if (FileExist(configPath))
        {
            // Load from JSON
            JsonFileLoader<BankingConfig>.JsonLoadFile(configPath, this);

            Print("[ATM System] Configuration loaded from: " + configPath);
            Print("[ATM System] RequireCreditCard: " + RequireCreditCard.ToString());
            Print("[ATM System] TransactionFee: " + TransactionFee.ToString());
            Print("[ATM System] DefaultMaxBalance: " + DefaultMaxBalance.ToString());
        }
        else
        {
            // Create default config file
            Save();
            Print("[ATM System] Configuration file not found. Created default configuration at: " + configPath);
        }
    }

    // ============================================================
    // SAVE CONFIGURATION TO JSON FILE
    // ============================================================
    void Save()
    {
        string configPath = "$profile:banking_config.json";
        JsonFileLoader<BankingConfig>.JsonSaveFile(configPath, this);
        Print("[ATM System] Configuration saved to: " + configPath);
    }

    // ============================================================
    // RELOAD CONFIGURATION - Reload from file
    // ============================================================
    void Reload()
    {
        Load();
        Print("[ATM System] Configuration reloaded.");
    }

    // ============================================================
    // CALCULATE TRANSACTION FEE
    // ============================================================
    int CalculateFee(int amount)
    {
        return Math.Floor(amount * TransactionFee);
    }

    // ============================================================
    // VALIDATE TRANSACTION AMOUNT
    // ============================================================
    bool IsValidTransactionAmount(int amount)
    {
        if (amount < MinTransactionAmount)
        {
            return false;  // Too small
        }

        if (amount > MaxSingleTransaction)
        {
            return false;  // Too large
        }

        return true;
    }

    // ============================================================
    // CHECK IF CURRENCY IS ACCEPTED
    // ============================================================
    bool IsCurrencyAccepted(string currencyClassName)
    {
        return CurrenciesAccepted.Find(currencyClassName) >= 0;
    }
}

// ============================================================
// GLOBAL HELPER FUNCTION - Quick access to config
// ============================================================
BankingConfig GetBankingConfig()
{
    return BankingConfig.GetInstance();
}
