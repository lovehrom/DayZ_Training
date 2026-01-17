// CS_HUD.c
// GUI crafting menu with recipe rows and item previews

class CS_RecipeRow
{
    private Widget m_Root;
    private CS_Recipe m_Recipe;
    private int m_RecipeIndex;
    private ButtonWidget m_ButtonCraft;
    private ref array<EntityAI> m_PreviewEntities;

    void CS_RecipeRow()
    {
        m_PreviewEntities = new array<EntityAI>();
    }

    void ~CS_RecipeRow()
    {
        // Cleanup preview entities
        foreach (EntityAI ent : m_PreviewEntities)
        {
            if (ent) GetGame().ObjectDelete(ent);
        }
    }

    // Initialize recipe row widget
    Widget Init(Widget parent, CS_Recipe recipe, bool canCraft, int recipeIndex, PlayerBase player)
    {
        m_Recipe = recipe;
        m_RecipeIndex = recipeIndex;

        // Load layout
        m_Root = GetGame().GetWorkspace().CreateWidgets("CraftSystem/Scripts/GUI/Layouts/CS_RecipeRow.layout", parent);

        if (!m_Root) return null;

        // Get craft button
        m_ButtonCraft = ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonCraft"));

        // Setup result preview
        ItemPreviewWidget previewResult = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("PreviewResult"));
        SetupItemPreview(previewResult, recipe.ResultItem);

        // Setup ingredient previews (simplified)
        for (int i = 0; i < recipe.Ingredients.Count(); i++)
        {
            CS_Ingredient ing = recipe.Ingredients.Get(i);

            // Find widgets
            string widgetName = "PreviewItem" + (i + 1).ToString();
            ItemPreviewWidget preview = ItemPreviewWidget.Cast(m_Root.FindAnyWidget(widgetName));

            if (preview)
            {
                EntityAI item = SetupItemPreview(preview, ing.Classname);

                // Check availability
                int playerHas = CS_CraftLogic.CountItemInInventory(player, ing.Classname);
                int color = ARGB(50, 255, 0, 0); // Red

                if (playerHas >= ing.Quantity)
                {
                    color = ARGB(50, 0, 255, 0); // Green
                }

                // Set background color
                Widget panelName = m_Root.FindAnyWidget("PanelNameBG" + (i + 1).ToString());
                if (panelName) panelName.SetColor(color);
            }
        }

        // Enable/disable button
        if (m_ButtonCraft)
        {
            m_ButtonCraft.Enable(canCraft);

            if (canCraft)
            {
                m_ButtonCraft.SetColor(ARGB(255, 62, 174, 70)); // Green
            }
            else
            {
                m_ButtonCraft.SetColor(ARGB(255, 180, 40, 40)); // Red
            }
        }

        return m_Root;
    }

    // Setup item preview widget
    EntityAI SetupItemPreview(ItemPreviewWidget preview, string classname)
    {
        if (!preview || classname == "") return null;

        // Create temporary entity for preview
        EntityAI itemEntity = EntityAI.Cast(GetGame().CreateObject(classname, vector.Zero, true, false, true));

        if (itemEntity)
        {
            preview.SetItem(itemEntity);
            preview.SetView(itemEntity.GetViewIndex());
            preview.SetModelPosition(Vector(0, 0, 0.5));
            m_PreviewEntities.Insert(itemEntity);
        }

        return itemEntity;
    }

    // Getters
    CS_Recipe GetRecipe() { return m_Recipe; }
    int GetRecipeIndex() { return m_RecipeIndex; }
    ButtonWidget GetButton() { return m_ButtonCraft; }
    Widget GetRoot() { return m_Root; }

    // Cleanup
    void Cleanup()
    {
        foreach (EntityAI ent : m_PreviewEntities)
        {
            if (ent) GetGame().ObjectDelete(ent);
        }
        m_PreviewEntities.Clear();
    }
}

// Main GUI menu
class CS_HUD extends UIScriptedMenu
{
    private Widget m_Root;
    private ScrollWidget m_ScrollRecipes;
    private WrapSpacerWidget m_RecipeContainer;
    private ButtonWidget m_ButtonClose;

    private ref array<ref CS_RecipeRow> m_RecipeRows;
    private PlayerBase m_Player;

