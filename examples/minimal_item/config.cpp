class CfgPatches
{
    class MinimalItem
    {
        units[] = {"MinimalItem"};
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
    class ItemBase;

    class MinimalItem: ItemBase
    {
        scope = 2;
        displayName = "Minimal Item";
        description = "Минимальный тестовый предмет для обучения моддингу DayZ";
        model = "\DZ\items\apple.p3d";

        weight = 100;
        itemSize[] = {1, 1};

        nominal = 100;
        min = 50;
        max = 150;
        restock = 0;
        lifetime = 7200;
        quantity = 1;
    };
};
