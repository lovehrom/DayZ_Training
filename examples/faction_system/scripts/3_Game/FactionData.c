// FactionData.c
// Faction data classes (relations, loadouts, reputation)

// Single faction data
class FactionData
{
    string Name;                           // Faction name
    string LoadoutName;                    // Loadout config name
    ref array<string> FriendlyFactions;    // Allied factions
    ref array<string> HostileFactions;     // Enemy factions
    string Color;                          // UI color (e.g., "0 0 255")

    void FactionData()
    {
        Name = "";
        LoadoutName = "";
        FriendlyFactions = new array<string>();
        HostileFactions = new array<string>();
        Color = "255 255 255";
    }
}

// Player faction data
class PlayerFactionData
{
    string FactionName;        // Current faction
    ref map<string, int> Reputation;  // Reputation per faction
    ref map<string, bool> Wanted;     // Wanted status per faction

    void PlayerFactionData()
    {
        FactionName = "";
        Reputation = new map<string, int>();
        Wanted = new map<string, bool>();
    }

    // Get reputation with faction
    int GetReputation(string faction)
    {
        if (!Reputation.Contains(faction))
        {
            Reputation.Insert(faction, 0); // Default neutral
        }
        return Reputation.Get(faction);
    }

    // Set reputation with faction
    void SetReputation(string faction, int value)
    {
        if (!Reputation.Contains(faction))
        {
            Reputation.Insert(faction, value);
        }
        else
        {
            Reputation.Set(faction, value);
        }

        // Check if wanted
        if (value < FactionConstants.WANTED_THRESHOLD)
        {
            SetWanted(faction, true);
        }
    }

    // Add reputation to faction
    void AddReputation(string faction, int amount)
    {
        int current = GetReputation(faction);
        SetReputation(faction, current + amount);
    }

    // Check if wanted by faction
    bool IsWanted(string faction)
    {
        if (!Wanted.Contains(faction))
        {
            return false;
        }
        return Wanted.Get(faction);
    }

    // Set wanted status
    void SetWanted(string faction, bool wanted)
    {
        if (!Wanted.Contains(faction))
        {
            Wanted.Insert(faction, wanted);
        }
        else
        {
            Wanted.Set(faction, wanted);
        }
    }

    // Check if friendly to player
    bool IsFriendly(string otherFaction)
    {
        int rep = GetReputation(otherFaction);
        return rep >= FactionConstants.REPUTATION_FRIENDLY;
    }

    // Check if hostile to player
    bool IsHostile(string otherFaction)
    {
        int rep = GetReputation(otherFaction);
        return rep <= FactionConstants.REPUTATION_HOSTILE;
    }
}
