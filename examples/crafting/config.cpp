class CfgPatches
{
    class CraftingExample
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {
            "DZ_Data",
            "DZ_Items",
            "DZ_Actions"
        };
    };
};

class CfgVehicles
{
    class ItemBase;

    // Рецепт 1: Крафт камней
    class Stone: ItemBase
    {
        scope = 2;
        displayName = "Stone";
        description = "Простой камень, можно получить при крафте";
        model = "\DZ\items\stone.p3d";

        weight = 500;
        itemSize[] = {1, 1};

        nominal = 100;
        min = 50;
        max = 150;
        restock = 0;
        lifetime = 7200;
        quantity = 1;
    };

    // Рецепт 2: Крафт верёвки
    class Rope_Custom: ItemBase
    {
        scope = 2;
        displayName = "Rope (Custom)";
        description = "Верёвка, созданная из лоскутов ткани";
        model = "\DZ\items\rope.p3d";

        weight = 300;
        itemSize[] = {2, 1};

        nominal = 50;
        min = 25;
        max = 75;
        restock = 0;
        lifetime = 7200;
        quantity = 1;
    };
};
