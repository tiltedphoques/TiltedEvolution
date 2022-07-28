import { Provider } from '@angular/core';


export const environment = {
  production: true,
  game: true,
  urlProtocol: 'https',
  url: 'skyrim-reborn-list.skyrim-together.com',
  chatMessageLengthLimit: 512,
  nbReconnectionAttempts: 5,

  providers: [] as Provider[],
};
