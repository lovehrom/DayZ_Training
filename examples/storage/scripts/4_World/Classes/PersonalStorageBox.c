class PersonalStorageBox extends Container_Base
{
    protected ref StorageManager m_StorageManager;
    protected bool m_IsLoading;

    override void EEInit()
    {
        super.EEInit();

        // Не загружаем на сервере при спавне
        // Загружаем только при открытии игроком
    }

    override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
    {
        super.OnPlacementComplete(player, position, orientation);

        // Это temporary box, удаляется после использования
        // Настоящее хранилище в StorageManager
    }

    override void Open()
    {
        super.Open();

        // Загружаем предметы из хранилища
        if (!m_IsLoading && GetGame().IsServer())
        {
            LoadStorage();
        }
    }

    override void Close()
    {
        super.Close();

        // Сохраняем предметы в хранилище
        if (GetGame().IsServer())
        {
            SaveStorage();
        }
    }

    void LoadStorage()
    {
        m_IsLoading = true;

        // Получаем игрока который открыл
        PlayerBase player = GetOpenerPlayer();

        if (!player || !player.GetIdentity())
        {
            m_IsLoading = false;
            return;
        }

        string steam64 = player.GetIdentity().GetId();

        Print("Loading storage for " + steam64);

        // Создаем менеджер хранилища
        m_StorageManager = new StorageManager(steam64);

        // Восстанавливаем предметы
        m_StorageManager.RestoreInventory(this, GetPosition());

        m_IsLoading = false;
    }

    void SaveStorage()
    {
        if (m_IsLoading)
            return;

        // Получаем игрока который закрыл
        PlayerBase player = GetOpenerPlayer();

        if (!player || !player.GetIdentity())
            return;

        string steam64 = player.GetIdentity().GetId();

        Print("Saving storage for " + steam64);

        // Создаем менеджер если не существует
        if (!m_StorageManager)
        {
            m_StorageManager = new StorageManager(steam64);
        }

        // Сохраняем все предметы
        m_StorageManager.StoreInventory(this);
    }

    PlayerBase GetOpenerPlayer()
    {
        // Ищем ближайшего игрока
        array<PlayerBase> players = new array<PlayerBase>;
        GetGame().GetPlayers(players);

        PlayerBase nearest = null;
        float minDistance = 3.0; // 3 метра

        foreach (PlayerBase player : players)
        {
            float distance = vector.Distance(GetPosition(), player.GetPosition());

            if (distance < minDistance)
            {
                minDistance = distance;
                nearest = player;
            }
        }

        return nearest;
    }

    override void SetActions()
    {
        super.SetActions();

        AddAction(ActionTogglePlaceObject);
        AddAction(ActionActionTarget);
        AddAction(ActionOpenFolder);
    }
}

// Helper class для notifications
class NotificationSystem
{
    static void Notify(PlayerBase player, string message)
    {
        if (!player)
            return;

        Param1<string> msg = new Param1<string>(message);
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, player.GetIdentity());
    }
}
