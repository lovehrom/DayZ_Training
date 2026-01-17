// LootBox.c
// Loot box with weighted random drops

class LootBox extends ItemBase
{
    override void Open(ItemBase item)
    {
        super.Open(item);

        // Give guaranteed drops first
        array<string> guaranteed = LootBoxConfig.GetGuaranteedDrops();
        for (int i = 0; i < guaranteed.Count(); i++)
        {
            SpawnItem(guaranteed.Get(i));
        }

        // Weighted random roll for rarity
        float roll = Math.RandomFloat01();
        int rarity = 0; // Common

        if (roll < 0.001)        // 0.1% - Legendary
            rarity = 3;
        else if (roll < 0.01)    // 1% - Epic
            rarity = 2;
        else if (roll < 0.10)    // 9% - Rare
            rarity = 1;
        else                     // 90% - Common
            rarity = 0;

        // Get random loot from rarity tier
        string reward = LootBoxConfig.GetRandomLoot(rarity);
        SpawnItem(reward);

        // Delete loot box
        GetGame().ObjectDelete(this);
    }

    void SpawnItem(string classname)
    {
        if (!classname || classname == "") return;

        EntityAI item = SpawnEntityOnGroundPos(classname, GetPosition());
        Print("[LootBox] Spawned: " + classname);
    }
}
