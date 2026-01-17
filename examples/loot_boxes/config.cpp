// config.cpp
// Loot box configuration

class CfgPatches
{
    class LootBoxSystem
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class LootBoxSystem
    {
        type="mod";
        author="Claude AI";
        dir="LootBoxSystem";
        name="Loot Box System";
        dependencies[]={"Game", "World"};
        class defs
        {
            class gameScriptModule
            {
                value="";
                files[]={"LootBoxSystem/Scripts/3_Game"};
            };
            class worldScriptModule
            {
                value="";
                files[]={"LootBoxSystem/Scripts/4_World"};
            };
        };
    };
};
