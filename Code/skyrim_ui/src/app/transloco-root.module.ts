import { HttpClient } from '@angular/common/http';
import { Injectable, NgModule } from '@angular/core';
import { Translation, TRANSLOCO_CONFIG, TRANSLOCO_LOADER, translocoConfig, TranslocoLoader, TranslocoModule } from '@ngneat/transloco';
import { environment } from '../environments/environment';


@Injectable({ providedIn: 'root' })
export class TranslocoHttpLoader implements TranslocoLoader {

  constructor(
    private readonly http: HttpClient,
  ) {
  }

  getTranslation(lang: string) {
    return this.http.get<Translation>(`assets/i18n/${ lang }.json`);
  }
}

@NgModule({
  exports: [TranslocoModule],
  providers: [
    {
      provide: TRANSLOCO_CONFIG,
      useValue: translocoConfig({
        availableLangs: [
          { id: 'de', label: 'Deutsch' },
          { id: 'en', label: 'English' },
          { id: 'fr', label: 'Français'}, 
          { id: 'nl', label: 'Nederlands' },
        ],
        defaultLang: 'overwrite',
        fallbackLang: 'en',
        missingHandler: {
          useFallbackTranslation: true,
        },
        prodMode: environment.production,
      }),
    },
    { provide: TRANSLOCO_LOADER, useClass: TranslocoHttpLoader },
  ],
})
export class TranslocoRootModule {
}
