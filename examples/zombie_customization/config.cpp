// config.cpp
// Zombie customization configuration

class CfgPatches
{
    class ZombieCustomization
    {
        units[]={"CustomZombie"};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};

class CfgVehicles
{
    class ZombieBase;
    class CustomZombie: ZombieBase
    {
        scope = 2;
        displayName = "Custom Zombie";
        description = "Strong zombie with loot";
        model = "DZ\characters\zombies\walker\zombie_walker.p3d";

        // Increased damage
        damage = 100;

        // Custom loot
        class Loot
        {
            class Items
            {
                item = "AK74";
                count = 1;
            };
        };
    };
};
