class CfgPatches
{
    class ConsumableItem
    {
        units[] = {"Battery9V"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {
            "DZ_Data",
            "DZ_Items"
        };
    };
};

class CfgVehicles
{
    class Battery;

    class Battery9V: Battery
    {
        scope = 2;
        displayName = "9V Battery";
        description = "Батарейка 9 вольт для фонарика.";
        model = "\DZ\items\battery9v.p3d";

        weight = 50;  // 50 грамм
        itemSize[] = {1, 1};

        // Battery properties
        energy = 9;  // 9 вольт
        quantity = 1;

        // Economy
        nominal = 100;
        min = 50;
        max = 150;
        restock = 1800;  // Респавн каждые 30 минут
        lifetime = 7200;
    };
};
