import { HttpClient } from '@angular/common/http';
import { Injectable, NgModule } from '@angular/core';
import {
  provideTransloco,
  Translation,
  TranslocoLoader,
  TranslocoModule,
} from '@jsverse/transloco';
import { environment } from '../environments/environment';

@Injectable({ providedIn: 'root' })
export class TranslocoHttpLoader implements TranslocoLoader {
  constructor(private readonly http: HttpClient) {}

  getTranslation(lang: string) {
    return this.http.get<Translation>(`assets/i18n/${lang}.json`);
  }
}

@NgModule({
  exports: [TranslocoModule],
  providers: [
    provideTransloco({
      config: {
        availableLangs: [
          { id: 'en', label: 'English' },
          { id: 'de', label: 'Deutsch' },
          { id: 'ru', label: 'Русский' },
          { id: 'fr', label: 'Français' },
          { id: 'zh-CN', label: '中文（中国）' },
          { id: 'ja', label: '日本語' },
          { id: 'nl', label: 'Nederlands' },
          { id: 'no', label: 'Norsk' },
          { id: 'sv', label: 'Svenska' },
          { id: 'es', label: 'Español' },
          { id: 'ko', label: '한국어' },
          { id: 'tr', label: 'Türkçe' },
          { id: 'cs', label: 'Čeština' },
          { id: 'pl', label: 'Polski' },
          { id: 'uk', label: 'Українська' },
          { id: 'overwrite', label: 'Custom' },
        ],
        defaultLang: 'en',
        fallbackLang: 'en',
        missingHandler: {
          useFallbackTranslation: true,
        },
        reRenderOnLangChange: true,
        prodMode: environment.production,
      },
      loader: TranslocoHttpLoader,
    }),
  ],
})
export class TranslocoRootModule {}
