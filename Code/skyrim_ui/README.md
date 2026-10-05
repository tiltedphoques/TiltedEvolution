# Skyrim Together UI

Check the wiki for information, such as requirements and how to use/build it.

https://wiki.tiltedphoques.com/tilted-online/technical-documentation/build-guide#building-the-together-ui

## Game themes

One UI serves every game. Components only use the names from
`src/styles/theme/_base.scss` (colors, fonts, `image(...)` and the
`window-frame`/`notification-frame` mixins). Each game has a folder in
`src/themes/<game>/`:

- `_theme.scss` imports the base contract and overrides what differs.
- `_skin.scss` adds global styling on top of the shared styles.
- `assets/` holds the game's images, fonts and overlay cursor.

The build picks the folder through the style include paths: `npm start` and
`deploy:production` use Skyrim, `npm run start:fallout` and `deploy:fallout`
use Fallout 4.
