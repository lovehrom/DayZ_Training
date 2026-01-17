// config.cpp
// Player modifiers configuration

class CfgPatches
{
    class PlayerModifiers
    {
        units[]={"EnergyDrink"};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};

class CfgVehicles
{
    class Drink_Base;
    class EnergyDrink: Drink_Base
    {
        scope = 2;
        displayName = "Energy Drink";
        description = "Boosts stamina for 5 minutes";
        nutrition = 100;
        hydration = 300;
        energy = 500;
        weight = 200;
    };
};
