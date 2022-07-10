import { APP_INITIALIZER, Provider } from '@angular/core';
import { MockClientService } from '../app/services/mock-client.service';


export const environment = {
  production: false,
  nightlyBuild: true,
  game: false,
  urlProtocol: 'http',
  url: 'localhost:4200',
  intervalPingWebSocket: 5, // seconds
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
