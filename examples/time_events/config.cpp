// config.cpp
// Time events configuration

class CfgPatches
{
    class TimeEventSystem
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class TimeEventSystem
    {
        type="mod";
        author="Claude AI";
        dir="TimeEventSystem";
        name="Time Event System";
        dependencies[]={"Game", "World", "Mission"};
        class defs
        {
            class missionScriptModule
            {
                value="";
                files[]={"TimeEventSystem/Scripts/5_Mission"};
            };
        };
    };
};
