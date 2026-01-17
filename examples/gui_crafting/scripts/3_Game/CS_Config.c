// CS_Config.c
// Recipe data classes (CS_Ingredient, CS_Recipe, CS_Settings)

// Single ingredient (e.g., 20 nails)
class CS_Ingredient
{
    string Classname;  // "Nail"
    int Quantity;      // 20

    void CS_Ingredient()
    {
        Classname = "";
        Quantity = 1;
    }
}

// Complete recipe (ingredients + result)
class CS_Recipe
{
    string DisplayName;                    // "AK-47"
    string ResultItem;                     // "AK74"
    int ResultQuantity;                    // 1
    ref array<ref CS_Ingredient> Ingredients; // Array of ingredients

    void CS_Recipe()
    {
        DisplayName = "";
        ResultItem = "";
        ResultQuantity = 1;
        Ingredients = new array<ref CS_Ingredient>();
    }
}

// Settings container (all recipes)
class CS_Settings
{
    ref array<ref CS_Recipe> Recipes;

    void CS_Settings()
    {
        Recipes = new array<ref CS_Recipe>();
    }
}
