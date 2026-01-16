# Crafting GUI Systems (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Animation System](ANIMATION_SYSTEM.md) | [RPC System](RPC_NETWORKING.md)

---

## ⚡ 30-Second Summary

Продвинутые GUI системы крафтинга: визуальное отображение рецептов, умный подсчет предметов, сервер-клиент RPC.

---

## 🎯 5-Minute Solution: Simple Crafting GUI

### Recipe definition (XML)

```xml
<!-- inputs/inputs.xml -->
<recipes>
    <recipe name="Spear">
        <ingredient item="SharpStick" count="1" />
        <ingredient item="Rope" count="1" />
        <result item="Spear" count="1" />
    </recipe>
</recipes>
```

### Crafting logic

```c
class CraftingSystem
{
    protected ref map<string, ref Recipe> m_Recipes;

    bool CanCraft(string recipeName, PlayerBase player)
    {
        Recipe recipe = m_Recipes.Get(recipeName);

        foreach (Ingredient ingredient : recipe.Ingredients)
        {
            int count = player.CountItems(ingredient.ItemName);

            if (count < ingredient.Count)
            {
                return false;
            }
        }

        return true;
    }

    void Craft(string recipeName, PlayerBase player)
    {
        if (!CanCraft(recipeName, player))
            return;

        Recipe recipe = m_Recipes.Get(recipeName);

        // Remove ingredients
        foreach (Ingredient ingredient : recipe.Ingredients)
        {
            player.RemoveItems(ingredient.ItemName, ingredient.Count);
        }

        // Add result
        player.SpawnItem(recipe.Result.ItemName, recipe.Result.Count);
    }
}
```

---

## 📚 Deep Dive

## CraftSystem (3626433444) - Advanced GUI Crafting

### HUD rendering with recipe preview

```c
class CS_HUD : ScriptedWidgetEventHandler
{
    protected ref map<string, ref Recipe> m_Recipes;
    protected ref array<ref Widget> m_RecipeWidgets;

    override void OnUpdate(float delta_time)
    {
        // Update recipe availability indicators
        UpdateRecipeIndicators();
    }

    void UpdateRecipeIndicators()
    {
        foreach (Recipe recipe : m_Recipes)
        {
            Widget widget = GetRecipeWidget(recipe.Name);

            if (CanCraft(recipe.Name))
            {
                widget.SetColor(ARGB(255, 0, 255, 0)); // Green = available
            }
            else
            {
                widget.SetColor(ARGB(255, 255, 0, 0)); // Red = unavailable
            }
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3626433444\addons\CraftSystem\CraftSystem\Scripts\5_Mission\CS_HUD.c:79-337`

### Smart inventory management

```c
class CS_CraftLogic
{
    bool HasIngredients(PlayerBase player, Recipe recipe)
    {
        foreach (Ingredient ingredient : recipe.Ingredients)
        {
            int totalCount = 0;

            // Search in inventory
            array<EntityAI> items = new array<EntityAI>;
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

            foreach (EntityAI item : items)
            {
                if (item.GetType() == ingredient.ItemName)
                {
                    totalCount += item.GetQuantity();

                    if (totalCount >= ingredient.Count)
                    {
                        break;
                    }
                }
            }

            if (totalCount < ingredient.Count)
            {
                return false;
            }
        }

        return true;
    }

    bool ConsumeIngredients(PlayerBase player, Recipe recipe)
    {
        foreach (Ingredient ingredient : recipe.Ingredients)
        {
            int remaining = ingredient.Count;

            array<EntityAI> items = new array<EntityAI>;
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

            foreach (EntityAI item : items)
            {
                if (item.GetType() == ingredient.ItemName)
                {
                    int quantity = item.GetQuantity();

                    if (quantity <= remaining)
                    {
                        remaining -= quantity;
                        GetGame().ObjectDelete(item);
                    }
                    else
                    {
                        item.SetQuantity(quantity - remaining);
                        remaining = 0;
                    }

                    if (remaining == 0)
                    {
                        break;
                    }
                }
            }
        }

        return true;
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3626433444\addons\CraftSystem\CraftSystem\Scripts\4_World\CS_CraftLogic.c:24-80, 109-160`

### Server-client RPC communication

