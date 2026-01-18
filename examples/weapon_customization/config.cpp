// config.cpp
// Weapon customization configuration

class CfgPatches
{
    class WeaponCustomization
    {
        units[]={};
        weapons[]={"CustomAK"};
        requiredVersion=0.1;
        requiredAddons[]={"DZ_Data", "DZ_Scripts"};
    };
};

class CfgWeapons
{
    class Rifle_Base;
    class CustomAK: Rifle_Base
    {
        scope = 2;
        displayName = "Custom AK";
        description = "High damage, fast reload";
        drySound[] = {"DZ\Sounds\Weapons\RSAK\drySound", 0.5, 1, 20};
        reloadSound[] = {"DZ\Sounds\Weapons\RSAK\reloadSound", 0.5, 1, 20};
        magReloadSound[] = {"DZ\Sounds\Weapons\RSAK\magReloadSound", 0.5, 1, 20};

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
