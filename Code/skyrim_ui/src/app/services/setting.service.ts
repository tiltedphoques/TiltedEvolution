import { Injectable } from '@angular/core';
import { BehaviorSubject } from 'rxjs';
import { StoreService } from './store.service';


@Injectable({
  providedIn: 'root',
})
export class SettingService {

  public partyShownChange = new BehaviorSubject(this.isPartyShown());
  public partyAutoHideChange = new BehaviorSubject(this.isPartyAutoHidden());

  constructor(
    private readonly storeService: StoreService,
  ) {
  }

  public setVolume(volume: number) {
    this.storeService.set('audio_volume', volume);
  }

  public getVolume(): number {
    return JSON.parse(this.storeService.get('audio_volume', 0.5));
  }

  public muteAudio(muted: boolean) {
    this.storeService.set('audio_muted', muted);
  }

  public isAudioMuted(): boolean {
    return JSON.parse(this.storeService.get('audio_muted', false));
  }

  public setDebugShown(shown: boolean) {
    this.storeService.set('debug_isShown', shown);
  }

  public isDebugShown(): boolean {
    return JSON.parse(this.storeService.get('debug_isShown', false));
  }

  public showParty(showParty: boolean) {
    this.storeService.set('party_isShown', showParty);
    this.partyShownChange.next(showParty);
  }

  public isPartyShown(): boolean {
    return JSON.parse(this.storeService.get('party_isShown', true));
  }

  public autoHideParty(autoHideParty: boolean) {
    this.storeService.set('party_autoHide', autoHideParty);
    this.partyAutoHideChange.next(autoHideParty);
  }

  public isPartyAutoHidden(): boolean {
    return JSON.parse(this.storeService.get('party_autoHide', false));
  }

  public setAutoHideTime(time: number) {
    this.storeService.set('party_autoHideTime', time);
  }

  public getAutoHideTime(): number {
    return JSON.parse(this.storeService.get('party_autoHideTime', 3));
  }
}
