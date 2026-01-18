// ============================================================
// RPC ECHO EXAMPLE - Simple request/response
// ============================================================
// This example demonstrates the simplest RPC pattern:
// 1. Client sends message to server
// 2. Server echoes message back to client
// 3. Client displays response

modded class CustomMission
{
    // ============================================================
    // REGISTER ECHO RPC
    // ============================================================
    void RegisterEchoRPC()
    {
        // Register server-side RPC
        GetRPCManager().AddRPC("CF_RPC_Example", "Echo", this,
            SingleplayerExecutionType.Server);

        // Register client-side RPC
        GetRPCManager().AddRPC("CF_RPC_Example", "ReceiveEcho", this,
            SingleplayerExecutionType.Client);

        Print("[CF_RPC_Example] Echo RPC registered");
    }

    // ============================================================
    // SERVER: Echo message back to client
    // ============================================================
    void Echo(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on server
        if (type != CallType.Server)
            return;

        // Validate sender
        if (!sender)
        {
            Print("[CF_RPC_Example] Error: Echo called without valid sender");
            return;
        }

        // Read parameter
        Param1<string> data;
        if (!ctx.Read(data))
        {
            Print("[CF_RPC_Example] Error: Failed to read echo parameter");
            return;
        }

        string message = data.param1;

        // Log
        Print("[CF_RPC_Example] Server received: " + message);

        // Echo back to client
        GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveEcho",
            new Param1<string>("Echo: " + message),
            true,  // Guaranteed delivery
            sender);

        Print("[CF_RPC_Example] Server echoed: " + message);
    }

    // ============================================================
    // CLIENT: Receive echoed message
    // ============================================================
    void ReceiveEcho(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on client
        if (type != CallType.Client)
            return;

        // Read parameter
        Param1<string> data;
        if (!ctx.Read(data))
            return;

        string message = data.param1;

        // Display to player
        Print("[CF_RPC_Example] Client received: " + message);

        // Show notification (if PlayerBase available)
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            NotificationSystem.Notify(player, message);
        }
    }
}
