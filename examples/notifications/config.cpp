// config.cpp
// Notification system configuration

class CfgPatches
{
    class NotificationSystem
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class NotificationSystem
    {
        type="mod";
        author="Claude AI";
        dir="NotificationSystem";
        name="Notification System";
        dependencies[]={"Game", "World"};
        class defs
        {
            class worldScriptModule
            {
                value="";
                files[]={"NotificationSystem/Scripts/4_World"};
            };
        };
    };
};
