# Crafted potion trade playtest

Build both clients and the server from the same commit on `codex/player-trading-1.6.1170`. Inventory packets and the Ready request now carry potion recipes; the previous server build is incompatible.

1. Craft a single-effect potion and a potion with multiple effects. Record each effect's magnitude and duration and the stack counts.
2. Give the recipient different Alchemy skills/perks and record their ingredients and Alchemy experience.
3. Offer part of each stack. Verify the offer lists show the actual potion name, the owner's temporary ID (FFxxxxxx) and each effect's magnitude and duration, and complete the trade.
4. Check that exactly the offered quantity left the sender and arrived at the recipient. Effects must match; ingredients and Alchemy experience must stay unchanged.
5. Drink one received potion, trade another back, and verify subsequent stack counts. Save/reload and verify the remaining potion still works.
6. Repeat with a crafted poison, with both players offering potions, and with an existing matching potion stack on the recipient.
7. Cancel Ready, change quantities, cancel the trade and disconnect during the countdown. Preparation must not add potions to inventory or consume anything.
8. Trade a normal cooked meal and a normal purchased potion to verify the existing static-form path.

Recreation uses Skyrim's `BGSCreatedObjectManager::AddPotion` (or `AddPoison` for poisons) with the captured effects directly, in the sender's effect order. It does not invoke the crafting menu or calculate strength from the recipient's skills. The Ready request confirms successful reconstruction; changing an offer invalidates readiness. Unsupported temporary effects and effect conditions are excluded from trading. Custom per-instance names remain local inventory UI metadata.

Native engine behavior, including the AddPotion address and created-object reference lifetime, still requires this in-game test. Passing compilation and packet tests does not establish in-game correctness.

## Items the recipient cannot load

Each client shows only the partner items it can rebuild: every plugin form the item references must be loaded locally, and crafted (temporary) potions and enchantments must have effects that resolve locally. Weapons coated with a crafted poison carry the poison's recipe and are receivable when its effects resolve; the recipient's coating is recreated with `AddPoison`, keeping the remaining uses. When a player readies, the Ready request lists exactly the partner items that client accepts; the server transfers only those, and everything else stays with its owner. Once the partner is ready, the sender's own offer marks any item the partner cannot receive.

9. A offers a crafted potion with an effect from a plugin B lacks, plus a crafted poison with vanilla effects. B offers a weapon with a crafted vanilla enchantment, plus armor from a plugin A lacks. Verify A sees only the weapon, B sees only the poison, each sender's offer marks the hidden item after the partner readies, and after the countdown only the weapon and poison change hands.
10. Coat a weapon with a crafted vanilla-effect poison, trade it, and verify the recipient's weapon shows the same poison effects and remaining uses, then hits apply the poison. Repeat with a poison using an effect from a plugin the recipient lacks: the weapon must stay hidden and remain with the sender.

## Live inventory snapshot

Enchanting, renaming and brewing can change items without the incremental inventory events the server relies on. When a trade starts and each time a player readies, their client sends its live inventory and the server replaces its copy for the trade. Offered items the snapshot no longer covers are dropped from the offer and both players are unreadied. Readying an outdated offer, or one built on an outdated partner offer, unreadies instead of cancelling.

11. Enchant and rename an item ("AAA ...") and brew a potion while connected, then start a trade without relogging. Both must appear in the picker, the renamed item must be found by searching "AAA", and readying with both offered must not cancel the trade.
