class CustomBandit extends InfectedBanditBase
{
    protected ref AIBanditConfig m_Config;

    override void EEInit()
    {
        super.EEInit();

        // Загружаем конфигурацию
        m_Config = LoadConfig();

        // Применяем конфигурацию
        if (m_Config)
        {
            SetHealth("GlobalHealth", "Health", m_Config.Health);

            Print("CustomBandit initialized: Health=" + m_Config.Health);
        }
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Спавним лут
        SpawnLoot();
    }

    void SpawnLoot()
    {
        if (!m_Config)
            return;

        vector position = GetPosition();

        foreach (string item : m_Config.Loot)
        {
            // 50% шанс
            if (Math.RandomFloat01() < 0.5)
            {
                EntityAI spawned = GetGame().CreateObject(item, position);

                if (spawned)
                {
                    Print("Spawned loot: " + item);
                }
            }
        }
    }

    AIBanditConfig LoadConfig()
    {
        string filePath = "$profile:bandit_config.json";

        if (FileExist(filePath))
        {
            AIBanditConfig config;
            JsonFileLoader<AIBanditConfig>.JsonLoadFile(filePath, config);
            return config;
        }
        else
        {
            Print("No bandit config found, using defaults");

            // Default config
            AIBanditConfig defaultConfig = new AIBanditConfig;
            defaultConfig.Health = 200;
            defaultConfig.Damage = 50;
            defaultConfig.AgroRange = 100;
            defaultConfig.Loot = {"AKM", "Magazine_AKM_30Rnd", "CannedBeans"};
            return defaultConfig;
        }
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Custom AI behavior (опционально)

        // Ищем ближайшего игрока
        PlayerBase nearest = FindNearestPlayer();

        if (nearest)
        {
            float distance = vector.Distance(GetPosition(), nearest.GetPosition());

            // Агримся если игрок близко
            if (distance < m_Config.AgroRange)
            {
                AddTarget(nearest);
            }
        }
    }

    PlayerBase FindNearestPlayer()
    {
        array<PlayerBase> players = new array<PlayerBase>;
        GetGame().GetPlayers(players);

        PlayerBase nearest = null;
        float minDistance = m_Config.AgroRange;

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
}
