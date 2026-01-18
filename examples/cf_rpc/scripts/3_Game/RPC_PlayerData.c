// ============================================================
// RPC PLAYER DATA EXAMPLE - Request player information
// ============================================================
// This example demonstrates requesting data from server:
// 1. Client requests specific data (balance, level, etc.)
// 2. Server retrieves and sends back
// 3. Client receives and displays data

modded class CustomMission
{
    // ============================================================
    // REGISTER PLAYER DATA RPC
    // ============================================================
    void RegisterPlayerDataRPC()
    {
        // Server-side RPCs
        GetRPCManager().AddRPC("CF_RPC_Example", "RequestPlayerData", this,
            SingleplayerExecutionType.Server);

        // Client-side RPCs
        GetRPCManager().AddRPC("CF_RPC_Example", "ReceivePlayerData", this,
            SingleplayerExecutionType.Client);

        Print("[CF_RPC_Example] Player Data RPC registered");
    }

    // ============================================================
    // SERVER: Handle player data request
    // ============================================================
    void RequestPlayerData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on server
        if (type != CallType.Server)
            return;

        // Validate sender
        if (!sender)
        {
            Print("[CF_RPC_Example] Error: RequestPlayerData without sender");
            return;
        }

        // Read request type
        Param1<string> data;
        if (!ctx.Read(data))
        {
            Print("[CF_RPC_Example] Error: Failed to read request type");
            return;
        }

        string requestType = data.param1;
        string playerID = sender.GetPlainId();
        string playerName = sender.GetName();

        Print("[CF_RPC_Example] Player " + playerName + " requested: " + requestType);

        // Process request
        int response = 0;

        switch (requestType)
        {
            case "balance":
                response = GetPlayerBalance(playerID);
                break;

            case "level":
                response = GetPlayerLevel(playerID);
                break;

            case "kills":
                response = GetPlayerKills(playerID);
                break;

            case "deaths":
                response = GetPlayerDeaths(playerID);
                break;

            default:
                Print("[CF_RPC_Example] Unknown request type: " + requestType);
                return;
        }

        // Send response to client
        GetRPCManager().SendRPC("CF_RPC_Example", "ReceivePlayerData",
            new Param2<string, int>(requestType, response),
            true,
            sender);

        Print("[CF_RPC_Example] Sent " + requestType + " = " + response.ToString() + " to " + playerName);
    }

    // ============================================================
    // CLIENT: Receive player data
    // ============================================================
    void ReceivePlayerData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on client
        if (type != CallType.Client)
            return;

        // Read parameters
        Param2<string, int> data;
        if (!ctx.Read(data))
            return;

        string dataType = data.param1;
        int value = data.param2;

        // Display
        string message = dataType + ": " + value.ToString();
        Print("[CF_RPC_Example] Received " + message);

        // Show notification
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            NotificationSystem.Notify(player, message);
        }
    }

    // ============================================================
    // HELPER: Get player balance (mock data)
    // ============================================================
    int GetPlayerBalance(string playerID)
    {
        // In real mod, this would query database
        // For demo, return mock data
        return 1000 + playerID.Length() * 100;  // Deterministic "random" value
    }

    // ============================================================
    // HELPER: Get player level (mock data)
    // ============================================================
    int GetPlayerLevel(string playerID)
    {
        return 5 + playerID.Length() % 10;
    }

    // ============================================================
    // HELPER: Get player kills (mock data)
    // ============================================================
    int GetPlayerKills(string playerID)
    {
        return playerID.Length() * 2;
    }

    // ============================================================
    // HELPER: Get player deaths (mock data)
    // ============================================================
    int GetPlayerDeaths(string playerID)
    {
        return playerID.Length();
    }
}
