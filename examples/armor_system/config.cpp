// config.cpp
// Armor system configuration

class CfgPatches
{
    class ArmorSystem
    {
        units[]={"CustomArmor"};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};

class CfgVehicles
{
    class Clothing;
    class CustomArmor: Clothing
    {
        scope = 2;
        displayName = "Custom Armor Vest";
        description = "Reduces damage by 50%";
        itemsPool[] = {"CustomArmor"};
        weight = 2000;
        itemSize[] = {3, 3};
    };
};
