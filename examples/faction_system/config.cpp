// config.cpp
// Faction system configuration

class CfgPatches
{
    class FactionSystem
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]=
        {
            "DZ_Data",
            "DZ_Scripts"
        };
    };
};

class CfgMods
{
    class FactionSystem
    {
        type="mod";
        author="Claude AI (based on TheHive by Hyerakon)";
        dir="FactionSystem";
        name="Faction System";
        dependencies[]=
        {
            "World",
            "Game"
        };
        class defs
        {
            class gameScriptModule
            {
                value="";
                files[]=
                {
                    "FactionSystem/Scripts/3_Game"
                };
            };
            class worldScriptModule
            {
                value="";
                files[]=
                {
                    "FactionSystem/Scripts/4_World"
                };
            };
        };
    };
};
