#ifndef OPENMW_SPELLAPI_HPP
#define OPENMW_SPELLAPI_HPP

#define SPELLAPI \
    {"ClearSpellbookChanges",          SpellFunctions::ClearSpellbookChanges},\
    {"ClearSpellsActiveChanges",       SpellFunctions::ClearSpellsActiveChanges},\
    \
    {"GetSpellbookChangesSize",        SpellFunctions::GetSpellbookChangesSize},\
    {"GetSpellbookChangesAction",      SpellFunctions::GetSpellbookChangesAction},\
    {"GetSpellsActiveChangesSize",     SpellFunctions::GetSpellsActiveChangesSize},\
    {"GetSpellsActiveChangesAction",   SpellFunctions::GetSpellsActiveChangesAction},\
    \
    {"SetSpellbookChangesAction",      SpellFunctions::SetSpellbookChangesAction},\
    {"SetSpellsActiveChangesAction",   SpellFunctions::SetSpellsActiveChangesAction},\
    \
    {"AddSpell",                       SpellFunctions::AddSpell},\
    {"AddSpellActive",                 SpellFunctions::AddSpellActive},\
    {"AddSpellActiveEffect",           SpellFunctions::AddSpellActiveEffect},\
    \
    {"GetSpellId",                     SpellFunctions::GetSpellId},\
    {"GetSpellsActiveId",              SpellFunctions::GetSpellsActiveId},\
    {"GetSpellsActiveDisplayName",     SpellFunctions::GetSpellsActiveDisplayName},\
    {"GetSpellsActiveEffectCount",     SpellFunctions::GetSpellsActiveEffectCount},\
    {"GetSpellsActiveEffectId",        SpellFunctions::GetSpellsActiveEffectId},\
    {"GetSpellsActiveEffectArg",       SpellFunctions::GetSpellsActiveEffectArg},\
    {"GetSpellsActiveEffectMagnitude", SpellFunctions::GetSpellsActiveEffectMagnitude},\
    {"GetSpellsActiveEffectDuration",  SpellFunctions::GetSpellsActiveEffectDuration},\
    {"GetSpellsActiveEffectTimeLeft",  SpellFunctions::GetSpellsActiveEffectTimeLeft},\
    \
    {"SendSpellbookChanges",           SpellFunctions::SendSpellbookChanges},\
    {"SendSpellsActiveChanges",        SpellFunctions::SendSpellsActiveChanges},\
    \
    {"InitializeSpellbookChanges",     SpellFunctions::InitializeSpellbookChanges}

class SpellFunctions
{
public:

    /**
    * \brief Clear the last recorded spellbook changes for a player.
    *
    * This is used to initialize the sending of new PlayerSpellbook packets.
    *
    * \param pid The player ID whose spellbook changes should be used.
    * \return void
    */
    static void ClearSpellbookChanges(unsigned short pid) noexcept;

    /**
    * \brief Clear the last recorded spells active changes for a player.
    *
    * This is used to initialize the sending of new PlayerSpellsActive packets.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \return void
    */
    static void ClearSpellsActiveChanges(unsigned short pid) noexcept;

    /**
    * \brief Get the number of indexes in a player's latest spellbook changes.
    *
    * \param pid The player ID whose spellbook changes should be used.
    * \return The number of indexes.
    */
    static unsigned int GetSpellbookChangesSize(unsigned short pid) noexcept;

    /**
    * \brief Get the action type used in a player's latest spellbook changes.
    *
    * \param pid The player ID whose spellbook changes should be used.
    * \return The action type (0 for SET, 1 for ADD, 2 for REMOVE).
    */
    static unsigned int GetSpellbookChangesAction(unsigned short pid) noexcept;

    /**
    * \brief Get the action type used in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \return The action type (0 for SET, 1 for ADD, 2 for REMOVE).
    */
    static unsigned int GetSpellsActiveChangesAction(unsigned short pid) noexcept;

    /**
    * \brief Get the number of indexes in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \return The number of indexes.
    */
    static unsigned int GetSpellsActiveChangesSize(unsigned short pid) noexcept;

