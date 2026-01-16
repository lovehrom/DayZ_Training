class ApplePie extends Edible_Base
{
    override void OnInit()
    {
        super.OnInit();
        // Код при инициализации
    }

    override void OnConsume(PlayerBase player)
    {
        super.OnConsume(player);

        // Дополнительная логика при потреблении
        // Например: восстановить немного здоровья
        player.AddHealth("", "GlobalHealth", 5);
    }

    override void SetActions()
    {
        super.SetActions();
        // Добавить действия в контекстное меню
        AddAction(ActionEat);
        AddAction(ActionForceFeed);
    }
}
