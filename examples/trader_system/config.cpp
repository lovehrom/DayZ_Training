class CfgPatches
{
    class TraderSystem
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

    // Currency items
    class CoinItem: ItemBase
    {
        scope = 2;
        displayName = "Coin";
        descriptionShort = "A valuable coin used for trading";
        weight = 1;
        value = 1;
    };

    class Coin_10: CoinItem
    {
        displayName = "10 Coins";
        value = 10;
    };

    class Coin_50: CoinItem
    {
        displayName = "50 Coins";
        value = 50;
    };

    class Coin_100: CoinItem
    {
        displayName = "100 Coins";
        value = 100;
    };
};

class CfgPatches
{
    class TraderSystem
    {
        units[] =
        {
            "CoinItem",
            "Coin_10",
            "Coin_50",
            "Coin_100"
        };
    };
};
