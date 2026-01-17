modded class PlayerBase
{
    protected ref TeleportManager m_TeleportManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Создаем менеджер телепортации
        m_TeleportManager = new TeleportManager(this);

        Print("Player " + GetIdentity().GetName() + " connected. Teleport system initialized.");
    }

    // Доступ к менеджеру телепортации
    TeleportManager GetTeleportManager()
    {
        return m_TeleportManager;
    }

    // Обработка чата для команды /tp
    override bool OnChat(PlayerBase player, string message)
    {
        // Передаем в базовый класс сначала
        bool handled = super.OnChat(player, message);

        if (handled)
            return true;

        // Проверяем команду /tp
        if (message.StartsWith("/tp "))
        {
            string location = message.Substring(4); // После "/tp "

            if (location != "")
            {
                if (m_TeleportManager)
                {
                    m_TeleportManager.TeleportPlayer(location.Trim());
                }
                else
                {
                    NotificationSystem.Notify(this, "Teleport system not initialized!");
                }

                return true; // Сообщение обработано
            }
        }

        return false; // Сообщение не обработано
    }
}