    /**
    * \brief Set the action type in a player's spellbook changes.
    *
    * \param pid The player ID whose spellbook changes should be used.
    * \param action The action (0 for SET, 1 for ADD, 2 for REMOVE).
    * \return void
    */
    static void SetSpellbookChangesAction(unsigned short pid, unsigned char action) noexcept;

    /**
    * \brief Set the action type in a player's spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param action The action (0 for SET, 1 for ADD, 2 for REMOVE).
    * \return void
    */
    static void SetSpellsActiveChangesAction(unsigned short pid, unsigned char action) noexcept;

    /**
    * \brief Add a new spell to the spellbook changes for a player.
    *
    * \param pid The player ID whose spellbook changes should be used.
    * \param spellId The spellId of the spell.
    * \return void
    */
    static void AddSpell(unsigned short pid, const char* spellId) noexcept;

    /**
    * \brief Add a new active spell to the spells active changes for a player,
    *        using the temporary effect times stored so far.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param spellId The spellId of the spell.
    * \param displayName The displayName of the spell.
    * \return void
    */
    static void AddSpellActive(unsigned short pid, const char* spellId, const char* displayName) noexcept;

    /**
    * \brief Add a new effect to the next active spell that will be added to a player.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param effectId The id of the effect.
    * \param magnitude The magnitude of the effect.
    * \param duration The duration of the effect.
    * \param timeLeft The timeLeft for the effect.
    * \param arg The arg of the effect when applicable, e.g. the skill used for Fortify Skill or the attribute
    *            used for Fortify Attribute.
    * \return void
    */
    static void AddSpellActiveEffect(unsigned short pid, int effectId, double magnitude, double duration, double timeLeft, int arg) noexcept;

    /**
    * \brief Get the spell id at a certain index in a player's latest spellbook changes.
    *
    * \param pid The player ID whose spellbook changes should be used.
    * \param index The index of the spell.
    * \return The spell id.
    */
    static const char *GetSpellId(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the spell id at a certain index in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param index The index of the spell.
    * \return The spell id.
    */
    static const char* GetSpellsActiveId(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the spell display name at a certain index in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param index The index of the spell.
    * \return The spell display name.
    */
    static const char* GetSpellsActiveDisplayName(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the number of effects at an index in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param index The index of the spell.
    * \return The number of effects.
    */
    static unsigned int GetSpellsActiveEffectCount(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the id for an effect index at a spell index in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param spellIndex The index of the spell.
    * \param effectIndex The index of the effect.
    * \return The id of the effect.
    */
    static unsigned int GetSpellsActiveEffectId(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept;

    /**
    * \brief Get the arg for an effect index at a spell index in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param spellIndex The index of the spell.
    * \param effectIndex The index of the effect.
    * \return The arg of the effect.
    */
    static int GetSpellsActiveEffectArg(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept;

    /**
    * \brief Get the magnitude for an effect index at a spell index in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param spellIndex The index of the spell.
    * \param effectIndex The index of the effect.
    * \return The magnitude of the effect.
    */
    static double GetSpellsActiveEffectMagnitude(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept;

    /**
    * \brief Get the duration for an effect index at a spell index in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param spellIndex The index of the spell.
    * \param effectIndex The index of the effect.
    * \return The duration of the effect.
    */
    static double GetSpellsActiveEffectDuration(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept;

    /**
    * \brief Get the time left for an effect index at a spell index in a player's latest spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param spellIndex The index of the spell.
    * \param effectIndex The index of the effect.
    * \return The time left for the effect.
    */
    static double GetSpellsActiveEffectTimeLeft(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept;

    /**
    * \brief Send a PlayerSpellbook packet with a player's recorded spellbook changes.
    *
    * \param pid The player ID whose spellbook changes should be used.
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendSpellbookChanges(unsigned short pid, bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send a PlayerSpellsActive packet with a player's recorded spells active changes.
    *
    * \param pid The player ID whose spells active changes should be used.
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendSpellsActiveChanges(unsigned short pid, bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    // All methods below are deprecated versions of methods from above

    static void InitializeSpellbookChanges(unsigned short pid) noexcept;

private:

};

#endif //OPENMW_SPELLAPI_HPP
