import { APP_INITIALIZER, Provider } from '@angular/core';
import { mockSkyrimTogether, SkyrimtogetherMock } from '../app/mock/skyrimtogether.mock';


mockSkyrimTogether();

export const environment = {
  production: false,
  game: false,
  urlProtocol: 'http',
  url: 'localhost:4200',
  chatMessageLengthLimit: 512,
  nbReconnectionAttempts: 5,

  providers: [
    {
      provide: APP_INITIALIZER,
      useFactory: () => () => (skyrimtogether as SkyrimtogetherMock).initMock(),
      multi: true,
    },
  ] as Provider[],
};
