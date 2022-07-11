// The file contents for the current environment will overwrite these during build.
// The build system defaults to the dev environment which uses `environment.ts`, but if you do
// `ng build --env=prod` then `environment.prod.ts` will be used instead.
// The list of which env maps to which file can be found in `.angular-cli.json`.

import { Provider } from '@angular/core';


export const environment = {
  production: false,
  game: true,
  urlProtocol: 'https',
  url: 'skyrim-reborn-list.skyrim-together.com',
  chatMessageLengthLimit: 512,
  nbReconnectionAttempts: 5,

  providers: [] as Provider[],
};
