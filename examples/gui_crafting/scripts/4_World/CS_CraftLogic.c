// CS_CraftLogic.c
// Core crafting logic: inventory checking, item removal, result spawning

class CS_CraftLogic
{
    // Check if player has all required ingredients
    static bool CanCraft(PlayerBase player, CS_Recipe recipe)
    {
        if (!player || !recipe)
            return false;

        for (int i = 0; i < recipe.Ingredients.Count(); i++)
        {
            CS_Ingredient ingredient = recipe.Ingredients.Get(i);
            int playerHas = CountItemInInventory(player, ingredient.Classname);

            if (playerHas < ingredient.Quantity)
                return false; // Missing ingredients
        }

        return true; // Has all ingredients
    }

    // Count how many items of a type the player has
    // Stackable items (nails, ammo): counts total quantity
    // Non-stackable items (food, tools): counts whole items
    static int CountItemInInventory(PlayerBase player, string classname)
    {
        int totalCount = 0;

        array<EntityAI> items = new array<EntityAI>();
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

        for (int i = 0; i < items.Count(); i++)
        {
            EntityAI item = items.Get(i);

            if (item && item.GetType() == classname)
            {
                ItemBase itemBase = ItemBase.Cast(item);

                if (itemBase && !itemBase.IsRuined())
                {
                    // Check if it's a stackable item (nails, ammo, rags)
                    if (itemBase.ConfigGetBool("canBeSplit"))
                    {
                        // Stackable: count by quantity
                        if (itemBase.HasQuantity())
                        {
                            totalCount += itemBase.GetQuantity();
                        }
                        else
                        {
                            totalCount += 1;
                        }
                    }
                    else
                    {
                        // Non-stackable (food, tools, bottles, weapons)
                        // Only count if >80% quantity/condition
                        if (itemBase.HasQuantity())
                        {
                            float maxQty = itemBase.GetQuantityMax();
                            float currentQty = itemBase.GetQuantity();
                            float percentage = (currentQty / maxQty) * 100;

                            if (percentage > 80)
                            {
                                totalCount += 1;
                            }
                        }
                        else
                        {
                            // Tools, weapons without quantity count as 1
                            totalCount += 1;
                        }
                    }
                }
            }
        }

        return totalCount;
    }

    // Execute crafting: consume ingredients and create result
    // IMPORTANT: Only runs on server
    static bool DoCraft(PlayerBase player, CS_Recipe recipe)
    {
        if (!GetGame().IsServer())
            return false;

        if (!CanCraft(player, recipe))
            return false;

        // Consume ingredients
        for (int i = 0; i < recipe.Ingredients.Count(); i++)
        {
            CS_Ingredient ingredient = recipe.Ingredients.Get(i);
            RemoveItemsFromInventory(player, ingredient.Classname, ingredient.Quantity);
        }

        // Create result item
        SpawnResultItem(player, recipe.ResultItem, recipe.ResultQuantity);

        return true;
    }

    // Remove specific quantity of items from inventory
    // Stackable items: removes by quantity
    // Non-stackable items: deletes whole items
    static void RemoveItemsFromInventory(PlayerBase player, string classname, int quantityToRemove)
    {
        int remaining = quantityToRemove;

        array<EntityAI> items = new array<EntityAI>();
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

        for (int i = 0; i < items.Count() && remaining > 0; i++)
        {
            EntityAI item = items.Get(i);

            if (item && item.GetType() == classname)
            {
                ItemBase itemBase = ItemBase.Cast(item);

                if (itemBase && !itemBase.IsRuined())
                {
                    // Check if it's a stackable item
                    if (itemBase.ConfigGetBool("canBeSplit"))
                    {
                        // Stackable: remove by quantity
                        if (itemBase.HasQuantity())
                        {
                            int itemQty = itemBase.GetQuantity();

                            if (itemQty <= remaining)
                            {
                                remaining -= itemQty;
                                GetGame().ObjectDelete(item); // Delete entire stack
                            }
                            else
                            {
                                itemBase.AddQuantity(-remaining); // Reduce quantity
                                remaining = 0;
                            }
                        }
                        else
                        {
                            remaining -= 1;
                            GetGame().ObjectDelete(item);
                        }
                    }
                    else
                    {
                        // Non-stackable: delete whole item
                        remaining -= 1;
                        GetGame().ObjectDelete(item);
                    }
                }
            }
        }
    }

    // Create result item on ground
    static void SpawnResultItem(PlayerBase player, string classname, int quantity)
    {
        if (!player || classname == "")
            return;

        // Use SpawnEntityOnGroundPos for proper sync
        EntityAI spawnedItem = EntityAI.Cast(player.SpawnEntityOnGroundPos(classname, player.GetPosition()));

        if (spawnedItem)
        {
            ItemBase spawnedItemBase = ItemBase.Cast(spawnedItem);

            // Set quantity
            if (spawnedItemBase && spawnedItemBase.HasQuantity())
            {
                // Check if it's stackable (ammo, nails)
                if (spawnedItemBase.ConfigGetBool("canBeSplit"))
                {
                    // Stackable: set to specified quantity
                    spawnedItemBase.SetQuantity(quantity);
                }
                else
                {
                    // Non-stackable (food, bottles): set to MAX (100%)
                    float maxQuantity = spawnedItemBase.GetQuantityMax();
                    spawnedItemBase.SetQuantity(maxQuantity);
                }
            }

            // If it's ammo pile
            if (spawnedItem.IsAmmoPile())
            {
                Ammunition_Base ammoPile = Ammunition_Base.Cast(spawnedItem);
                if (ammoPile)
                {
                    ammoPile.ServerSetAmmoCount(quantity);
                }
            }

            // If it's a magazine
            Magazine mag = Magazine.Cast(spawnedItem);
            if (mag)
            {
                mag.ServerSetAmmoCount(quantity);
            }

            Print("[CraftSystem] Item crafted: " + classname + " x" + quantity.ToString());
        }
        else
        {
            Print("[CraftSystem] ERROR: Could not create item: " + classname);
        }
    }
}
