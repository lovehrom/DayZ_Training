// Модификация PlayerBase для добавления кастомного действия
// Этот файл расширяет стандартный класс PlayerBase

modded class PlayerBase
{
    // Переопределяем SetActions для добавления своего действия
    override void SetActions(out TInputActionMap InputActionMap)
    {
        super.SetActions(InputActionMap);

        // Добавляем наше кастомное действие
        AddAction(ActionWaveHello);
    }

    // Опционально: Добавить обработку действия
    override void OnAction(int action_id, Man player, ContextData context)
    {
        super.OnAction(action_id, player, context);

        // Проверяем что это наше действие
        if (action_id == ActionWaveHello)
        {
            Print("Player performed Wave Hello action!");
            // Дополнительная логика при выполнении действия
        }
    }
}
