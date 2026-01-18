// ============================================================
// ACTION OPEN ATM - Open bank menu at ATM object
// ============================================================
// This action allows players to interact with ATM objects
// Shows "Open Bank Account" action when looking at ATM

class ActionOpenATM: ActionInteractBase
{
    // ============================================================
    // CONSTRUCTOR - Initialize action properties
    // ============================================================
    void ActionOpenATM()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_HUDCursorIcon = CursorIcons.Loot;
    }

    // ============================================================
    // CREATE CONDITION COMPONENTS - Define action conditions
    // ============================================================
    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTObject(10);  // 10-meter range
        m_ConditionItem = new CCINone;          // No item required in hands
    }

    // ============================================================
    // GET TEXT - Display name of action
    // ============================================================
    override string GetText()
    {
        return "Open Bank Account";
    }

    // ============================================================
    // ACTION CONDITION - Check if action should be shown
    // ============================================================
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        // Only execute on server
        if (!GetGame().IsServer())
        {
            return false;
        }

        // Check if target is a building/object
        BuildingBase building = BuildingBase.Cast(target.GetObject());

        if (!building)
        {
            return false;  // Not a building
        }

        // Check if building is an ATM (TraderID = -2)
        if (building.TraderID != -2)
        {
            return false;  // Not an ATM
        }

        // Check if credit card is required
        if (GetBankingConfig().RequireCreditCard)
        {
            // Player must hold credit card in hands
            ItemBase itemInHands = player.GetHumanInventory().GetEntityInHands();

            if (!itemInHands)
            {
                // Show hint: need credit card
                return false;
            }

            if (!itemInHands.IsKindOf("CreditCard"))
            {
                // Player is holding something, but it's not a credit card
                return false;
            }
        }

        // All conditions met - show action
        return true;
    }

    // ============================================================
    // ON START CLIENT - Execute action on client
    // ============================================================
    override void OnStartClient(ActionData action_data)
    {
        super.OnStartClient(action_data);

        // Request bank data from server
        PlayerBase player = action_data.m_Player;
        GetRPCManager().SendRPC("ATM_System", "RequestBankData", null, true, player.GetIdentity());
    }

    // ============================================================
    // ON START SERVER - Execute action on server
    // ============================================================
    override void OnStartServer(ActionData action_data)
    {
        super.OnStartServer(action_data);

        PlayerBase player = action_data.m_Player;

        if (GetBankingConfig().BankingLogs)
        {
            Print("[ATM System] Player " + player.GetIdentity().GetName() + " opened bank menu");
        }
    }
}
