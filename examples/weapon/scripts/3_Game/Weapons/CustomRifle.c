class CustomRifle extends Rifle_Base
{
    override void OnInit()
    {
        super.OnInit();
        Print("CustomRifle initialized!");
    }

    // При выстреле
    override void EEFired(int muzzleType, int mode, string ammo)
    {
        super.EEFired(muzzleType, mode, ammo);
        Print("CustomRifle fired!");
    }

    // При перезарядке
    override void SetActions()
    {
        super.SetActions();
        // Actions добавляются автоматически из базового класса
    }

    // Проверка может ли оружие использовать патроны
    override bool CanEnterIronsights()
    {
        return true;
    }
}
