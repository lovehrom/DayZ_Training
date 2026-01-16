class CfgPatches
{
    class ClothingItem
    {
        units[] = {"Shirt_Custom"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {
            "DZ_Data",
            "DZ_Items",
            "DZ_Clothing"
        };
    };
};

class CfgVehicles
{
    class Clothing;

    class Shirt_Custom: Clothing
    {
        scope = 2;
        displayName = "Custom Shirt";
        description = "Кастомная рубашка с карманами.";
        model = "\DZ\clothing\shirt_beige.p3d";

        weight = 300;  // 0.3 кг
        itemSize[] = {2, 2};

        // Slots для attachments
        class Inventory
        {
            class Cargo
            {
                slots[] = {5};  // 5 слотов
            };
        };

        class Attachments
        {
            class Armband  // Нашивка на рукав
            {
                slotName = "Armband";
                slotType = "Armband";
            };
            class Shoulder  // Патронташ
            {
                slotName = "Shoulder";
                slotType = "Shoulder";
            };
        };

        // Economy
        nominal = 30;
        min = 10;
        max = 50;
        restock = 3600;
        lifetime = 10800;
    };
};
