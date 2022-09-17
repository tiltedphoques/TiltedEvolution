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

class SelectSetting extends Setting<string> {

  constructor(
    private readonly storeService: StoreService, 
    private storeKey: string, 
    private options: string[], 
    defaultValue: string
  ) {
    const storedValue = storeService.get(storeKey, defaultValue);
    super(options.includes(storedValue) ? storedValue : defaultValue);
  }

  public next(value: string) {
    if (this.options.includes(value)) {
      this.storeService.set(this.storeKey, value);
      super.next(value);
    }
  }
}

@Injectable({
  providedIn: 'root',
})
export class SettingService {

  private readonly languageValues: string[] = Object.values(this.translocoService.getAvailableLangs()).map(lang => lang.id);
  private readonly fontSizeValues = Object.values(FontSize);

  public settings = {
    volume: new SliderSetting(this.storeService, "audio_volume", 0.5),
    muted: new ToggleSetting(this.storeService, "audio_muted", false),
    language: new SelectSetting(this.storeService, "language", this.languageValues, "en"),
    fontSize: new SelectSetting(this.storeService, "font_size", this.fontSizeValues, FontSize.M),
    isPartyShown: new ToggleSetting(this.storeService, "party_isShown", true),
    autoHideParty: new ToggleSetting(this.storeService, "party_autoHide", false),
    partyAnchorOffsetX: new SliderSetting(this.storeService, "party_anchor_offset_x", 0),
    partyAnchorOffsetY: new SliderSetting(this.storeService, "party_anchor_offset_y", 3),
    isDebugShown: new ToggleSetting(this.storeService, "debug_isShown", false),
  }

  constructor(
    private readonly storeService: StoreService,
    private readonly translocoService: TranslocoService,
  ) {
    this.settings.language.subscribe(lang => translocoService.setActiveLang(lang));    
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

}
