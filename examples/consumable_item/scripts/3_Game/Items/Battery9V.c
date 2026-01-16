class Battery9V extends Battery
{
    override void OnInit()
    {
        super.OnInit();
        // Батарейка полностью заряжена при создании
        SetQuantity(1);
    }

    override void OnConsume(PlayerBase player)
    {
        super.OnConsume(player);

        // Логика при использовании батарейки
        // Например: зарядить фонарик
        ItemBase item = player.GetItemInHands();

        if (item && item.IsInherited(Flashlight))
        {
            // Зарядить фонарик
            item.SetHealth("", "", 100);
        }
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionAttach);
        AddAction(ActionDetach);
    }
}