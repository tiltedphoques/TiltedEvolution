import { APP_INITIALIZER, Provider } from '@angular/core';
import { MockClientService } from '../app/mock/mock-client.service';
import { mockSkyrimTogether } from '../app/mock/skyrimtogether.mock';


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
      useFactory: (mockClientService: MockClientService) => () => undefined,
      deps: [MockClientService],
      multi: true,
    },
    MockClientService,
  ] as Provider[],
};
