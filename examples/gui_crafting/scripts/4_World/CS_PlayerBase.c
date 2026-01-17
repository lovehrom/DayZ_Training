// CS_PlayerBase.c
// Client-Server RPC handlers for crafting requests

modded class PlayerBase
{
    // RPC handler (client + server)
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        // --- CLIENT SIDE ---

        // Client receives recipes from server
        if (rpc_type == CS_Constants.RPC_SEND_RECIPES)
        {
            Param1<ref CS_Settings> params;
            if (!ctx.Read(params)) return;

            if (g_CS_Handler)
            {
                g_CS_Handler.ApplySettings(params.param1);
                Print("[CraftSystem] Recipes received from server - " + params.param1.Recipes.Count() + " recipes");
            }
        }

        // Client receives craft result from server
        if (rpc_type == CS_Constants.RPC_CRAFT_RESULT)
        {
            Param2<bool, string> resultParams;
            if (!ctx.Read(resultParams)) return;

            bool success = resultParams.param1;
            string itemName = resultParams.param2;

            if (success)
            {
                NotificationSystem.AddNotificationExtended(10, CS_Constants.NOTIFICATION_TITLE, "You crafted: " + itemName, CS_Constants.NOTIFICATION_ICON);

                // Play craft sound
                PlayCraftSound();
            }
            else
            {
                NotificationSystem.AddNotificationExtended(10, CS_Constants.NOTIFICATION_TITLE, "Could not craft: " + itemName, CS_Constants.NOTIFICATION_ICON);
            }
        }

        // --- SERVER SIDE ---

        // Server receives craft request from client
        if (rpc_type == CS_Constants.RPC_REQUEST_CRAFT)
        {
            if (!GetGame().IsServer()) return;

            Param1<int> craftParams;
            if (!ctx.Read(craftParams)) return;

            int recipeIndex = craftParams.param1;

            // Get player who sent the request
            PlayerBase player = PlayerBase.Cast(sender.GetPlayer());
            if (!player) return;

            // Process crafting
            ProcessCraftRequest(player, recipeIndex);
        }
    }

    // Play sound effect (client-side)
    void PlayCraftSound()
    {
        SEffectManager.PlaySound(CS_Constants.SOUND_CRAFT, GetPosition());
    }

    // Process craft request on server
    void ProcessCraftRequest(PlayerBase player, int recipeIndex)
    {
        if (!GetGame().IsServer()) return;
        if (!g_CS_Handler || !g_CS_Handler.GetSettings()) return;

        CS_Settings settings = g_CS_Handler.GetSettings();

        if (recipeIndex < 0 || recipeIndex >= settings.Recipes.Count())
        {
            Print("[CraftSystem] ERROR: Invalid recipe index: " + recipeIndex.ToString());
            return;
        }

        CS_Recipe recipe = settings.Recipes.Get(recipeIndex);

        if (!recipe)
        {
            Print("[CraftSystem] ERROR: Null recipe at index: " + recipeIndex.ToString());
            return;
        }

        bool success = false;

        // Validate and execute craft
        if (CS_CraftLogic.CanCraft(player, recipe))
        {
            success = CS_CraftLogic.DoCraft(player, recipe);
        }

        // Send result back to client
        if (player.GetIdentity())
        {
            GetGame().RPCSingleParam(player, CS_Constants.RPC_CRAFT_RESULT, new Param2<bool, string>(success, recipe.DisplayName), true, player.GetIdentity());
        }

        Print("[CraftSystem] Craft processed - Recipe: " + recipe.DisplayName + " - Success: " + success.ToString());
    }

    // Client requests crafting from server
    void RequestCraft(int recipeIndex)
    {
        if (!GetIdentity()) return;

        // Send RPC to server
        GetGame().RPCSingleParam(this, CS_Constants.RPC_REQUEST_CRAFT, new Param1<int>(recipeIndex), true);

        Print("[CraftSystem] Requesting craft for recipe index: " + recipeIndex.ToString());
    }
}
