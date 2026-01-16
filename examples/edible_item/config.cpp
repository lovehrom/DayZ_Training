class CfgPatches
{
    class EdibleItem
    {
        units[] = {"ApplePie"};
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
    class Edible_Base;

    class ApplePie: Edible_Base
    {
        scope = 2;
        displayName = "Apple Pie";
        description = "Вкусный яблочный пирог. Восстанавливает 100 калорий.";
        model = "\DZ\items\apple.p3d";

        weight = 200;  // 0.2 кг
        itemSize[] = {1, 2};

        // Nutrition values
        foodType = FoodType.Meat;
        energy = 100;  // Калории
        water = 10;     // Вода
        nutritionalValue = 2;  // Пищевая ценность
        toxicity = 0;    // Токсичность

        // Temperature
        temperatureEnabled = 1;
        temperatureThreshold = 4;  // Замерзает при 4°C

        // Economy
        nominal = 50;
        min = 20;
        max = 80;
        restock = 0;
        lifetime = 7200;
        quantity = 1;
    };
};
