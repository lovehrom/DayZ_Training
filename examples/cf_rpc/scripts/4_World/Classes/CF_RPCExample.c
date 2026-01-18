// ============================================================
// CF RPC EXAMPLE - Integration with PlayerBase
// ============================================================
// This file integrates all RPC examples with PlayerBase
// Adds chat commands for testing RPC functionality

modded class PlayerBase
{
    // ============================================================
    // INIT - Register all RPC handlers
    // ============================================================
    override void OnInit()
    {
        super.OnInit();

        // Register all RPC examples
        RegisterEchoRPC();
        RegisterPlayerDataRPC();
        RegisterChatRPC();
        RegisterArrayRPC();

        Print("[CF_RPC_Example] All RPC handlers registered");
    }

    // ============================================================
    // CHAT COMMANDS - Test RPC functionality
    // ============================================================
    override bool ChatCommand(string command)
    {
        array<string> args = new array<string>;
        command.Split(" ", args);

        if (args.Count() == 0)
            return super.ChatCommand(command);

        string cmd = args[0];

        // RPC test commands
        switch (cmd)
        {
            case "/rpc_test":
                return CommandRPCTest(args);

            case "/echo":
                return CommandEcho(args);

            case "/balance":
                return CommandRequestData("balance");

            case "/level":
                return CommandRequestData("level");

            case "/chat":
                return CommandChat(args);

            case "/array":
                return CommandArray(args);

            default:
                return super.ChatCommand(command);
        }
    }

    // ============================================================
    // COMMAND: rpc_test - Show available commands
    // ============================================================
    bool CommandRPCTest(array<string> args)
    {
        string message = "CF RPC Example Commands:\n";
        message += "/echo <message> - Test echo RPC\n";
        message += "/balance - Request player balance\n";
        message += "/level - Request player level\n";
        message += "/chat <message> - Broadcast chat message\n";
        message += "/array <count> - Test array transfer";

        Print(message);
        NotificationSystem.Notify(this, "Available commands: /echo, /balance, /level, /chat, /array");
        return true;
    }

    // ============================================================
    // COMMAND: echo - Test echo RPC
    // ============================================================
    bool CommandEcho(array<string> args)
    {
        if (args.Count() < 2)
        {
            NotificationSystem.Notify(this, "Usage: /echo <message>");
            return true;
        }

        // Reconstruct message
        string message = "";
        for (int i = 1; i < args.Count(); i++)
        {
            if (i > 1)
                message += " ";
            message += args[i];
        }

        // Send echo RPC
        GetRPCManager().SendRPC("CF_RPC_Example", "Echo",
            new Param1<string>(message),
            true,
            GetIdentity());

        Print("[CF_RPC_Example] Sent echo: " + message);
        return true;
    }

    // ============================================================
    // COMMAND: balance/level - Request player data
    // ============================================================
    bool CommandRequestData(string dataType)
    {
        // Request data from server
        GetRPCManager().SendRPC("CF_RPC_Example", "RequestPlayerData",
            new Param1<string>(dataType),
            true,
            GetIdentity());

        Print("[CF_RPC_Example] Requested " + dataType);
        return true;
    }

    // ============================================================
    // COMMAND: chat - Broadcast chat message
    // ============================================================
    bool CommandChat(array<string> args)
    {
        if (args.Count() < 2)
        {
            NotificationSystem.Notify(this, "Usage: /chat <message>");
            return true;
        }

        // Reconstruct message
        string message = "";
        for (int i = 1; i < args.Count(); i++)
        {
            if (i > 1)
                message += " ";
            message += args[i];
        }

        // Send chat message
        GetRPCManager().SendRPC("CF_RPC_Example", "ChatMessage",
            new Param1<string>(message),
            true,
            GetIdentity());

        Print("[CF_RPC_Example] Sent chat: " + message);
        return true;
    }

    // ============================================================
    // COMMAND: array - Test array transfer
    // ============================================================
    bool CommandArray(array<string> args)
    {
        int count = 10;  // Default

        if (args.Count() >= 2)
        {
            count = args[1].ToInt();

            if (count <= 0 || count > 1000)
            {
                NotificationSystem.Notify(this, "Usage: /array <count: 1-1000>");
                return true;
            }
        }

        // Create array
        array<int> numbers = new array<int>;
        for (int i = 0; i < count; i++)
        {
            numbers.Insert(i + 1);  // 1, 2, 3, ..., count
        }

        // Send array to server
        GetRPCManager().SendRPC("CF_RPC_Example", "ProcessArray",
            new Param1<array<int>>(numbers),
            true,
            GetIdentity());

        Print("[CF_RPC_Example] Sent array with " + count.ToString() + " elements");
        return true;
    }
}
