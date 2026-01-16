// Пример создания крафта камней через ActionCombination

modded class ActionCraftStone: ActionContinuousBase
{
    void ActionCraftStone()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CRAFTING;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
        m_Sound = "craft_universal_0";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        // Проверка: у игрока есть 2 маленьких камня
        ItemBase item1 = player.FindAttachmentBySlotName("Armband");
        if (!item1 || item1.GetType() != "SmallStone")
            return false;

        // Проверка количества ингредиентов
        int stoneCount = player.CountItemType("SmallStone");
        if (stoneCount < 2)
            return false;

        return player.IsAlive();
    }

    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;

        // Удалить ингредиенты
        player.RemoveItemOfType("SmallStone", 2);

        // Создать результат
        player.GetInventory().CreateInInventory("Stone");

        // Сообщение игроку
        player.MessageAction("Вы создали камень!");
    }

    override void OnExecuteClient(ActionData action_data)
    {
        // Визуальные эффекты на клиенте
    }
};
