import { Injectable } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { BehaviorSubject, Observable } from 'rxjs';
import {FontSize, PartyAnchor} from '../components/settings/settings.component';
import { StoreService } from './store.service';

class Setting<T> extends BehaviorSubject<T>{

  constructor(defaultValue: T) {
    super(defaultValue)
  }

  public next(value: T) {
    console.log("next", value)
    super.next(value)
  }
}

class SliderSetting extends Setting<number> {

  constructor(private readonly storeService: StoreService, private storeKey: string, defaultValue: number) {
    const inital_value = storeService.getFloat(storeKey, defaultValue);
    super(inital_value)
  }

  public next(value: number) {
    this.storeService.set(this.storeKey, value)
    super.next(value)
  }
}

class ToggleSetting extends Setting<boolean> {

  constructor(readonly storeService: StoreService, private storeKey: string, defaultValue: boolean) {
    const inital_value = storeService.getBool(storeKey, defaultValue);
    super(inital_value)
  }

  public next(value: boolean) {
    this.storeService.set(this.storeKey, value)
    super.next(value)
  }
}

@Injectable({
  providedIn: 'root',
})
export class SettingService {
  public fontSizeChange = new BehaviorSubject(this.getFontSize())

  public settings = {
    volume: new SliderSetting(this.storeService, "audio_volume", 0.5),
    muted: new ToggleSetting(this.storeService, "audio_muted", false),
    isPartyShown: new ToggleSetting(this.storeService, "party_isShown", true),
    autoHideParty: new ToggleSetting(this.storeService, "party_autoHide", false),
  }

  constructor(
    private readonly storeService: StoreService,
    private readonly translocoService: TranslocoService,
  ) {
  }

  public setDebugShown(shown: boolean) {
    this.storeService.set('debug_isShown', shown);
  }

  public isDebugShown(): boolean {
    return JSON.parse(this.storeService.get('debug_isShown', false));
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

  public setFontSize(size: FontSize) {
    this.fontSizeChange.next(size);
    this.storeService.set('font_size', size);
  }

  public getFontSize(): FontSize {
    return this.storeService.get('font_size', FontSize.M);
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
