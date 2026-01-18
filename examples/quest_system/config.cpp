class CfgPatches
{
    class QuestSystem
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] =
        {
            "DZ_Data",
            "DZ_Scripts"
        };
    };
};

class CfgVehicles
{
    class DayZInfected;

    // Quest Giver NPC
    class QuestGiverNPC: DayZInfected
    {
        scope = 2;
        displayName = "Quest Giver";
        descriptionShort = "An NPC that gives quests";
        model = "DZ\\characters\\infantry\\infantry_walker.p3d";
    };
};

class CfgPatches
{
    class QuestSystem
    {
        units[] =
        {
            "QuestGiverNPC"
        };
    };
};
