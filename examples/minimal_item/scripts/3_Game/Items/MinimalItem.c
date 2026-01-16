class MinimalItem extends ItemBase
{
    override void OnInit()
    {
        super.OnInit();
        // Код при инициализации предмета
        Print("MinimalItem initialized!");
    }

    override void OnConsume(PlayerBase player)
    {
        super.OnConsume(player);
        // Логика при потреблении предмета
        Print("MinimalItem consumed by " + player);
    }
}
