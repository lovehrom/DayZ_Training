class CfgPatches
{
    class CustomNPC
    {
        units[] = {"CustomBandit"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgVehicles
{
    class InfectedBanditBase;
    class CustomBandit: InfectedBanditBase
    {
        scope = 2;
        displayName = "Bandit";

        // Модель
        model = "\DZ\characters\infected\infected_male.p3d";

        // Лут при смерти
        lootTable[] = {
            {"AKM", 0.3},
            {"Magazine_AKM_30Rnd", 0.5},
            {"CannedBeans", 0.8}
        };
    };
};

class CfgMods
{
    class CustomNPC
    {
        dir = "CustomNPC";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Custom NPC";
        logo = "";
        logoSmall = "";
        tooltip = "Custom NPC for DayZ";
        overview = "Working example of custom NPC";
        description = "Custom NPC";
        author = "DayZ Training";
    };
};
