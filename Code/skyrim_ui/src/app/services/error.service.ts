import { Injectable } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { BehaviorSubject, firstValueFrom } from 'rxjs';
import { Sound, SoundService } from './sound.service';


export interface ErrorEvent {
  error: 'wrong_version' | 'mods_mismatch' | 'client_mods_disallowed' | 'wrong_password'  | 'server_full'| 'no_reason';
  data?: Record<any, any>;
}

export interface WrongVersionErrorEvent extends ErrorEvent {
  error: 'wrong_version';
  data: { version: string; expectedVersion: string };
}

export interface ModsMismatchErrorEvent extends ErrorEvent {
  error: 'mods_mismatch';
  data: { mods: [filename: string, id: string][] };
}

export interface ClientModsDisallowedErrorEvent extends ErrorEvent {
  error: 'client_mods_disallowed';
  data: { mods: ('SKSE' | 'MO2')[] };
}

export interface ServerFullErrorEvent extends ErrorEvent {
  error: 'server_full';
}

export type ErrorEvents =
  | WrongVersionErrorEvent
  | ModsMismatchErrorEvent
  | ClientModsDisallowedErrorEvent
  | ServerFullErrorEvent
  | ErrorEvent

@Injectable({
  providedIn: 'root',
})
export class ErrorService {

  private error = new BehaviorSubject<string>('');
  public error$ = this.error.asObservable();

  constructor(
    private readonly sound: SoundService,
    private readonly translocoService: TranslocoService,
  ) {
  }

  getError() {
    return this.error.getValue();
  }

  async setError(error: ErrorEvents | string) {
    this.sound.play(Sound.Fail);
    let message: string;
    if (typeof error === 'string') {
      message = error;
    } else {
      let data: Record<string, any> = error.data;
      switch (error.error) {
        case 'mods_mismatch':
          let mods = '';
          const install = error.data.mods.filter(mod => mod[1] === '0').map(mod => mod[0]);
          if (install.length > 0) {
            mods += '\n\n<hr>\n';
            mods += await firstValueFrom(
              this.translocoService.selectTranslate(
                'SERVICE.ERROR.ERRORS.MODS_MISMATCH_INSTALL',
                { mods: `<strong>${ install.join(', ') }</strong>` },
              ),
            );
          }
          const remove = error.data.mods.filter(mod => mod[1] !== '0').map(mod => mod[0]);
          if (remove.length > 0) {
            mods += '\n\n<hr>\n';
            mods += await firstValueFrom(
              this.translocoService.selectTranslate(
                'SERVICE.ERROR.ERRORS.MODS_MISMATCH_REMOVE',
                { mods: `<strong>${ remove.join(', ') }</strong>` },
              ),
            );
          }
          data = { mods };
          break;
        case 'client_mods_disallowed':
          data = { mods: error.data.mods.join(', ') };
          break;
      }
      message = await firstValueFrom(
        this.translocoService.selectTranslate('SERVICE.ERROR.ERRORS.' + error.error.toUpperCase(), data),
      );
    }
    this.error.next(message);
  }

  removeError() {
    this.sound.play(Sound.Ok);
    this.error.next('');
  }

}
