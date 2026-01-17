// FactionConstants.c
// Faction names, reputation thresholds, wanted levels

class FactionConstants
{
    // Faction names
    static const string FACTION_BLU = "Blu";
    static const string FACTION_ROSSI = "Rossi";
    static const string FACTION_VERDE = "Verde";
    static const string FACTION_GIALLI = "Gialli";
    static const string FACTION_HIVE = "Hive";

    // Reputation thresholds
    static const int REPUTATION_HOSTILE = -50;   // Below: KOS
    static const int REPUTATION_NEUTRAL = 0;     // Neutral
    static const int REPUTATION_FRIENDLY = 50;   // Above: Friendly

    // Reputation changes
    static const int REPUTATION_KILL_ENEMY = 20;     // Kill enemy faction
    static const int REPUTATION_KILL_ALLY = -50;     // Kill ally faction
    static const int REPUTATION_HELP_ALLY = 10;      // Help ally faction
    static const int REPUTATION_TRADE = 5;           // Trade with faction

    // Wanted system
    static const int WANTED_THRESHOLD = -30;         // Reputation below this = wanted
    static const int WANTED_BOUNTY = 1000;           // Bounty for wanted players
}
