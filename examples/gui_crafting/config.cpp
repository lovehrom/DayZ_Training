// config.cpp
// Mod configuration for GUI Crafting System

class CfgPatches
{
    class CraftSystem
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
    class CraftSystem
    {
        type="mod";
        author="Reash & Claude AI";
        dir="CraftSystem";
        name="Craft System";
        inputs="CraftSystem/inputs/inputs.xml";
        dependencies[]=
        {
            "World",
            "Mission"
        };
        class defs
        {
            class gameScriptModule
            {
                value="";
                files[]=
                {
                    "CraftSystem/Scripts/3_Game"
                };
            };
            class worldScriptModule
            {
                value="";
                files[]=
                {
                    "CraftSystem/Scripts/4_World"
                };
            };
            class missionScriptModule
            {
                value="";
                files[]=
                {
                    "CraftSystem/Scripts/5_Mission"
                };
            };
        };
    };
};

// Sound configuration
class CfgSoundShaders
{
    class CS_Craft_SoundShader
    {
        samples[] = {{"CraftSystem/Sounds/sound", 1}};
        volume = 0.8;
        range = 30;
    };
};

class CfgSoundSets
{
    class CS_Craft_SoundSet
    {
        soundShaders[] = {"CS_Craft_SoundShader"};
        volumeFactor = 0.8;
        frequencyFactor = 1.0;
        spatial = 0;
    };
};
