// Пример создания крафта верёвки через комбинацию предметов

modded class ActionCraftRopeCustom: ActionCombinationBase
{
    void ActionCraftRopeCustom()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_COMBINE;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
        m_Text = "#combine";  // Текст действия (combine = объединить)
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCINonRuined;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        // Get предметы из рук
        ItemBase itemInHands = player.GetItemInHands();

        if (!itemInHands)
            return false;

        // Проверка: в руках лоскут ткани (Rag)
        if (itemInHands.GetType() != "Rag")
            return false;

        // Проверка: цель также лоскут ткани
        Object targetObject = target.GetObject();
        if (!targetObject)
            return false;

        ItemBase targetItem = ItemBase.Cast(targetObject);
        if (!targetItem || targetItem.GetType() != "Rag")
            return false;

        return true;
    }

    override bool CanCombine(ItemBase item1, ItemBase item2)
    {
        // Проверка: оба предмета - лоскуты ткани
        if (!item1 || !item2)
            return false;

        return item1.GetType() == "Rag" && item2.GetType() == "Rag";
    }

    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;

        // Удалить ингредиенты
        ItemBase item1 = action_data.m_MainItem;
        ItemBase item2 = action_data.m_TargetItem;

        if (item1)
            item1.Delete();

        if (item2)
            item2.Delete();

        // Создать результат
        player.GetInventory().CreateInInventory("Rope_Custom");

        // Сообщение игроку
        player.MessageAction("Вы создали верёвку!");
    }

    override void OnExecuteClient(ActionData action_data)
    {
        // Визуальные эффекты на клиенте
    }
};
