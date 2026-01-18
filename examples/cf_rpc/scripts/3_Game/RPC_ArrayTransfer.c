// ============================================================
// RPC ARRAY TRANSFER EXAMPLE - Send arrays of data
// ============================================================
// This example demonstrates sending arrays:
// 1. Client sends array of numbers to server
// 2. Server processes array (sum, average, etc.)
// 3. Server sends result back to client

modded class CustomMission
{
    // ============================================================
    // REGISTER ARRAY RPC
    // ============================================================
    void RegisterArrayRPC()
    {
        // Server-side RPC
        GetRPCManager().AddRPC("CF_RPC_Example", "ProcessArray", this,
            SingleplayerExecutionType.Server);

        // Client-side RPC
        GetRPCManager().AddRPC("CF_RPC_Example", "ReceiveArrayResult", this,
            SingleplayerExecutionType.Client);

        Print("[CF_RPC_Example] Array Transfer RPC registered");
    }

    // ============================================================
    // SERVER: Process array and return result
    // ============================================================
    void ProcessArray(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on server
        if (type != CallType.Server)
            return;

        // Validate sender
        if (!sender)
        {
            Print("[CF_RPC_Example] Error: ProcessArray without sender");
            return;
        }

        // Read array
        Param1<array<int>> data;
        if (!ctx.Read(data))
        {
            Print("[CF_RPC_Example] Error: Failed to read array");
            return;
        }

        array<int> numbers = data.param1;

        if (!numbers)
        {
            Print("[CF_RPC_Example] Error: Array is null");
            return;
        }

        string playerName = sender.GetName();
        Print("[CF_RPC_Example] Received array from " + playerName + " with " + numbers.Count() + " elements");

        // Process array
        int sum = 0;
        int max = int.MIN;
        int min = int.MAX;

        foreach (int num : numbers)
        {
            sum += num;

            if (num > max)
                max = num;

            if (num < min)
                min = num;
        }

        float average = sum / (float)numbers.Count();

        // Log
        Print("[CF_RPC_Example] Array stats - Sum: " + sum.ToString() +
              " Avg: " + average.ToString() +
              " Min: " + min.ToString() +
              " Max: " + max.ToString());

        // Send result back
        GetRPCManager().SendRPC("CF_RPC_Example", "ReceiveArrayResult",
            new Param4<int, float, int, int>(sum, average, min, max),
            true,
            sender);

        Print("[CF_RPC_Example] Sent array result to " + playerName);
    }

    // ============================================================
    // CLIENT: Receive array processing result
    // ============================================================
    void ReceiveArrayResult(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        // Only execute on client
        if (type != CallType.Client)
            return;

        // Read parameters
        Param4<int, float, int, int> data;
        if (!ctx.Read(data))
            return;

        int sum = data.param1;
        float average = data.param2;
        int min = data.param3;
        int max = data.param4;

        // Format message
        string message = "Array Result:\n";
        message += "Sum: " + sum.ToString() + "\n";
        message += "Average: " + average.ToString() + "\n";
        message += "Min: " + min.ToString() + "\n";
        message += "Max: " + max.ToString();

        // Display
        Print("[CF_RPC_Example] " + message);

        // Show notification
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            NotificationSystem.Notify(player, "Sum: " + sum.ToString() + ", Avg: " + average.ToString());
        }
    }
}
