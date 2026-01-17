// config.cpp
// Building system configuration

class CfgPatches
{
    class BuildingSystem
    {
        units[]={"CustomBuilding"};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};

class CfgVehicles
{
    class House;
    class CustomBuilding: House
    {
        scope = 2;
        displayName = "Custom Shed";
        model = "DZ\structures\building\shed_w01\shed_w01.p3d";
    };
};
