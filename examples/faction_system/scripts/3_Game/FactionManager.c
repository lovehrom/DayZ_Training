// FactionManager.c
// Manages all factions, player reputation, wanted status

class FactionManager
{
    protected ref map<string, ref FactionData> m_Factions;
    protected ref map<string, ref PlayerFactionData> m_PlayerData;

    protected static ref FactionManager m_Instance;

    void FactionManager()
    {
        m_Factions = new map<string, ref FactionData>();
        m_PlayerData = new map<string, ref PlayerFactionData>();

        InitializeFactions();
    }

    // Get singleton instance
    static FactionManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new FactionManager();
        }
        return m_Instance;
    }

    // Initialize default factions
    void InitializeFactions()
    {
        // Blu Faction (Blue)
        FactionData blu = new FactionData();
        blu.Name = FactionConstants.FACTION_BLU;
        blu.LoadoutName = "BluLoadout";
        blu.Color = "0 0 255"; // Blue
        blu.FriendlyFactions = {"Verde", "Gialli"};
        blu.HostileFactions = {"Rossi", "Hive"};
        m_Factions.Insert("Blu", blu);

        // Rossi Faction (Red)
        FactionData rossi = new FactionData();
        rossi.Name = FactionConstants.FACTION_ROSSI;
        rossi.LoadoutName = "RossiLoadout";
        rossi.Color = "255 0 0"; // Red
        rossi.FriendlyFactions = {"Hive"};
        rossi.HostileFactions = {"Blu", "Verde", "Gialli"};
        m_Factions.Insert("Rossi", rossi);

        // Verde Faction (Green)
        FactionData verde = new FactionData();
        verde.Name = FactionConstants.FACTION_VERDE;
        verde.LoadoutName = "VerdeLoadout";
        verde.Color = "0 255 0"; // Green
        verde.FriendlyFactions = {"Blu", "Gialli"};
        verde.HostileFactions = {"Rossi", "Hive"};
        m_Factions.Insert("Verde", verde);

        // Gialli Faction (Yellow)
        FactionData gialli = new FactionData();
        gialli.Name = FactionConstants.FACTION_GIALLI;
        gialli.LoadoutName = "GialliLoadout";
        gialli.Color = "255 255 0"; // Yellow
        gialli.FriendlyFactions = {"Blu", "Verde"};
        gialli.HostileFactions = {"Rossi", "Hive"};
        m_Factions.Insert("Gialli", gialli);

        // Hive Faction (Purple/Enemy)
        FactionData hive = new FactionData();
        hive.Name = FactionConstants.FACTION_HIVE;
        hive.LoadoutName = "HiveLoadout";
        hive.Color = "128 0 128"; // Purple
        hive.FriendlyFactions = {"Rossi"};
        hive.HostileFactions = {"Blu", "Verde", "Gialli"};
        m_Factions.Insert("Hive", hive);

        Print("[FactionManager] Initialized " + m_Factions.Count() + " factions");
    }

    // Get faction data
    FactionData GetFactionData(string factionName)
    {
        if (!m_Factions.Contains(factionName))
        {
            return null;
        }
        return m_Factions.Get(factionName);
    }

    // Check if two factions are friendly
    bool IsFriendly(string faction1, string faction2)
    {
        if (faction1 == faction2) return true; // Same faction

        FactionData data = GetFactionData(faction1);
        if (!data) return false;

        foreach (string friendly : data.FriendlyFactions)
        {
            if (friendly == faction2)
            {
                return true;
            }
        }

        return false;
    }

    // Check if two factions are hostile
    bool IsHostile(string faction1, string faction2)
    {
        if (faction1 == faction2) return false; // Same faction

        FactionData data = GetFactionData(faction1);
        if (!data) return false;

        foreach (string hostile : data.HostileFactions)
        {
            if (hostile == faction2)
            {
                return true;
            }
        }

        return false;
    }

    // Get player data
    PlayerFactionData GetPlayerData(string playerID)
    {
        if (!m_PlayerData.Contains(playerID))
        {
            PlayerFactionData data = new PlayerFactionData();
            m_PlayerData.Insert(playerID, data);
        }
        return m_PlayerData.Get(playerID);
    }

    // Set player faction
    void SetFaction(PlayerBase player, string factionName)
    {
        if (!player) return;

        string playerID = player.GetIdentity().GetId();
        PlayerFactionData data = GetPlayerData(playerID);
        data.FactionName = factionName;

        // Initialize reputation with all factions
        foreach (string faction, FactionData factionData : m_Factions)
        {
            if (!data.Reputation.Contains(faction))
            {
                data.Reputation.Insert(faction, FactionConstants.REPUTATION_NEUTRAL);
            }
        }

        // Start with friendly reputation to own faction
        if (factionName != "")
        {
            data.SetReputation(factionName, FactionConstants.REPUTATION_FRIENDLY);
        }

        Print("[FactionManager] Player " + playerID + " joined faction: " + factionName);
    }

    // Get player faction
    string GetFaction(PlayerBase player)
    {
        if (!player) return "";

        string playerID = player.GetIdentity().GetId();
        PlayerFactionData data = GetPlayerData(playerID);
        return data.FactionName;
    }

    // Add reputation to player
    void AddReputation(PlayerBase player, string faction, int amount)
    {
        if (!player) return;

        string playerID = player.GetIdentity().GetId();
        PlayerFactionData data = GetPlayerData(playerID);
        data.AddReputation(faction, amount);

        int newRep = data.GetReputation(faction);
        Print("[FactionManager] Player " + playerID + " reputation with " + faction + ": " + newRep.ToString());
    }

    // Get player reputation with faction
    int GetReputation(PlayerBase player, string faction)
    {
        if (!player) return 0;

        string playerID = player.GetIdentity().GetId();
        PlayerFactionData data = GetPlayerData(playerID);
        return data.GetReputation(faction);
    }

    // Check if player is wanted by faction
    bool IsWanted(PlayerBase player, string faction)
    {
        if (!player) return false;

        string playerID = player.GetIdentity().GetId();
        PlayerFactionData data = GetPlayerData(playerID);
        return data.IsWanted(faction);
    }

    // Set wanted status
    void SetWanted(PlayerBase player, string faction, bool wanted)
    {
        if (!player) return;

        string playerID = player.GetIdentity().GetId();
        PlayerFactionData data = GetPlayerData(playerID);
        data.SetWanted(faction, wanted);

        Print("[FactionManager] Player " + playerID + " wanted by " + faction + ": " + wanted.ToString());
    }

    // Handle player killed (reputation changes)
    void OnPlayerKilled(PlayerBase killer, PlayerBase victim)
    {
        if (!killer || !victim) return;

        FactionManager manager = FactionManager.GetInstance();

        string killerFaction = manager.GetFaction(killer);
        string victimFaction = manager.GetFaction(victim);

        // Both in factions
        if (killerFaction != "" && victimFaction != "")
        {
            // Same faction (killed ally)
            if (killerFaction == victimFaction)
            {
                manager.AddReputation(killer, killerFaction, FactionConstants.REPUTATION_KILL_ALLY);
                Print("[FactionManager] " + killer.GetIdentity().GetName() + " killed ally! -" + FactionConstants.REPUTATION_KILL_ALLY.ToString());
            }
            // Different factions
            else
            {
                // Check if hostile
                if (manager.IsHostile(killerFaction, victimFaction))
                {
                    // Killed enemy
                    manager.AddReputation(killer, killerFaction, FactionConstants.REPUTATION_KILL_ENEMY);
                    Print("[FactionManager] " + killer.GetIdentity().GetName() + " killed enemy! +" + FactionConstants.REPUTATION_KILL_ENEMY.ToString());
                }
                else
                {
                    // Killed neutral/friendly
                    manager.AddReputation(killer, victimFaction, FactionConstants.REPUTATION_KILL_ALLY);
                    Print("[FactionManager] " + killer.GetIdentity().GetName() + " killed neutral! -" + FactionConstants.REPUTATION_KILL_ALLY.ToString());
                }
            }
        }
    }

    // Save faction data to JSON
    void SaveData()
    {
        // Implementation for JSON saving
        // Use JsonFileLoader to save m_PlayerData
        // File location: $profile:Factions/players.json

        string filePath = "$profile:Factions/players.json";

        // Make directory if needed
        if (!FileExist("$profile:Factions"))
        {
            MakeDirectory("$profile:Factions");
        }

        // Save data
        JsonFileLoader<map<string, ref PlayerFactionData>>.JsonSaveFile(filePath, m_PlayerData);
        Print("[FactionManager] Saved faction data for " + m_PlayerData.Count() + " players");
    }

    // Load faction data from JSON
    void LoadData()
    {
        string filePath = "$profile:Factions/players.json";

        if (FileExist(filePath))
        {
            JsonFileLoader<map<string, ref PlayerFactionData>>.JsonLoadFile(filePath, m_PlayerData);
            Print("[FactionManager] Loaded faction data for " + m_PlayerData.Count() + " players");
        }
        else
        {
            Print("[FactionManager] No existing faction data found");
        }
    }
}
