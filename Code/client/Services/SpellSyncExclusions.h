#pragma once

#include <cstdint>

/**
 * @brief Plugins whose spells and magic effects are never synchronised between players.
 *
 * Some plugins use a spell purely as a local control surface — a configuration menu bound to a
 * lesser power, a hotkey wheel, a grab gesture. Replaying one of those on another player runs it
 * against THEIR game, which at best does nothing and at worst opens a menu they did not ask for or
 * changes their character state. Filtering by originating plugin rather than by spell type is
 * deliberate: powers and shouts are replayed on purpose, so a type-based rule would break vanilla
 * play.
 *
 * The list is read from Data/SkyrimTogetherReborn/config/SpellSyncExclusions.ini, which is created
 * with defaults on first run so a user can edit, extend or empty it without a rebuild.
 */
namespace SpellSyncExclusions
{
/**
 * @brief True when the form originates in a plugin on the exclusion list.
 *
 * Runtime-created forms (0xFF) and the null form have no originating plugin and are never excluded.
 */
bool IsExcluded(uint32_t aFormId) noexcept;
} // namespace SpellSyncExclusions
