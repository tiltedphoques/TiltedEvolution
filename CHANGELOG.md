# [1.15.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.14.0...v1.15.0) (2022-02-12)


### Features

* address library for RTTI as well ([35fb2ba](https://github.com/tiltedphoques/TiltedEvolution/commit/35fb2ba313deb34710a5499a9eb0b7fe9b04a6d0))
* Use address library instead of hardcoded addresses ([6b2afeb](https://github.com/tiltedphoques/TiltedEvolution/commit/6b2afebce3cf1545137c84adfd75ae4840828625))
* weird pointers now use address library as well ([543a379](https://github.com/tiltedphoques/TiltedEvolution/commit/543a3793b11294e11db3082b6e6fca5744e84b4c))

# [1.14.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.13.0...v1.14.0) (2022-02-12)


### Bug Fixes

* address submodule clusterfuck ([2fafe5d](https://github.com/tiltedphoques/TiltedEvolution/commit/2fafe5d7c7b2e88dfe0e2ad1d46da12f1fecab35))


### Features

* Always exit the server gracefully on windows. ([a3394bd](https://github.com/tiltedphoques/TiltedEvolution/commit/a3394bd26ecc71269e74d055f73a6d337f590985))

# [1.13.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.12.0...v1.13.0) (2022-02-09)


### Bug Fixes

* build error ([7c8c017](https://github.com/tiltedphoques/TiltedEvolution/commit/7c8c017096a5f08bcd6beabeffc5143be5329d5e))
* build errors ([92f8dbf](https://github.com/tiltedphoques/TiltedEvolution/commit/92f8dbf578f3d39ed729e224b1d1774cf987a89c))
* crash when allocating too much data in a single packet ([d672708](https://github.com/tiltedphoques/TiltedEvolution/commit/d6727085c1932306d02c87a31dd5ec5a2c6a7c8e))
* linux build ([6d340a0](https://github.com/tiltedphoques/TiltedEvolution/commit/6d340a08c75215ca367133e768ac4cf3cd663b27))
* linux build should not include imgui ([6359122](https://github.com/tiltedphoques/TiltedEvolution/commit/63591224f0531386cc46ebb8a6e5b91430194e9d))
* linux build vs lower case ([0e8b386](https://github.com/tiltedphoques/TiltedEvolution/commit/0e8b386fdc10941306f7ac217f5a9da447276c27))
* linux now builds ! ([46f85be](https://github.com/tiltedphoques/TiltedEvolution/commit/46f85be9399926f22947f845e5982281dd20c4f6))
* Project builds, proper namespace naming conventions and some warning fixes ([5ccf173](https://github.com/tiltedphoques/TiltedEvolution/commit/5ccf1733bfc102628374b2492d84c3ba097d8b66))
* Tests now build correctly on first build ([23801f5](https://github.com/tiltedphoques/TiltedEvolution/commit/23801f50b96c79839721c1908b699366be86a43a))


### Features

* Build a reference map of navmesh chunks for each worldspace/cell ([b16ec88](https://github.com/tiltedphoques/TiltedEvolution/commit/b16ec88e1a51e732724c50c36048ba721af96fa6))
* Docker image can now be built ([3ef8308](https://github.com/tiltedphoques/TiltedEvolution/commit/3ef8308529c4840b78c0342b6c82ea85bf89ad91))
* docker-compose ([8d4b802](https://github.com/tiltedphoques/TiltedEvolution/commit/8d4b8026d881b8bf507eadf93c729e7b0ce53e27))
* docker-compose now forward port correctly ([c85d138](https://github.com/tiltedphoques/TiltedEvolution/commit/c85d13828baa19b7632b57b063da69976415574d))
* load world data on the server ([e6d8d28](https://github.com/tiltedphoques/TiltedEvolution/commit/e6d8d28d7e6c5c62e42ef80a83dd98a3ef3192ec))
* Parse navmesh and geometry ([80c70b5](https://github.com/tiltedphoques/TiltedEvolution/commit/80c70b50d5562cee3919991e5750083035cccaef))

# [1.12.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.11.0...v1.12.0) (2022-01-24)


### Bug Fixes

* Attempt to make linux build. ([ed9fa0a](https://github.com/tiltedphoques/TiltedEvolution/commit/ed9fa0ae41117f709d1d18068f124b8bafa6eb2d))
* Block SKSE Plugin Preloader for compatabiliy reasons. ([0d5d0e4](https://github.com/tiltedphoques/TiltedEvolution/commit/0d5d0e4cef885fd8946bcb3619c3d8bf40999961))
* change some naming. ([fb876c0](https://github.com/tiltedphoques/TiltedEvolution/commit/fb876c033f6ad911e74855953cc0e76636c38d2c))
* Imgui end scope. ([5493a52](https://github.com/tiltedphoques/TiltedEvolution/commit/5493a52b5c98f05eeb2f79e8afe91972f4fb50a1))
* Repair my monke code ([f9491fe](https://github.com/tiltedphoques/TiltedEvolution/commit/f9491fe4a0d48824c17586fecd95e9e753adc65d))


### Features

* Deny load for unsupported game versions. ([0a1e7cb](https://github.com/tiltedphoques/TiltedEvolution/commit/0a1e7cb12042a83453cccebec3c56e877800b196))
* rip 'allocator' ([2695f2f](https://github.com/tiltedphoques/TiltedEvolution/commit/2695f2fe81402139095af7f94058d9e4d650fc9f))
* Selectable entity for component view debug, hudmenuutils. ([5b2e063](https://github.com/tiltedphoques/TiltedEvolution/commit/5b2e06399dcacfec70b1f84ae176d3ff51a49b0e))
* Smarter input handling for skyrim. ([68e188b](https://github.com/tiltedphoques/TiltedEvolution/commit/68e188bae36838c4f2628fe292366fdc212ec142))
* world to screen with hud matrix for skyrim. Cxx20 updates for client. ([4825286](https://github.com/tiltedphoques/TiltedEvolution/commit/4825286c8227ad82e903080bfa03ab768eef0c2c))

# [1.11.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.10.0...v1.11.0) (2022-01-23)


### Bug Fixes

* merge conflict ([c2b082f](https://github.com/tiltedphoques/TiltedEvolution/commit/c2b082f40e4a145226299af06a1dfc842884e36b))
* merge conflicts ([7f299e9](https://github.com/tiltedphoques/TiltedEvolution/commit/7f299e9c6655f6c9c0b9f6c6adab181f09b2884d))


### Features

* AI sync for Fallout 4 ([d86dfda](https://github.com/tiltedphoques/TiltedEvolution/commit/d86dfdaedd4d14c26e360223758118b637e97dd7))
* first step in syncing AI packages ([de84956](https://github.com/tiltedphoques/TiltedEvolution/commit/de84956e489a9f145afd7957cdaa63a4e7a06aa8))
* more ugly debuggers (force dont look) ([77b85e9](https://github.com/tiltedphoques/TiltedEvolution/commit/77b85e914664d4cd43fee63504df793baf54091a))
* reverse engineered TESAIForm ([59cf6a2](https://github.com/tiltedphoques/TiltedEvolution/commit/59cf6a2a56bbde2a2e273b28b65bacbd42992ab4))
* sync TESPackage by actor ownership ([a2aa136](https://github.com/tiltedphoques/TiltedEvolution/commit/a2aa1362985440854d5e17d676bcfe97d413e90b))

# [1.10.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.9.0...v1.10.0) (2022-01-21)


### Bug Fixes

* build error ([e7c1f9a](https://github.com/tiltedphoques/TiltedEvolution/commit/e7c1f9a9ab83e994ed848d9a122dd11922521915))
* build errors ([4826b8b](https://github.com/tiltedphoques/TiltedEvolution/commit/4826b8b4db1f8170e789f60e8b385092632c6d39))
* moved opcode check ([f168015](https://github.com/tiltedphoques/TiltedEvolution/commit/f168015c01b36dacec8d9e4a014ff37bbfc967e7))
* variable naming convention ([47f3e70](https://github.com/tiltedphoques/TiltedEvolution/commit/47f3e7093bd31a09e2d84742767e369e9e34f57a))


### Features

* remote horses are now synced ([a8a267a](https://github.com/tiltedphoques/TiltedEvolution/commit/a8a267a9229e8a239bd6d6411590b2171d1d0871))
* take control of remote horse ([400c12e](https://github.com/tiltedphoques/TiltedEvolution/commit/400c12e0a6065e021a7a2ace7744c2e283347aba))
* trigger mounting ([6fe7d4a](https://github.com/tiltedphoques/TiltedEvolution/commit/6fe7d4a04f0f898d303e43d1b9c37714c1535a57))

# [1.9.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.8.0...v1.9.0) (2022-01-21)


### Bug Fixes

* compression bug in es parser ([426a251](https://github.com/tiltedphoques/TiltedEvolution/commit/426a25147c65b3436aee49806d1f624e1605540d))
* script property parsing ([3018ad7](https://github.com/tiltedphoques/TiltedEvolution/commit/3018ad761eb098573113e0b245d590e9740f23a0))
* static var rename ([6919096](https://github.com/tiltedphoques/TiltedEvolution/commit/69190962da08c63ed2222cbfb7bd144ec4638d94))


### Features

* another attempt at full inventory control ([4f8fbe0](https://github.com/tiltedphoques/TiltedEvolution/commit/4f8fbe0cf9b0386f75f3002cdd2aac88f01b8f6a))
* base id assignment for parsed records ([dee225a](https://github.com/tiltedphoques/TiltedEvolution/commit/dee225a22f4700420e9c0e595201a2c97f73eced))
* chunk sorting and parsing on REFR records ([8c196f5](https://github.com/tiltedphoques/TiltedEvolution/commit/8c196f523d71242a95266c52375c89d47e70cdef))
* copy parse chunks ([47e0441](https://github.com/tiltedphoques/TiltedEvolution/commit/47e04410994b976c7be0d1a3a2ba8c914fb89b9d))
* ESLoader setup ([3f8dd1b](https://github.com/tiltedphoques/TiltedEvolution/commit/3f8dd1bcc72a6af35276fc76d3301adc2d5a3be4))
* fetch es files from Data/ ([39bfed8](https://github.com/tiltedphoques/TiltedEvolution/commit/39bfed88439f6d365dc02eaf2685ac89a10b528e))
* index parent esms and handle overwritten records ([7866a42](https://github.com/tiltedphoques/TiltedEvolution/commit/7866a42bdf24a21362830a08009e35eebdb32b31))
* integrated es parser into server ([7bd1acd](https://github.com/tiltedphoques/TiltedEvolution/commit/7bd1acd3bf5b4415a5445efb2c29a4adc7ae25d8))
* iterate and parse chunks in record ([173e3ee](https://github.com/tiltedphoques/TiltedEvolution/commit/173e3ee30ceb84a724d5101f1012881a103baaf9))
* load esm data and record base ([a208a85](https://github.com/tiltedphoques/TiltedEvolution/commit/a208a851ea08822a9d1308067f569ca6d8280b61))
* load game settings on server ([b0e1c33](https://github.com/tiltedphoques/TiltedEvolution/commit/b0e1c33dab4c1b4f75cb329acc50b6f43fb8388b))
* parse climate records ([dde6f16](https://github.com/tiltedphoques/TiltedEvolution/commit/dde6f1686e920b34776767aaeb7e8b683b5ad4d3))
* parse containers and decoded scripts data ([1ff6f73](https://github.com/tiltedphoques/TiltedEvolution/commit/1ff6f73ff341072c7e45c28bfb4cb8c1284f94cc))
* parse group headers ([46eb291](https://github.com/tiltedphoques/TiltedEvolution/commit/46eb2915ac054bdcbb1815ad27888c1a1bd3c180))
* parse map markers ([fb45e77](https://github.com/tiltedphoques/TiltedEvolution/commit/fb45e7761ca57c3fabef9d3de143e60853d21b17))
* parse NPCs ([5ce7919](https://github.com/tiltedphoques/TiltedEvolution/commit/5ce79195df3a592b289967f7a85d45dc339231c3))
* parse records and groups, and fetch fields ([a3e69f2](https://github.com/tiltedphoques/TiltedEvolution/commit/a3e69f2fa30dbe0ab1f11bb0f8dbf8b2ce6918b3))
* RecordCollection ([98c0825](https://github.com/tiltedphoques/TiltedEvolution/commit/98c0825e492b46670f3bfc30a046c98e70867454))
* sort files by esm/esp/esl ([e4ec55a](https://github.com/tiltedphoques/TiltedEvolution/commit/e4ec55a49479f6bbff67d7286534c8be96ba1a05))
* store cell records ([8b4021d](https://github.com/tiltedphoques/TiltedEvolution/commit/8b4021d19e4fca8106dd1e5930b1c64f55740451))
* working es parser ([ee5271e](https://github.com/tiltedphoques/TiltedEvolution/commit/ee5271eb5fadb66521e5f8f1ed4665a1157288c3))

# [1.8.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.7.0...v1.8.0) (2022-01-17)


### Features

* Serverlist tags support and a few bugfixes. ([37c4348](https://github.com/tiltedphoques/TiltedEvolution/commit/37c4348d7de7847e1c92f3c880c28490bdfbe19e))

# [1.7.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.6.0...v1.7.0) (2022-01-15)


### Bug Fixes

* Incorrect arg name fix ([b08d26b](https://github.com/tiltedphoques/TiltedEvolution/commit/b08d26ba073f89fbc3f4bf12059d1787b06ca8b6))


### Features

* Added coding guidelines ([5c413ae](https://github.com/tiltedphoques/TiltedEvolution/commit/5c413ae5363f2050348feb4dd8d117727710681c))
* Modified arg names and checking event opcodes first ([6d5c175](https://github.com/tiltedphoques/TiltedEvolution/commit/6d5c17536f4dd8d6d8754ed024b4f179c210824e))

# [1.6.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.5.1...v1.6.0) (2022-01-15)


### Bug Fixes

* Docker compose not using the correct output path for server list service ([4c87f3f](https://github.com/tiltedphoques/TiltedEvolution/commit/4c87f3f28c78c1abfe1e4900f3b9c7548c5b6267))
* Eliminate warnings. ([2080cb2](https://github.com/tiltedphoques/TiltedEvolution/commit/2080cb2b5cdf54f8672fe62ba525ceea49bda3a7))
* improve error handling for the server list service ([2d4defa](https://github.com/tiltedphoques/TiltedEvolution/commit/2d4defae507205f1fce651b088efe8e66ebb1ad2))
* re-enable SSL for masterlist endpoint. ([4fa7149](https://github.com/tiltedphoques/TiltedEvolution/commit/4fa714999bbc62894eaf1af14db18c7296b8bc18))
* server admin console now relies exclusively on xmake ([b2304a4](https://github.com/tiltedphoques/TiltedEvolution/commit/b2304a4a8692057a52189f149a356f3a4c680d39))


### Features

* Console commands POC ([0e8d9e8](https://github.com/tiltedphoques/TiltedEvolution/commit/0e8d9e8a5fcd5731ffc8795a2d4e1935e2560730))
* Console component ([cf7283d](https://github.com/tiltedphoques/TiltedEvolution/commit/cf7283dce052166b16e88af5c4e2010a6f1a6b30))
* EULA validation for server owners. ([d5148f1](https://github.com/tiltedphoques/TiltedEvolution/commit/d5148f1c2dce5f485348561c8d6474e033d50942))
* libbase setting<t> draft ([085fc9c](https://github.com/tiltedphoques/TiltedEvolution/commit/085fc9c4601fc334a9dacc503719fc9531a09705))
* server settings ini poc code. ([f2efe42](https://github.com/tiltedphoques/TiltedEvolution/commit/f2efe42a6670817efaea45c9fe5201d63982a493))
* Test coverage for console component ([de73919](https://github.com/tiltedphoques/TiltedEvolution/commit/de73919e24c50b7c7c4cc975f59a69ca0af5aca4))
* Upgrade api endpoint with icon, desc, tick support; re-enable list service. ([2873b39](https://github.com/tiltedphoques/TiltedEvolution/commit/2873b399a6cfb7d820d2fba8a023118bf0b01817))

## [1.5.1](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.5.0...v1.5.1) (2021-12-20)


### Bug Fixes

* projectile related crashes ([48eec06](https://github.com/tiltedphoques/TiltedEvolution/commit/48eec065e282898bdbd02ed7a2ad195665cd92d7))

# [1.5.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.4.1...v1.5.0) (2021-12-11)


### Features

* progress on leveled actor sync ([94d4039](https://github.com/tiltedphoques/TiltedEvolution/commit/94d4039174b565398ff5d4482b6be33c6f6e527f))

## [1.4.1](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.4.0...v1.4.1) (2021-12-11)


### Bug Fixes

* inventory crash fix ([7358e7f](https://github.com/tiltedphoques/TiltedEvolution/commit/7358e7f031418efef456d0f6218d8fa64e24ddf0))

# [1.4.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.3.0...v1.4.0) (2021-12-10)


### Bug Fixes

* distrib player messages bug ([3f37634](https://github.com/tiltedphoques/TiltedEvolution/commit/3f37634a36b416a0d3c1aad4f9b7bf4cc1f1ed46))


### Features

* updated mod to AE patch 1 ([912cae8](https://github.com/tiltedphoques/TiltedEvolution/commit/912cae8c91d65bb51ad100e3584a89f8b7d0c88a))

# [1.3.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.2.1...v1.3.0) (2021-12-07)


### Bug Fixes

* debugger logic bugs ([2bcb30e](https://github.com/tiltedphoques/TiltedEvolution/commit/2bcb30e9a8275cf32ca42b652112185ec8a051fc))
* fallout 4 build error ([a13154b](https://github.com/tiltedphoques/TiltedEvolution/commit/a13154bc73d856897d81467ffb58b387df88be85))
* server id lookup bug ([efba5dd](https://github.com/tiltedphoques/TiltedEvolution/commit/efba5dd40666596bd6c2e158bba42b0abaa9ab67))


### Features

* debug TESContainer ([0c48cc5](https://github.com/tiltedphoques/TiltedEvolution/commit/0c48cc5a087d88641abbd3a808146c9e29acfc72))
* half drawn bow sync ([120ea21](https://github.com/tiltedphoques/TiltedEvolution/commit/120ea21b1e08558f5ffb3909182f9c6f73e662fb))
* inventory debugger tool ([bdc2315](https://github.com/tiltedphoques/TiltedEvolution/commit/bdc231557c8f1f89a5378fc734257af6f0825414))
* major improvements to equip sync ([cb8fbcd](https://github.com/tiltedphoques/TiltedEvolution/commit/cb8fbcdada88f8cba130d1343e28dbf7ce910a59))
* perfect magic projectile sync ([da1a7e2](https://github.com/tiltedphoques/TiltedEvolution/commit/da1a7e2fced7e4be621e5d91084905aea065f095))
* reverse engineered extra data for skyrim containers ([19ea6a2](https://github.com/tiltedphoques/TiltedEvolution/commit/19ea6a2f67d0ea6b2d73c7716cfa4977bb8255bb))

## [1.2.1](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.2.0...v1.2.1) (2021-11-29)


### Bug Fixes

* removed 'skeever butt' dialogue from players ([60fd7bd](https://github.com/tiltedphoques/TiltedEvolution/commit/60fd7bda6684813a80b2ede828172f96d5a6d54e))

# [1.2.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.1.0...v1.2.0) (2021-11-25)


### Bug Fixes

* fix 'dragonsoul colected' bug ([8c56bfc](https://github.com/tiltedphoques/TiltedEvolution/commit/8c56bfc55a5be9c1d2cb2b5bcd80bc48f00b32e1))
* fps drop bug ([199b51c](https://github.com/tiltedphoques/TiltedEvolution/commit/199b51c81558884fdb46c3b6e3ec38f8d31be2ad))
* new addresses for object animations ([6e042c1](https://github.com/tiltedphoques/TiltedEvolution/commit/6e042c156540a6d580cbbd54546df49dbee96926))
* remote SetPlayer() added in reused spawn data ([da3a10e](https://github.com/tiltedphoques/TiltedEvolution/commit/da3a10eb6c8d8d46d11e4b23d1fec2881cad0fa4))


### Features

* MagicService ([bf77eae](https://github.com/tiltedphoques/TiltedEvolution/commit/bf77eaed105cbbc62811f4200b850a1273b90ef3))
* reverse engineered fallout 4 magic classes ([b05b415](https://github.com/tiltedphoques/TiltedEvolution/commit/b05b4151e5134986c94c08b2e5da7043c6c4d820))
* script to fetch addresses for new AE patch ([8684497](https://github.com/tiltedphoques/TiltedEvolution/commit/868449716295d5fe8ab5fa2b792407d3a1ebcc2d))
* shitty object anim sync implementation ([6202563](https://github.com/tiltedphoques/TiltedEvolution/commit/6202563ebfc10d5402c90ae946cefa5ec89c9f7f))
* sneak sync in fallout 4 ([cce6f47](https://github.com/tiltedphoques/TiltedEvolution/commit/cce6f47ee7015cfe26fa209068fbac3efffb9269))
* sneak sync in skyrim ([5be1768](https://github.com/tiltedphoques/TiltedEvolution/commit/5be17680332fa7bae39bf2fd805b4cdff8a599fc))

# [1.1.0](https://github.com/tiltedphoques/TiltedEvolution/compare/v1.0.0...v1.1.0) (2021-11-22)


### Bug Fixes

* crash fix ([8842773](https://github.com/tiltedphoques/TiltedEvolution/commit/88427732874fae9234c9ce526518b9c36dc95feb))
* fallout 4 crash fix ([1e2bd32](https://github.com/tiltedphoques/TiltedEvolution/commit/1e2bd32b05382f31bcc1094289acca799c571504))
* fix crash when casting spell on object ([4ed0172](https://github.com/tiltedphoques/TiltedEvolution/commit/4ed0172dea8f5fb8801e15a4e5172eac55bb9f55))


### Features

* debug animation variables ([55bb7bc](https://github.com/tiltedphoques/TiltedEvolution/commit/55bb7bc936a41c39464fd9f95b2f8c1707ae5093))

# 1.0.0 (2021-11-19)


### Bug Fixes

* converted some remaining AE addresses ([dcac1f8](https://github.com/tiltedphoques/TiltedEvolution/commit/dcac1f89e1fd320005a21c3c20b205286cd80f36))
* **magic:** re enabled magic sync ([48bc450](https://github.com/tiltedphoques/TiltedEvolution/commit/48bc450c567ad9114206f24f990ed084db9d4413))
* **projectiles:** build fixes ([01e4afa](https://github.com/tiltedphoques/TiltedEvolution/commit/01e4afacae427df63c73e5d4f617275efbda6922))
* Repair ActorMediator for AE ([dfa1b79](https://github.com/tiltedphoques/TiltedEvolution/commit/dfa1b791139741ebce94fd9852b807cd741c179c))


### Features

* AE sigma grindset (testing needed) ([8eaca85](https://github.com/tiltedphoques/TiltedEvolution/commit/8eaca8587a0b0b0699a449d9bf56cd5c35a9b429))
* **ai:** reverse engineered AI stuff ([16a999a](https://github.com/tiltedphoques/TiltedEvolution/commit/16a999ae6a24af71c1528310f09cfd36beef71a5))
* **anims:** fixed anims by hand (life is pain) ([a9a7ea2](https://github.com/tiltedphoques/TiltedEvolution/commit/a9a7ea25691b24327a4d392ddd3c6b63abb7e3c0))
* equip sync works for real now ([c8e324f](https://github.com/tiltedphoques/TiltedEvolution/commit/c8e324f2d3f0536c36f11d701ff804f3caaf7a7b))
* fallout 4 shitty projectile sync ([3f304bb](https://github.com/tiltedphoques/TiltedEvolution/commit/3f304bb9dd0629e2cdf49ea58ca73981f3775a80))
* implemented equip sync for fallout 4 ([088892e](https://github.com/tiltedphoques/TiltedEvolution/commit/088892e05c0b7c70c180d28347c216211d9628ad))
* Import the new versioning system ([5a23cca](https://github.com/tiltedphoques/TiltedEvolution/commit/5a23cca660fe73e77605149b42fdcec7c0735aa2))
* **magic:** AddTargetEvent ([e4f0393](https://github.com/tiltedphoques/TiltedEvolution/commit/e4f0393290fecbbb09f57d6d68ab802b61f2ae1c))
* **magic:** filter magic effects by archetype ([6ff5c98](https://github.com/tiltedphoques/TiltedEvolution/commit/6ff5c98a4215d5752633fbf996f06f716852b681))
* **magic:** intercept SummonCreatureEffect ([9fa92ac](https://github.com/tiltedphoques/TiltedEvolution/commit/9fa92ac0c9be2c922a43d632c3ac1897d52bbf98))
* **magic:** magic effects are now synced ([0df4fff](https://github.com/tiltedphoques/TiltedEvolution/commit/0df4fffad8f982ceeb6713b25996c4e166adcce4))
* **magic:** reverse engineerd MagicTarget system ([e60ef88](https://github.com/tiltedphoques/TiltedEvolution/commit/e60ef88d78871bd078c6024f5f6b26ec047958c1))
* script to convert to Anniversary update ([a53644e](https://github.com/tiltedphoques/TiltedEvolution/commit/a53644e0a7dae5eebe878d56e4d944ed26708001))


### Reverts

* Revert "Sync AttachArrow events" ([ea83570](https://github.com/tiltedphoques/TiltedEvolution/commit/ea835706e166ba2334554049c60b0a1ccd4b9158))
* Revert "Reverse engineerd AttachArrow for skyrim" ([e503975](https://github.com/tiltedphoques/TiltedEvolution/commit/e5039750eb0a8649bf71aad294ef75f5ff2d4c9c))
* Revert "Skyrim dwarven sphere animation sync" ([6bfa413](https://github.com/tiltedphoques/TiltedEvolution/commit/6bfa413c4a426cd37a3b0891f12f416d00c45108))
* Revert "Extra boolean buffer" ([656f3d4](https://github.com/tiltedphoques/TiltedEvolution/commit/656f3d4d5f24d45fa8bc2f06ec4fb24af39b9e06))
* Revert "Cleanup" ([fc105e0](https://github.com/tiltedphoques/TiltedEvolution/commit/fc105e0bb0ee4ca0cdc2aaf69a00cdd99b3517a3))
* Revert "added debugoutput for inventory events" ([2ec1aa4](https://github.com/tiltedphoques/TiltedEvolution/commit/2ec1aa400af0a6e7f404ae0da9929f2f14b2b00c))
