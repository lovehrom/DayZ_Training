// ============================================================
// RPC CHAT EXAMPLE - Broadcast messages
// ============================================================
// This example demonstrates broadcasting to all players:
// 1. Client sends chat message to server
// 2. Server broadcasts to all connected players
// 3. All clients receive and display message

modded class CustomMission
{
    // ============================================================
    // REGISTER CHAT RPC
    // ============================================================
    void RegisterChatRPC()
    {
        // Server-side RPC
        GetRPCManager().AddRPC("CF_RPC_Example", "ChatMessage", this,
            SingleplayerExecutionType.Server);

        // Client-side RPC
        GetRPCManager().AddRPC("CF_RPC_Example", "ReceiveChat", this,
            SingleplayerExecutionType.Client);

        Print("[CF_RPC_Example] Chat RPC registered");
    }

    // ============================================================
    // SERVER: Receive and broadcast chat message
    // ============================================================
    void ChatMessage(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on server
        if (type != CallType.Server)
            return;

        // Validate sender
        if (!sender)
        {
            Print("[CF_RPC_Example] Error: ChatMessage without sender");
            return;
        }

        // Read message
        Param1<string> data;
        if (!ctx.Read(data))
        {
            Print("[CF_RPC_Example] Error: Failed to read chat message");
            return;
        }

        string message = data.param1;
        string playerName = sender.GetName();
        string playerID = sender.GetPlainId();

        // Validate message
        if (message.Length() == 0)
        {
            Print("[CF_RPC_Example] Warning: Empty message from " + playerName);
            return;
        }

        if (message.Length() > 128)
        {
            Print("[CF_RPC_Example] Warning: Message too long from " + playerName);
            message = message.Substring(0, 128);
        }

        // Log
        Print("[CF_RPC_Example] [CHAT] " + playerName + ": " + message);

        // Broadcast to all players
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        int playerCount = 0;

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player)
            {
                GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveChat",
                    new Param2<string, string>(playerName, message),
                    true,  // Guaranteed delivery
                    player.GetIdentity());

                playerCount++;
            }
        }

        Print("[CF_RPC_Example] Broadcasted message to " + playerCount.ToString() + " players");
    }

    // ============================================================
    // CLIENT: Receive chat message
    // ============================================================
    void ReceiveChat(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on client
        if (type != CallType.Client)
            return;

        // Read parameters
        Param2<string, string> data;
        if (!ctx.Read(data))
            return;

        string playerName = data.param1;
        string message = data.param2;

        // Display
        string fullMessage = playerName + ": " + message;
        Print("[CF_RPC_Example] [CHAT] " + fullMessage);

        // Show notification
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            NotificationSystem.Notify(player, fullMessage);
        }
    }
}
