// ATM System - Configuration File
// This file defines the ATM entity and credit card item

class CfgPatches
{
    class ATM_System
    {
        units[] = {
            "Land_ATM",           // Physical ATM object
            "CreditCard"          // Credit card item
        };
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {
            "DZ_Data",            // Vanilla DayZ data
            "DZ_Characters",      // Player characters
            "DZ_Items_Weapons"    // Weapon items (for card model)
        };
    };
};

// ============================================================
// ATM ENTITY - Physical ATM object in game world
// ============================================================
class CfgVehicles
{
    class House;  // Base building class
    class Land_Deutshe: House  // Vanilla building model (looks like ATM)
    {
        scope = 1;  // Protected (not spawnable by default)
    };

    class Land_ATM: Land_Deutshe
    {
        scope = 2;  // Public (spawnable in Eden Editor and by scripts)

        displayname = "Automated Teller Machine (ATM)";
        descriptionShort = "Use to access your bank account";

        model = "\dz\structures\commercial\deutshe\deutshe.p3d";

        // CRITICAL: This ID identifies the object as an ATM!
        // ActionOpenATM.c checks for TraderID == -2
        TraderID = -2;

        // Building properties
        destrType = DestructNo;  // Cannot be destroyed
        healthLevels[] = {
            {
                "health",
                1.0  // Full health
            }
        };

        // No inventory (can't be looted)
        inventorySlot[] = {};
    };

    // ============================================================
    // CREDIT CARD ITEM - Optional item for ATM access
    // ============================================================
    class ItemBase;
    class CreditCard: ItemBase
    {
        scope = 2;  // Public (spawnable)

        displayname = "Bank Credit Card";
        descriptionShort = "A credit card for accessing ATM machines. Required if enabled in config.";

        weight = 10;  // 10 grams (very light)

        // Use card model from vanilla weapons (prototype card)
        model = "\dz\weapons\prototypes\card.p3d";

        // Item properties
        consumable = 0;  // Not consumable
        usable = 1;      // Can be used (held in hands)
        edible = -1;     // Not edible
        drinkable = -1;  // Not drinkable

        // Item slots
        itemSize[] = {1, 1};  // Takes 1x1 slot in inventory

        // No attachments
        attachments[] = {};

        // Not repairable
        repairable = 0;

        // No damage (can't be damaged)
        damage = 0;

        // Appearance
        skin = "CreditCard";
    };
};

// ============================================================
// ACTIONS - Register ATM interaction action
// ============================================================
class CfgActions
{
    class ActionOpenATM
    {
        type = "interact";
        priority = 0.5;
        condition = "true";
    };
};