```c
class CS_MissionServer : MissionServer
{
    override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, target, rpc_type, ctx);

        switch (rpc_type)
        {
            case CS_RPCs.CRAFT_ITEM:
                Param1<string> recipeName = new Param1<string>("");
                if (ctx.Read(recipeName))
                {
                    PlayerBase player = PlayerBase.Cast(target);
                    CraftItem(player, recipeName.param1);
                }
                break;
        }
    }

    void CraftItem(PlayerBase player, string recipeName)
    {
        if (!GetGame().IsServer())
            return;

        Recipe recipe = m_Recipes.Get(recipeName);

        if (m_CraftLogic.HasIngredients(player, recipe))
        {
            m_CraftLogic.ConsumeIngredients(player, recipe);
            player.GetInventory().CreateInInventory(recipe.Result.ItemName);
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3626433444\addons\CraftSystem\CraftSystem\Scripts\5_Mission\CS_MissionServer.c:1-34`

### XML-based configuration system

```c
class CS_Config
{
    void LoadRecipes()
    {
        string filePath = "$profile:CraftSystem\\inputs.xml";

        if (!FileExist(filePath))
            return;

        FileHandle file = OpenFile(filePath, FileMode.READ);

        if (file != 0)
        {
            string line;
            while (FGets(file, line) > 0)
            {
                // Parse XML and create Recipe objects
                ParseRecipeLine(line);
            }

            CloseFile(file);
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3626433444\addons\CraftSystem\CraftSystem\Scripts\3_Game\CS_Config.c:1-39`

---

## Material pack Free (3620127965) - Custom Recipes

### Loot box mechanics

```c
class GiftBox extends ItemBase
{
    override void Open(ItemBase item)
    {
        super.Open(item);

        // Random reward system
        array<string> rewards = {
            "Apple",
            "CannedBeans",
            "Rifle",
            "Magazine_Random"
        };

        string reward = rewards.GetRandomElement();
        SpawnEntity(reward, GetPosition());

        // Delete box after opening
        GetGame().ObjectDelete(this);
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3620127965\addons\Kote_CraftMaterial\Kote_CraftMaterial\scripts\4_World\GiftBox.c:47-136`

---

## 💡 Examples

### Example 1: Simple recipe checker

```c
bool CanCraftSpear(PlayerBase player)
{
    return player.CountItems("SharpStick") >= 1 &&
           player.CountItems("Rope") >= 1;
}
```

### Example 2: Recursive ingredient counting

```c
int CountItemsRecursively(PlayerBase player, string typeName)
{
    int totalCount = 0;

    array<EntityAI> items = new array<EntityAI>;
    player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

    foreach (EntityAI item : items)
    {
        if (item.GetType() == typeName)
        {
            totalCount += item.GetQuantity();
        }

        // Check cargo items
        if (item.GetInventory())
        {
            totalCount += CountItemsInCargo(item, typeName);
        }
    }

    return totalCount;
}
```

---

## 🔗 Related Topics

→ [Animation System](ANIMATION_SYSTEM.md) - Item animations
→ [RPC System](RPC_NETWORKING.md) - Server-client communication
→ [Inventory System](INVENTORY_SYSTEM.md) - EnumerateInventory patterns

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ HUD rendering: CS_HUD.c:79-337
- ✅ Smart inventory: CS_CraftLogic.c:24-80, 109-160
- ✅ RPC communication: CS_MissionServer.c:1-34
- ✅ XML config: CS_Config.c:1-39

**Что НЕ проверено:**
- ❌ Работает ли без CraftSystem мода
- ❌ Производительность с большим количеством рецептов
- ❌ Поддержка nested ingredients (ingredients in ingredients)

---

## 💡 Tips

### Best Practices

1. **Используйте EnumerateInventory** для подсчета предметов
2. **Валидируйте на сервере** все крафт операции
3. **Используйте RPC** для коммуникации клиент-сервер
4. **Храните рецепты в XML/JSON** для лёгкого редактирования
5. **Обновляйте GUI indicators** каждую секунду (не каждый кадр)

### Performance Tips

- **Кэшируйте рецепты** при загрузке
- **Используйте map** для быстрого поиска по имени
- **Избегайте EnumerateInventory** в OnUpdate (каждый кадр)
- **Используйте таймер** для периодических обновлений

---

*Последнее обновление: 2025-01-16*
*Источники: CraftSystem (3626433444), Material pack Free (3620127965)*