    void CS_HUD()
    {
        m_RecipeRows = new array<ref CS_RecipeRow>();
        m_Player = PlayerBase.Cast(GetGame().GetPlayer());
    }

    void ~CS_HUD()
    {
        // Cleanup recipe rows
        foreach (CS_RecipeRow row : m_RecipeRows)
        {
            if (row) row.Cleanup();
        }
    }

    // Initialize menu
    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("CraftSystem/Scripts/GUI/Layouts/CS_Main.layout");

        if (!layoutRoot) return null;

        // Get widgets
        m_ScrollRecipes = ScrollWidget.Cast(layoutRoot.FindAnyWidget("ScrollRecipes"));
        m_RecipeContainer = WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("RecipeContainer"));
        m_ButtonClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonClose"));

        // Load recipes
        LoadRecipes();

        return layoutRoot;
    }

    // Load all recipes
    void LoadRecipes()
    {
        ClearRecipes();

        if (!g_CS_Handler || !g_CS_Handler.GetSettings()) return;

        CS_Settings settings = g_CS_Handler.GetSettings();

        for (int i = 0; i < settings.Recipes.Count(); i++)
        {
            CS_Recipe recipe = settings.Recipes.Get(i);
            bool canCraft = CS_CraftLogic.CanCraft(m_Player, recipe);

            CS_RecipeRow row = new CS_RecipeRow();
            row.Init(m_RecipeContainer, recipe, canCraft, i, m_Player);
            m_RecipeRows.Insert(row);
        }
    }

    // Clear all recipe rows
    void ClearRecipes()
    {
        foreach (CS_RecipeRow row : m_RecipeRows)
        {
            if (row) row.Cleanup();
        }

        if (m_RecipeContainer)
        {
            while (m_RecipeContainer.GetChildren())
            {
                m_RecipeContainer.RemoveChild(m_RecipeContainer.GetChildren());
            }
        }

        m_RecipeRows.Clear();
    }

    // Refresh recipes (after crafting)
    void RefreshRecipes()
    {
        LoadRecipes();
    }

    // Click handler
    override bool OnClick(Widget w, int x, int y, int button)
    {
        super.OnClick(w, x, y, button);

        // Close button
        if (w == m_ButtonClose)
        {
            CloseMenu();
            return true;
        }

        // Craft buttons
        for (int i = 0; i < m_RecipeRows.Count(); i++)
        {
            CS_RecipeRow row = m_RecipeRows.Get(i);
            if (w == row.GetButton())
            {
                TryCraft(row.GetRecipeIndex());
                return true;
            }
        }

        return false;
    }

    // Attempt crafting
    void TryCraft(int recipeIndex)
    {
        if (!m_Player) return;
        if (!g_CS_Handler || !g_CS_Handler.GetSettings()) return;

        CS_Settings settings = g_CS_Handler.GetSettings();
        if (recipeIndex < 0 || recipeIndex >= settings.Recipes.Count()) return;

        CS_Recipe recipe = settings.Recipes.Get(recipeIndex);

        if (CS_CraftLogic.CanCraft(m_Player, recipe))
        {
            // Play sound
            EffectSound sound = SEffectManager.PlaySound(CS_Constants.SOUND_CRAFT, m_Player.GetPosition());
            if (sound) sound.SetSoundAutodestroy(true);

            // Request craft from server
            m_Player.RequestCraft(recipeIndex);

            // Refresh GUI after 500ms
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RefreshRecipes, 500, false);
        }
    }

    // Update handler (ESC to close)
    override void Update(float timeslice)
    {
        super.Update(timeslice);

        if (GetUApi().GetInputByName("UAUIBack").LocalPress())
        {
            CloseMenu();
        }
    }

    // Menu show
    override void OnShow()
    {
        super.OnShow();
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
        GetGame().GetUIManager().ShowCursor(true);
        GetGame().GetInput().ChangeGameFocus(1);
    }

    // Menu hide
    override void OnHide()
    {
        super.OnHide();
        GetGame().GetUIManager().ShowCursor(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetMission().PlayerControlEnable(true);
    }

    // Close menu
    void CloseMenu()
    {
        GetGame().GetUIManager().HideScriptedMenu(this);
    }
}
