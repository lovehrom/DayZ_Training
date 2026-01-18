# Config Patterns

**Source:** raw_data/game_config.cpp (68 KB)
**Analysis:** Common config.cpp patterns

---

## 📋 Basic Structure

```cpp
class CfgPatches
{
    class MyMod
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};
```

---

## 📋 CfgVehicles (Items)

```cpp
class CfgVehicles
{
    class ItemBase;
    class MyItem: ItemBase
    {
        scope = 2; // 0=hidden, 1=protected, 2=public
        displayName = "My Item";
        description = "Custom item";
        weight = 1000; // grams

        // Quantity
        varQuantityInit = 1;
        varQuantityMin = 0;
        varQuantityMax = 100;

        // Temperature
        TemperatureMin = -20;
        TemperatureMax = 50;
    };
};
```

---

## 📋 CfgWeapons (Weapons)

```cpp
class CfgWeapons
{
    class Rifle_Base;
    class MyWeapon: Rifle_Base
    {
        scope = 2;
        displayName = "Custom Rifle";

        // Magazines
        magazines[] = {"Mag_MYWEAPON"};

        // Optics
        class Optics
        {
            class M4_T3NRDSuppressor
            {
                item = "M4_T3NRDSuppressor";
                slot = "weaponOptics";
            };
        };
    };
};
```

---

## 📋 CfgMods (Mod Definition)

```cpp
class CfgMods
{
    class MyMod
    {
        type = "mod";
        author = "Author";
        dir = "MyMod";
        name = "My Mod";
        picture = "mod.paa";
        logo = "logo.paa";
        logoSmall = "logoSmall.paa";
        logoOver = "logoOver.paa";
        tooltip = "My Mod";

        dependencies[] = {"Game", "World"};

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"MyMod/Scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"MyMod/Scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"MyMod/Scripts/5_Mission"};
            };
        };
    };
};
```

---

## 📋 Actions

```cpp
class CfgActions
{
    class MyAction
    {
        displayName = "Do Something";
        displayNameProgress = "Doing...";
        priority = 0.5;
        icon = "icon.paa";
        condition = "true";
    };
};
```

---

## 🔗 Related

→ [Config](../topics/02_config.md) - Config basics

---

*Last updated: 2025-01-18*
