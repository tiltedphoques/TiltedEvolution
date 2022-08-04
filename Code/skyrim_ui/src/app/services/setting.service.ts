import { Injectable } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { BehaviorSubject } from 'rxjs';
import { PartyAnchor } from '../components/settings/settings.component';
import { StoreService } from './store.service';


@Injectable({
  providedIn: 'root',
})
export class SettingService {

  public partyShownChange = new BehaviorSubject(this.isPartyShown());
  public partyAutoHideChange = new BehaviorSubject(this.isPartyAutoHidden());

  constructor(
    private readonly storeService: StoreService,
    private readonly translocoService: TranslocoService,
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

  public setPartyAnchor(anchor: PartyAnchor) {
    this.storeService.set('party_anchor', anchor);
  }

  public getPartyAnchor(): PartyAnchor {
    return JSON.parse(this.storeService.get('party_anchor', PartyAnchor.TOP_LEFT));
  }

  public setPartyAnchorOffsetX(offset: number) {
    this.storeService.set('party_anchor_offset_x', offset);
  }

  public getPartyAnchorOffsetX(): number {
    return JSON.parse(this.storeService.get('party_anchor_offset_x', 0));
  }

  public setPartyAnchorOffsetY(offset: number) {
    this.storeService.set('party_anchor_offset_y', offset);
  }

  public getPartyAnchorOffsetY(): number {
    return JSON.parse(this.storeService.get('party_anchor_offset_y', 3));
  }

  public getLanguage(): string {
    const language = this.storeService.get('language', 'en');
    const isValid = !!(this.translocoService.getAvailableLangs() as any[])
      .find(lang => lang === language || lang.id === language);
    if (isValid) {
      return language;
    }
    return this.translocoService.getDefaultLang();
  }

  public setLanguage(language: string) {
    this.storeService.set('language', language);
  }
}
