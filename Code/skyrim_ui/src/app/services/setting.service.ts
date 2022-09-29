import { Injectable } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { BehaviorSubject } from 'rxjs';
import { StoreService } from './store.service';

export enum FontSize {
  XS = 'xs',
  S = 's',
  M = 'm',
  L = 'l',
  XL = 'xl'
}

export enum PartyAnchor {
  TOP_LEFT = 0,
  TOP_RIGHT,
  BOTTOM_RIGHT,
  BOTTOM_LEFT,
}

export const autoHideTimerLengths = [1, 3, 5];

export const fontSizeToPixels: Record<FontSize, number> = {
  [FontSize.XS]: 10,
  [FontSize.S]: 12,
  [FontSize.M]: 16,
  [FontSize.L]: 20,
  [FontSize.XL]: 26,
}

class Setting<T> extends BehaviorSubject<T>{

  constructor(defaultValue: T) {
    super(defaultValue)
  }
}

class SliderSetting extends Setting<number> {

  constructor(private readonly storeService: StoreService, private storeKey: string, defaultValue: number) {
    const initialValue = storeService.getFloat(storeKey, defaultValue);
    super(initialValue)
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

class SelectSetting<T extends string | number> extends Setting<T> {

  constructor(
    private readonly storeService: StoreService, 
    private storeKey: string, 
    private options: T[], 
    defaultValue: T
  ) {
    const storedValue = typeof defaultValue === "number" ? storeService.getFloat(storeKey, defaultValue) : storeService.get(storeKey, defaultValue);
    super(options.includes(storedValue) ? storedValue : defaultValue);
  }

  public next(value: T) {
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
  private readonly partyAnchorValues = Object.values(PartyAnchor) as PartyAnchor[];
  private readonly autoHideTimeValues = autoHideTimerLengths;

  public settings = {
    volume: new SliderSetting(this.storeService, "audio_volume", 0.5),
    muted: new ToggleSetting(this.storeService, "audio_muted", false),
    language: new SelectSetting(this.storeService, "language", this.languageValues, this.translocoService.getDefaultLang()),
    fontSize: new SelectSetting(this.storeService, "font_size", this.fontSizeValues, FontSize.M),
    isPartyShown: new ToggleSetting(this.storeService, "party_isShown", true),
    autoHideParty: new ToggleSetting(this.storeService, "party_autoHide", false),
    autoHideTime: new SelectSetting(this.storeService, "party_autoHideTime", this.autoHideTimeValues, this.autoHideTimeValues[0]),
    partyAnchor: new SelectSetting(this.storeService, "party_anchor", this.partyAnchorValues, PartyAnchor.TOP_LEFT),
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
}
