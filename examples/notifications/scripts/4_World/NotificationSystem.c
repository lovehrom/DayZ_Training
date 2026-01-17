// NotificationSystem.c
// Server-to-client notification system

class NotificationSystem
{
    // Send notification to single player
    static void Notify(PlayerBase player, string message)
    {
        if (!player || !player.GetIdentity()) return;

        Param1<string> msg = new Param1<string>(message);
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, player.GetIdentity());

        Print("[Notification] To " + player.GetIdentity().GetName() + ": " + message);
    }

    // Broadcast to all players
    static void Broadcast(string message)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player)
            {
                Notify(player, message);
            }
        }

        Print("[Notification] Broadcast: " + message);
    }

    // Achievement notification
    static void Achievement(PlayerBase player, string achievement)
    {
        string msg = "Achievement: " + achievement;
        Notify(player, msg);
    }

    // Server message
    static void ServerMessage(string message)
    {
        Broadcast("[SERVER] " + message);
    }
}
