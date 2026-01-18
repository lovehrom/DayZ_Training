// Mail System - Configuration File
// Defines mailbox entity

class CfgPatches
{
    class MailSystem
    {
        units[] = {"MailBox"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgVehicles
{
    class Container_Base;

    // Static mailbox (placed by admin)
    class MailBox: Container_Base
    {
        scope = 2;
        displayName = "Mailbox";

        // Model (use vanilla mailbox model if available)
        model = "\dz\structures\commercial\mailbox\mailbox.p3d";

        // Inventory properties
        inventorySlot[] = {"Cargo"};
        weight = 1000;  // 1000 kg (heavy, can't be moved)

        // Mailbox properties
        maximumCargo = 0;  // No actual inventory (mail stored in JSON)
    };
};
