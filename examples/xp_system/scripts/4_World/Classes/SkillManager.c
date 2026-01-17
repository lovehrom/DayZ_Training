class SkillManager
{
    protected ref map<string, ref SimpleSkill> m_Skills = new map<string, ref SimpleSkill>;
    protected PlayerBase m_Player;

    void SkillManager(PlayerBase player)
    {
        m_Player = player;

        // Инициализируем навыки
        m_Skills.Insert("Survival", new SimpleSkill("Survival"));
        m_Skills.Insert("Crafting", new SimpleSkill("Crafting"));
        m_Skills.Insert("Combat", new SimpleSkill("Combat"));
    }

    void AddXP(string skillName, int amount)
    {
        SimpleSkill skill = m_Skills.Get(skillName);
        if (skill)
        {
            skill.AddXP(amount);

            // Уведомляем игрока
            NotifyPlayer("+" + amount + " XP (" + skillName + ")");

            // Сохраняем
            SaveSkills();
        }
    }

    void NotifyPlayer(string message)
    {
        if (!m_Player)
            return;

        // Простое уведомление в чат
        NotificationSystem.Notify(m_Player, message);
    }

    void SaveSkills()
    {
        if (!m_Player || !m_Player.GetIdentity())
            return;

        string steam64 = m_Player.GetIdentity().GetId();
        string filePath = "$profile:skills_" + steam64 + ".json";

        JsonFileLoader<map<string, ref SimpleSkill>>::JsonSaveFile(filePath, m_Skills);

        Print("Saved skills for " + steam64);
    }

    void LoadSkills()
    {
        if (!m_Player || !m_Player.GetIdentity())
            return;

        string steam64 = m_Player.GetIdentity().GetId();
        string filePath = "$profile:skills_" + steam64 + ".json";

        if (FileExist(filePath))
        {
            JsonFileLoader<map<string, ref SimpleSkill>>::JsonLoadFile(filePath, m_Skills);
            Print("Loaded skills for " + steam64);
        }
        else
        {
            Print("No skills file found for " + steam64);
        }
    }

    SimpleSkill GetSkill(string skillName)
    {
        return m_Skills.Get(skillName);
    }

    int GetLevel(string skillName)
    {
        SimpleSkill skill = m_Skills.Get(skillName);
        if (skill)
        {
            return skill.Level;
        }
        return 1;
    }

    // XP события
    void OnZombieKilled()
    {
        AddXP("Combat", 10); // 10 XP за зомби
    }

    void OnBookRead()
    {
        AddXP("Survival", 1500); // 1500 XP за книгу
    }

    void OnItemCrafted()
    {
        AddXP("Crafting", 500); // 500 XP за крафтинг
    }
}

// Helper class для notifications
class NotificationSystem
{
    static void Notify(PlayerBase player, string message)
    {
        if (!player)
            return;

        // Отправляем сообщение в чат
        Param1<string> msg = new Param1<string>(message);
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, player.GetIdentity());
    }
}
