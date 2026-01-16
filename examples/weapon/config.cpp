class CfgPatches
{
    class WeaponExample
    {
        units[] = {"CustomRifle"};
        weapons[] = {"CustomRifle"};
        requiredVersion = 0.1;
        requiredAddons[] = {
            "DZ_Data",
            "DZ_Weapons_Rifles"
        };
    };
};

class CfgVehicles
{
    class Rifle_Base;

    class CustomRifle: Rifle_Base
    {
        scope = 2;
        displayName = "Custom Rifle";
        description = "Кастомная винтовка";

        // Модель (заглушка)
        model = "\DZ\weapons\rifles\M4\M4.p3d";

        // Вес и размер
        weight = 3500;  // 3.5 кг
        itemSize[] = {6, 2};

        // Повреждение
        damage = 50;
        // Использует стандартные патроны 5.56mm

        // Magazine
        class Attachments
        {
            class CustomRifle_Magazine
            {
                slotName = "magazine";
                slotType = "CustomRifle_Mag";
            };
            class WeaponOptics
            {
                slotName = "weaponOptics";
                slotType = "weaponOptics";
            };
            class WeaponFlashlight
            {
                slotName = "weaponFlashlight";
                slotType = "weaponFlashlight";
            };
            class WeaponButtstock
            {
                slotName = "weaponButtstock";
                slotType = "weaponButtstock";
            };
        };

        // Economy
        nominal = 20;
        min = 10;
        max = 30;
        restock = 0;
        lifetime = 10800;
        quantity = 1;
    };
};

// Magazine для оружия
class CfgVehicles
{
    class Magazine_Base;

    class CustomRifle_Mag: Magazine_Base
    {
        scope = 2;
        displayName = "Custom Rifle Magazine";
        description = "Магазин для кастомной винтовки";
        model = "\DZ\weapons\magazines\M4_Mag\M4_Mag.p3d";

        weight = 200;
        itemSize[] = {1, 1};

        ammoCount = 30;  // 30 патронов
        class Ammo
        {
            type = "Bullet_556x45";
            count = 30;
        };

        nominal = 30;
        min = 15;
        max = 45;
        restock = 0;
        lifetime = 7200;
    };
};
