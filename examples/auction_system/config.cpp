class CfgPatches
{
    class AuctionSystem
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] =
        {
            "DZ_Data",
            "DZ_Scripts"
        };
    };
};

class CfgVehicles
{
    class ItemBase;

    // Auction Terminal
    class AuctionTerminal: ItemBase
    {
        scope = 2;
        displayName = "Auction Terminal";
        descriptionShort = "Access the player auction marketplace";
        model = "DZ\\weapons\\attachments\\UniversalLight.p3d";
        rotationFlags = 16;
        weight = 1000;
    };

    // Mailbox
    class Mailbox: ItemBase
    {
        scope = 2;
        displayName = "Mailbox";
        descriptionShort = "Receive auction items and mail";
        model = "DZ\\gear\\camping\\portablecampfire.p3d";
        rotationFlags = 16;
        weight = 5000;
    };
};

class CfgPatches
{
    class AuctionSystem
    {
        units[] =
        {
            "AuctionTerminal",
            "Mailbox"
        };
    };
};
