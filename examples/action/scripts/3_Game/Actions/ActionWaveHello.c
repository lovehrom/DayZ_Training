class ActionWaveHello: ActionSingleBase
{
    void ActionWaveHello()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACT;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone;
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        return player.IsAlive() && !player.IsRestrained();
    }

    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;
        // Логика на сервере
    }

    override void OnExecuteClient(ActionData action_data)
    {
        // Визуальные эффекты на клиенте
        // Например: анимация помахивания
    }
}
