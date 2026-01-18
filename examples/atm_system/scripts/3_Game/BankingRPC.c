// ============================================================
// BANKING RPC - Client-Server Communication
// ============================================================
// This file handles all RPC calls for the banking system
// RPC (Remote Procedure Call) allows client and server to communicate

modded class CustomMission
{
    // ============================================================
    // ON INIT - Register RPC handlers
    // ============================================================
    override void OnMissionStart()
    {
        super.OnMissionStart();

        // Register RPC handlers
        GetRPCManager().AddRPC("ATM_System", "RequestBankData", this, SingeplayerExecutionType.Client);
        GetRPCManager().AddRPC("ATM_System", "ReceiveAccountData", this, SingeplayerExecutionType.Client);
        GetRPCManager().AddRPC("ATM_System", "DepositMoney", this, SingeplayerExecutionType.Server);
        GetRPCManager().AddRPC("ATM_System", "WithdrawMoney", this, SingeplayerExecutionType.Server);
        GetRPCManager().AddRPC("ATM_System", "TransferMoney", this, SingeplayerExecutionType.Server);

        Print("[ATM System] RPC handlers registered.");
    }

    // ============================================================
    // REQUEST BANK DATA - Client requests bank account data
    // ============================================================
    void RequestBankData(CallType type, ParamsReadContext ctx, PlayerBase sender)
    {
        if (type != CallType.Client)
        {
            return;
        }

        // Get player ID
        string playerID = sender.GetIdentity().GetPlainId();

        // Get player account
        BankingAccount account = BankingManager.GetInstance().GetPlayerAccount(playerID, sender.GetIdentity().GetName());

        // Send account data back to client
        GetRPCManager().SendRPC("ATM_System", "ReceiveAccountData",
            new Param1<ref BankingAccount>(account), true, sender.GetIdentity());

        // Open bank UI (simplified - just shows notification for now)
        NotificationSystem.Notify(sender, "Balance: " + account.Balance.ToString());
    }

    // ============================================================
    // RECEIVE ACCOUNT DATA - Client receives updated account data
    // ============================================================
    void ReceiveAccountData(CallType type, ParamsReadContext ctx, PlayerBase sender)
    {
        if (type != CallType.Client)
        {
            return;
        }

        // Read account data from RPC
        Param1<ref BankingAccount> data;
        if (!ctx.Read(data))
        {
            return;
        }

        BankingAccount account = data.param1;

        // Update local account data (for UI display)
        // This is where you would update the UI
        // For now, just show notification
        NotificationSystem.Notify(sender, "Account Updated. Balance: " + account.Balance.ToString());
    }

    // ============================================================
    // DEPOSIT MONEY - Server handles deposit request
    // ============================================================
    void DepositMoney(CallType type, ParamsReadContext ctx, PlayerBase sender)
    {
        if (type != CallType.Server)
        {
            return;
        }

        // Read amount from RPC
        Param1<int> data;
        if (!ctx.Read(data))
        {
            return;
        }

        int amount = data.param1;

        // Execute deposit
        BankingManager.GetInstance().DepositMoney(sender, amount);
    }

    // ============================================================
    // WITHDRAW MONEY - Server handles withdraw request
    // ============================================================
    void WithdrawMoney(CallType type, ParamsReadContext ctx, PlayerBase sender)
    {
        if (type != CallType.Server)
        {
            return;
        }

        // Read amount from RPC
        Param1<int> data;
        if (!ctx.Read(data))
        {
            return;
        }

        int amount = data.param1;

        // Execute withdraw
        BankingManager.GetInstance().WithdrawMoney(sender, amount);
    }

    // ============================================================
    // TRANSFER MONEY - Server handles transfer request
    // ============================================================
    void TransferMoney(CallType type, ParamsReadContext ctx, PlayerBase sender)
    {
        if (type != CallType.Server)
        {
            return;
        }

        // Read parameters from RPC
        Param2<string, int> data;
        if (!ctx.Read(data))
        {
            return;
        }

        string targetID = data.param1;
        int amount = data.param2;

        // Execute transfer
        BankingManager.GetInstance().TransferMoney(sender, targetID, amount);
    }
}
