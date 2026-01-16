class Shirt_Custom extends Clothing
{
    override void OnInit()
    {
        super.OnInit();
    }

    override void OnAttachment(EntityAI attachment, EntityAI parent, string slot_name)
    {
        super.OnAttachment(attachment, parent, slot_name);

        // Логика при присоединении attachment
        if (slot_name == "Armband")
        {
            // Например: добавить бонус к защите
        }
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionDyeColor);  // Окрасить
        AddAction(ActionRepair);     // Починить
    }
}
