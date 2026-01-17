class TeleportManager
{
    protected ref array<ref TeleportNode> m_Nodes = new array<ref TeleportNode>;
    protected PlayerBase m_Player;

    void TeleportManager(PlayerBase player)
    {
        m_Player = player;
        LoadNodes();
    }

    void LoadNodes()
    {
        string filePath = "$profile:teleports.json";

        if (FileExist(filePath))
        {
            JsonFileLoader<array<ref TeleportNode>>::JsonLoadFile(filePath, m_Nodes);
            Print("Loaded " + m_Nodes.Count() + " teleport nodes");
        }
        else
        {
            Print("No teleport config found at " + filePath);
            Print("Create teleports.json in $profile: folder");
        }
    }

    array<TeleportNode> GetNodes()
    {
        return m_Nodes;
    }

    TeleportNode FindNode(string name)
    {
        foreach (TeleportNode node : m_Nodes)
        {
            if (node.Name == name)
                return node;
        }
        return null;
    }

    bool CanTeleport(PlayerBase player)
    {
        // Проверка 1: Не в транспорте
        if (player.GetCommand_Vehicle())
        {
            NotificationSystem.Notify(player, "Cannot teleport in vehicle!");
            return false;
        }

        // Проверка 2: Не unconscious
        if (player.IsUnconscious())
        {
            NotificationSystem.Notify(player, "Cannot teleport while unconscious!");
            return false;
        }

        // Проверка 3: Не restrained
        if (player.IsRestrained())
        {
            NotificationSystem.Notify(player, "Cannot teleport while restrained!");
            return false;
        }

        // Проверка 4: Не в combate (опционально)
        /*
        float lastDamageTime = player.GetLastDamageTime();
        float currentTime = GetGame().GetTickTime();

        if ((currentTime - lastDamageTime) < 300) // 5 минут
        {
            NotificationSystem.Notify(player, "Cannot teleport in combat!");
            return false;
        }
        */

        return true;
    }

    void TeleportPlayer(string location)
    {
        TeleportNode node = FindNode(location);

        if (!node)
        {
            NotificationSystem.Notify(m_Player, "Location not found: " + location);
            Print("Teleport location not found: " + location);
            return;
        }

        if (!CanTeleport(m_Player))
            return;

        // Телепортируем
        m_Player.SetPosition(node.Position);

        // Оповещаем
        NotificationSystem.Notify(m_Player, "Teleported to " + node.Name);
        Print("Player " + m_Player.GetIdentity().GetName() + " teleported to " + node.Name);

        // Эффект (опционально)
        m_Player.CreateSoundEffect("teleport_SoundSet");
    }
}

// Helper class для notifications
class NotificationSystem
{
    static void Notify(PlayerBase player, string message)
    {
        if (!player)
            return;

        // Отправляем сообщение в чат
        Param1<string> msg = new Param1<string>(message);
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, player.GetIdentity());
    }
}
