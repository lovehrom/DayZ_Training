// ============================================================
// BANKING TRANSACTION DATA STRUCTURE
// ============================================================
// This class represents a single banking transaction
// Used for transaction queue and logging

class BankingTransaction
{
    string TransactionID;         // Unique transaction ID
    string PlayerID;              // Steam64 ID of player
    string PlayerName;            // Player name (for logging)
    int Amount;                   // Transaction amount
    string TransactionType;       // "deposit", "withdraw", "transfer"
    string TargetID;              // Target player ID (for transfers)
    string TargetName;            // Target player name (for transfers)
    int Fee;                      // Transaction fee (for transfers)
    int Timestamp;                // Unix timestamp
    bool Processed;               // Whether transaction is completed
    string Status;                // "pending", "completed", "failed"

    // ============================================================
    // CONSTRUCTOR - Create new transaction
    // ============================================================
    void BankingTransaction(string playerID, string playerName, int amount, string type)
    {
        TransactionID = GenerateTransactionID();
        PlayerID = playerID;
        PlayerName = playerName;
        Amount = amount;
        TransactionType = type;
        Timestamp = GetGame().GetTickTime();
        Processed = false;
        Status = "pending";
        Fee = 0;
    }

    // ============================================================
    // GENERATE UNIQUE TRANSACTION ID
    // ============================================================
    string GenerateTransactionID()
    {
        return "TXN_" + PlayerID + "_" + Timestamp.ToString() + "_" + Math.RandomInt(1000, 9999).ToString();
    }

    // ============================================================
    // SET TRANSACTION TARGET (for transfers)
    // ============================================================
    void SetTarget(string targetID, string targetName)
    {
        TargetID = targetID;
        TargetName = targetName;
    }

    // ============================================================
    // SET TRANSACTION FEE
    // ============================================================
    void SetFee(int fee)
    {
        Fee = fee;
    }

    // ============================================================
    // MARK AS COMPLETED
    // ============================================================
    void MarkCompleted()
    {
        Processed = true;
        Status = "completed";
    }

    // ============================================================
    // MARK AS FAILED
    // ============================================================
    void MarkFailed(string reason)
    {
        Processed = true;
        Status = "failed: " + reason;
    }

    // ============================================================
    // GET TRANSACTION SUMMARY (for logging)
    // ============================================================
    string GetSummary()
    {
        string summary = "[" + TransactionType.ToUpper() + "] ";
        summary += PlayerName + " (" + PlayerID + ") ";
        summary += "Amount: " + Amount.ToString() + " ";

        if (TransactionType == "transfer")
        {
            summary += "→ " + TargetName + " (" + TargetID + ") ";
            if (Fee > 0)
            {
                summary += "(Fee: " + Fee.ToString() + ") ";
            }
        }

        summary += "Status: " + Status;
        return summary;
    }

    // ============================================================
    // SERIALIZATION - Save to JSON
    // ============================================================
    static string SerializeToJson(BankingTransaction transaction)
    {
        string json = "{";
        json += "\"TransactionID\":\"" + transaction.TransactionID + "\",";
        json += "\"PlayerID\":\"" + transaction.PlayerID + "\",";
        json += "\"PlayerName\":\"" + transaction.PlayerName + "\",";
        json += "\"Amount\":" + transaction.Amount.ToString() + ",";
        json += "\"TransactionType\":\"" + transaction.TransactionType + "\",";
        json += "\"TargetID\":\"" + transaction.TargetID + "\",";
        json += "\"TargetName\":\"" + transaction.TargetName + "\",";
        json += "\"Fee\":" + transaction.Fee.ToString() + ",";
        json += "\"Timestamp\":" + transaction.Timestamp.ToString() + ",";
        json += "\"Processed\":" + (transaction.Processed ? "true" : "false") + ",";
        json += "\"Status\":\"" + transaction.Status + "\"";
        json += "}";
        return json;
    }
}
