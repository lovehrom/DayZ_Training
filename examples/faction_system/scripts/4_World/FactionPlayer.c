// FactionPlayer.c
// PlayerBase extension for faction system (kill tracking, chat commands)

modded class PlayerBase
{
    protected ref FactionManager m_FactionManager;

    void PlayerBase()
    {
        m_FactionManager = FactionManager.GetInstance();
    }

    // Called when player connects
    override void OnConnect()
    {
        super.OnConnect();

        // Load faction data on server
        if (GetGame().IsServer())
        {
            // Initialize player data if needed
            string playerID = GetIdentity().GetId();

            // Set default faction (none)
            if (m_FactionManager.GetFaction(this) == "")
            {
                // Player is not in any faction
                Print("[FactionPlayer] Player " + GetIdentity().GetName() + " connected (no faction)");
            }
            else
            {
                Print("[FactionPlayer] Player " + GetIdentity().GetName() + " connected (faction: " + m_FactionManager.GetFaction(this) + ")");
            }
        }
    }

    // Called when player disconnects
    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save faction data
        if (GetGame().IsServer())
        {
            m_FactionManager.SaveData();
        }
    }

    // Called when player is killed
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Handle reputation changes
        if (GetGame().IsServer())
        {
            PlayerBase killerPlayer = PlayerBase.Cast(killer);

            if (killerPlayer)
            {
                m_FactionManager.OnPlayerKilled(killerPlayer, this);
            }
        }
    }

    // Chat command handler
    override bool ChatCommand(string command)
    {
        super.ChatCommand(command);

        // Parse command
        TStringArray tokens = new TStringArray();
        command.Split(" ", tokens);

        if (tokens.Count() == 0) return false;

        string cmd = tokens[0];

        // Faction commands
        switch (cmd)
        {
            case "/faction":
                CommandFaction(tokens);
                return true;

            case "/reputation":
            case "/rep":
                CommandReputation(tokens);
                return true;

            case "/join":
                CommandJoin(tokens);
                return true;

            case "/wanted":
                CommandWanted(tokens);
                return true;
        }

        return false;
    }

    // Command: /faction
    void CommandFaction(TStringArray tokens)
    {
        string myFaction = m_FactionManager.GetFaction(this);

        if (myFaction == "")
        {
            SendMessage("You are not in a faction. Use /join <faction> to join.");
        }
        else
        {
            FactionData data = m_FactionManager.GetFactionData(myFaction);

            string msg = "Your faction: " + myFaction;
            if (data)
            {
                msg += " (Color: " + data.Color + ")";
            }

            SendMessage(msg);
        }
    }

    // Command: /reputation or /rep
    void CommandReputation(TStringArray tokens)
    {
        string myFaction = m_FactionManager.GetFaction(this);

        if (myFaction == "")
        {
            SendMessage("You are not in a faction.");
            return;
        }

        // Show reputation with all factions
        foreach (string faction, FactionData data : m_FactionManager.m_Factions)
        {
            int rep = m_FactionManager.GetReputation(this, faction);

            string status = "Neutral";
            if (rep <= FactionConstants.REPUTATION_HOSTILE)
            {
                status = "Hostile";
            }
            else if (rep >= FactionConstants.REPUTATION_FRIENDLY)
            {
                status = "Friendly";
            }

            SendMessage(faction + ": " + rep.ToString() + " (" + status + ")");
        }
    }

    // Command: /join <faction>
    void CommandJoin(TStringArray tokens)
    {
        if (tokens.Count() < 2)
        {
            SendMessage("Usage: /join <faction>");
            SendMessage("Available factions: Blu, Rossi, Verde, Gialli, Hive");
            return;
        }

        string factionName = tokens[1];

        // Validate faction exists
        if (!m_FactionManager.m_Factions.Contains(factionName))
        {
            SendMessage("Unknown faction: " + factionName);
            SendMessage("Available factions: Blu, Rossi, Verde, Gialli, Hive");
            return;
        }

        // Join faction
        m_FactionManager.SetFaction(this, factionName);
        SendMessage("You joined faction: " + factionName);

        // Save data
        m_FactionManager.SaveData();
    }

    // Command: /wanted
    void CommandWanted(TStringArray tokens)
    {
        string myFaction = m_FactionManager.GetFaction(this);

        if (myFaction == "")
        {
            SendMessage("You are not in a faction.");
            return;
        }

        // Check wanted status with all factions
        bool isWanted = false;
        foreach (string faction, FactionData data : m_FactionManager.m_Factions)
        {
            if (m_FactionManager.IsWanted(this, faction))
            {
                SendMessage("WANTED by " + faction + "!");
                isWanted = true;
            }
        }

        if (!isWanted)
        {
            SendMessage("You are not wanted by any faction.");
        }
    }

    // Helper: Send message to player
    void SendMessage(string message)
    {
        Param1<string> msg = new Param1<string>(message);
        GetGame().RPCSingleParam(this, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, GetIdentity());
    }
}
