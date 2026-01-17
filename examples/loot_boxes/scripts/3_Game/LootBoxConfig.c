// LootBoxConfig.c
// Drop tables for loot boxes

class LootBoxConfig
{
    // Common drops (90% chance)
    static array<string> GetCommonDrops()
    {
        return {"Bandage", "CanOpener", "Matches", "Compass", "Battery9V", "Rag"};
    }

    // Rare drops (9% chance)
    static array<string> GetRareDrops()
    {
        return {"AK74", "MP5K", "Izh18", "CZ75", "FNP45", "Makarov"};
    }

    // Epic drops (1% chance)
    static array<string> GetEpicDrops()
    {
        return {"M4A1", "SVD", "VSS", "AKM", "UMP45"};
    }

    // Legendary drops (0.1% chance)
    static array<string> GetLegendaryDrops()
    {
        return {"GoldenDeagle", "LAR", "NVGHeadstrap", "OpticScope"};
    }

    // Guaranteed drops (always get)
    static array<string> GetGuaranteedDrops()
    {
        return {"Bandage_Dressing", "SodaCan_Cola"};
    }

    // Get random item from rarity tier
    static string GetRandomLoot(int rarity)
    {
        array<string> items;

        switch (rarity)
        {
            case 0: // Common
                items = GetCommonDrops();
                break;
            case 1: // Rare
                items = GetRareDrops();
                break;
            case 2: // Epic
                items = GetEpicDrops();
                break;
            case 3: // Legendary
                items = GetLegendaryDrops();
                break;
            default:
                items = GetCommonDrops();
                break;
        }

        if (items.Count() == 0)
            return "Bandage";

        int index = Math.RandomInt(0, items.Count());
        return items.Get(index);
    }
}
