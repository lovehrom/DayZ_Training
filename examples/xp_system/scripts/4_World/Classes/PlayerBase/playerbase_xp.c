modded class PlayerBase
{
    protected SkillManager m_SkillManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Создаем менеджер навыков
        m_SkillManager = new SkillManager(this);
        m_SkillManager.LoadSkills();

        Print("Player " + GetIdentity().GetName() + " connected. XP system initialized.");
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        if (m_SkillManager)
        {
            m_SkillManager.SaveSkills();
            Print("Player " + GetIdentity().GetName() + " disconnected. Skills saved.");
        }
    }

    // XP за убийство зомби
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Даем XP убийце
        if (killer && killer.IsMan())
        {
            Man man = killer.CastToMan();
            if (man && man.IsPlayer())
            {
                PlayerBase player = PlayerBase.Cast(man);
                if (player && player.m_SkillManager)
                {
                    player.m_SkillManager.OnZombieKilled();
                }
            }
        }
    }

    // Доступ к менеджеру навыков
    SkillManager GetSkillManager()
    {
        return m_SkillManager;
    }

    // XP за чтение книги
    override void ReadItem(ItemBase book)
    {
        super.ReadItem(book);

        if (m_SkillManager)
        {
            m_SkillManager.OnBookRead();
        }
    }

    // XP за крафтинг
    override void CraftItem(ItemBase item)
    {
        super.CraftItem(item);

        if (m_SkillManager)
        {
            m_SkillManager.OnItemCrafted();
        }
    }
}
