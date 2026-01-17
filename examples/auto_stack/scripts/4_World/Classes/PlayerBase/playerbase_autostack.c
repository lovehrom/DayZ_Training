modded class PlayerBase
{
    protected float m_LastStackTime = 0;
    protected const float STACK_COOLDOWN = 5.0; // 5 секунд между авто-стеканием

    override void OnInventoryEnter(EntityAI item)
    {
        super.OnInventoryEnter(item);

        // Проверяем cooldown
        float currentTime = GetGame().GetTickTime();

        if (currentTime - m_LastStackTime >= STACK_COOLDOWN)
        {
            // Автоматически объединяем предметы
            AutoStackItems();

            // Обновляем cooldown
            m_LastStackTime = currentTime;
        }
    }

    void AutoStackItems()
    {
        // Получаем все предметы в инвентаре
        array<EntityAI> items = new array<EntityAI>;
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

        int stackedCount = 0;

        // Проходим по всем предметам
        for (int i = 0; i < items.Count(); i++)
        {
            ItemBase item1 = ItemBase.Cast(items[i]);

            if (!item1 || !ShouldStack(item1))
                continue;

            // Ищем дубликаты
            for (int j = i + 1; j < items.Count(); j++)
            {
                ItemBase item2 = ItemBase.Cast(items[j]);

                if (!item2 || !ShouldStack(item2))
                    continue;

                // Проверяем что это одинаковые предметы
                if (CanStack(item1, item2))
                {
                    // Объединяем
                    if (StackItems(item1, item2))
                    {
                        stackedCount++;
                        break; // item1 изменен, выходим из внутреннего цикла
                    }
                }
            }
        }

        if (stackedCount > 0)
        {
            Print("Auto-stacked " + stackedCount + " items for player " + GetIdentity().GetName());
        }
    }

    bool ShouldStack(ItemBase item)
    {
        // Проверяем что предмет можно стекать
        if (!item)
            return false;

        // Исключаем magazines (они управляются иначе)
        if (item.IsInherited(Magazine))
            return false;

        // Исключаем оружие
        if (item.IsInherited(Weapon))
            return false;

        // Исключаем контейнеры
        if (item.GetInventory().GetCargo())
            return false;

        // Проверяем что предмет имеет quantity
        if (item.GetQuantity() <= 0)
            return false;

        // Проверяем что предмет не полный
        if (item.GetQuantity() >= item.GetQuantityMax())
            return false;

        return true;
    }

    bool CanStack(ItemBase item1, ItemBase item2)
    {
        // Проверяем что это одинаковые типы
        if (item1.GetType() != item2.GetType())
            return false;

        // Проверяем health (оба должны быть pristine или worn)
        float health1 = item1.GetHealth();
        float health2 = item2.GetHealth();
        float maxHealth = item1.GetMaxHealth();

        // Сравниваем health level
        int healthLevel1 = GetHealthLevel(health1, maxHealth);
        int healthLevel2 = GetHealthLevel(health2, maxHealth);

        if (healthLevel1 != healthLevel2)
            return false;

        // Проверяем wetness (оба должны быть сухими или мокрыми)
        float wetness1 = item1.GetWet();
        float wetness2 = item2.GetWet();

        if (Math.AbsFloat(wetness1 - wetness2) > 0.1)
            return false;

        return true;
    }

    int GetHealthLevel(float health, float maxHealth)
    {
        float healthPercent = health / maxHealth;

        if (healthPercent > 0.9)
            return 4; // Pristine
        else if (healthPercent > 0.6)
            return 3; // Worn
        else if (healthPercent > 0.3)
            return 2; // Damaged
        else
            return 1; // Badly Damaged
    }

    bool StackItems(ItemBase target, ItemBase source)
    {
        if (!target || !source)
            return false;

        float targetQuantity = target.GetQuantity();
        float sourceQuantity = source.GetQuantity();
        float maxQuantity = target.GetQuantityMax();

        // Проверяем что есть место
        if (targetQuantity >= maxQuantity)
            return false;

        // Вычисляем сколько можно добавить
        float spaceAvailable = maxQuantity - targetQuantity;
        float toTransfer = Math.Min(spaceAvailable, sourceQuantity);

        if (toTransfer <= 0)
            return false;

        // Добавляем quantity
        target.AddQuantity(toTransfer);

        // Уменьшаем source quantity
        source.AddQuantity(-toTransfer);

        // Если source пустой, удаляем
        if (source.GetQuantity() <= 0)
        {
            GetGame().ObjectDelete(source);
        }

        return true;
    }

    // Метод для ручного вызова (опционально)
    void ForceAutoStack()
    {
        AutoStackItems();
        m_LastStackTime = GetGame().GetTickTime();
    }
}
