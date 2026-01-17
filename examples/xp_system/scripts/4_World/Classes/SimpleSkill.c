class SimpleSkill
{
    string Name;
    int CurrentXP;
    int Level;

    void SimpleSkill(string name)
    {
        Name = name;
        CurrentXP = 0;
        Level = 1;
    }

    void AddXP(int amount)
    {
        CurrentXP += amount;
        CheckLevelUp();
    }

    void CheckLevelUp()
    {
        int requiredXP = Level * 1000; // 1000 XP per level

        if (CurrentXP >= requiredXP)
        {
            CurrentXP -= requiredXP;
            Level++;
            OnLevelUp();
        }
    }

    void OnLevelUp()
    {
        Print("Skill " + Name + " leveled up to " + Level);

        // Уведомляем всех игроков (опционально)
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man player : players)
        {
            PlayerBase pb = PlayerBase.Cast(player);
            if (pb)
            {
                NotificationSystem.Notify(pb, Name + " leveled up to " + Level + "!");
            }
        }
    }

    float GetLevelBonus()
    {
        // +10% бонуса за уровень
        return 1.0 + (Level * 0.1);
    }
}
