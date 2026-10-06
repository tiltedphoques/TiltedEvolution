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
