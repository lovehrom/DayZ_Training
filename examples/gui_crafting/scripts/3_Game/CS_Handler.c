// CS_Handler.c
// Manages recipe loading from JSON, syncing to clients, auto-reload

class CS_Handler
{
    ref CS_Settings m_Settings;
    protected bool m_IsLoaded;

    void CS_Handler()
    {
        m_Settings = new CS_Settings();
        m_IsLoaded = false;

        // Only load on server
        if (GetGame().IsServer())
        {
            LoadSettings();

            // Auto-reload every 60 seconds
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ReloadSettings, 60000, true);
        }
    }

    void ~CS_Handler()
    {
        // Cleanup reload timer
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ReloadSettings);
    }

    // Load recipes from JSON file
    void LoadSettings()
    {
        // Create directory if missing
        if (!FileExist("$profile:CraftSystem"))
        {
            MakeDirectory("$profile:CraftSystem");
            Print("[CraftSystem] Directory created: $profile:CraftSystem");
        }

        string path = "$profile:CraftSystem/CS_Recipes.json";

        // Create default if missing
        if (!FileExist(path))
        {
            CreateDefaultSettings();
        }

        // Load JSON
        JsonFileLoader<CS_Settings>.JsonLoadFile(path, m_Settings);
        m_IsLoaded = true;

        Print("[CraftSystem] Settings loaded - " + m_Settings.Recipes.Count() + " recipes");
    }

    // Reload recipes and sync to all players
    void ReloadSettings()
    {
        string path = "$profile:CraftSystem/CS_Recipes.json";

        if (FileExist(path))
        {
            JsonFileLoader<CS_Settings>.JsonLoadFile(path, m_Settings);
            Print("[CraftSystem] Settings reloaded - " + m_Settings.Recipes.Count() + " recipes");

            // Send to all connected players
            if (GetGame().IsServer())
            {
                SendRecipesToAllPlayers();
            }
        }
    }

    // Send recipes to all connected players
    void SendRecipesToAllPlayers()
    {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        foreach (Man m : players)
        {
            if (m && m.GetIdentity())
            {
                GetGame().RPCSingleParam(m, CS_Constants.RPC_SEND_RECIPES, new Param1<ref CS_Settings>(m_Settings), true, m.GetIdentity());
            }
        }

        Print("[CraftSystem] Recipes sent to " + players.Count() + " players");
    }

    // Create default JSON config file
    void CreateDefaultSettings()
    {
        CS_Settings defaultSettings = new CS_Settings();

        // Example Recipe 1: AK-47
        CS_Recipe recipe1 = new CS_Recipe();
        recipe1.DisplayName = "AK-47";
        recipe1.ResultItem = "AK74";
        recipe1.ResultQuantity = 1;

        CS_Ingredient ing1_1 = new CS_Ingredient();
        ing1_1.Classname = "Nail";
        ing1_1.Quantity = 20;
        recipe1.Ingredients.Insert(ing1_1);

        CS_Ingredient ing1_2 = new CS_Ingredient();
        ing1_2.Classname = "MetalWire";
        ing1_2.Quantity = 1;
        recipe1.Ingredients.Insert(ing1_2);

        defaultSettings.Recipes.Insert(recipe1);

        // Example Recipe 2: Glasses
        CS_Recipe recipe2 = new CS_Recipe();
        recipe2.DisplayName = "Glasses";
        recipe2.ResultItem = "AviatorGlasses";
        recipe2.ResultQuantity = 1;

        CS_Ingredient ing2_1 = new CS_Ingredient();
        ing2_1.Classname = "SmallStone";
        ing2_1.Quantity = 1;
        recipe2.Ingredients.Insert(ing2_1);

        CS_Ingredient ing2_2 = new CS_Ingredient();
        ing2_2.Classname = "WoodenStick";
        ing2_2.Quantity = 1;
        recipe2.Ingredients.Insert(ing2_2);

        defaultSettings.Recipes.Insert(recipe2);

        // Save to file
        JsonFileLoader<CS_Settings>.JsonSaveFile("$profile:CraftSystem/CS_Recipes.json", defaultSettings);
        Print("[CraftSystem] Default config created with 2 example recipes");
    }

    // Get recipe settings
    CS_Settings GetSettings()
    {
        return m_Settings;
    }

    // Check if loaded
    bool IsLoaded()
    {
        return m_IsLoaded;
    }

    // Apply settings from server (client-side)
    void ApplySettings(CS_Settings settings)
    {
        if (settings)
        {
            m_Settings = settings;
            m_IsLoaded = true;
        }
    }
}

// Global handler instance
ref CS_Handler g_CS_Handler;

// Getter function
CS_Handler GetCSHandler()
{
    return g_CS_Handler;
}
