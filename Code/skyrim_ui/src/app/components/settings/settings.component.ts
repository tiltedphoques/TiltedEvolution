import { Component, EventEmitter, HostListener, OnInit, Output } from '@angular/core';
import { ClientService } from 'src/app/services/client.service';
import { SettingService } from 'src/app/services/setting.service';
import { Sound, SoundService } from '../../services/sound.service';
import { RootView } from '../root/root.component';

export enum FontSize {
  XS = 'xs',
  S = 's',
  M = 'm',
  L = 'l',
  XL = 'xl'
}

export enum PartyAnchor {
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_RIGHT,
  BOTTOM_LEFT,
}

@Component({
  selector: 'app-settings',
  templateUrl: './settings.component.html',
  styleUrls: ['./settings.component.scss'],
})
export class SettingsComponent implements OnInit {

  /* ### ENUMS ### */
  readonly PartyAnchor = PartyAnchor;

  public volume: number;
  public muted: boolean;
  public showDebug: boolean;
  public autoHideParty: boolean;
  public showParty: boolean;
  public autoHideTime: number;
  public partyAnchor: PartyAnchor;
  public partyAnchorOffsetX: number;
  public partyAnchorOffsetY: number;
  public fontSize: FontSize;
  private _fontSize: number;

  public maxFontSize = Object.values(FontSize).length - 1;
  public minFontSize = 0;

  @Output() public done = new EventEmitter<void>();
  @Output() public setView = new EventEmitter<RootView>();
  @Output() public settingsUpdated = new EventEmitter<void>();

  constructor(
    private readonly settings: SettingService,
    private readonly client: ClientService,
    private readonly sound: SoundService,
  ) {
  }

  ngOnInit(): void {
    this.volume = this.settings.getVolume();
    this.muted = this.settings.isAudioMuted();
    this.showDebug = this.settings.isDebugShown();
    this.autoHideParty = this.settings.isPartyAutoHidden();
    this.showParty = this.settings.isPartyShown();
    this.autoHideTime = this.settings.getAutoHideTime();
    this.partyAnchor = this.settings.getPartyAnchor();
    this.partyAnchorOffsetX = this.settings.getPartyAnchorOffsetX();
    this.partyAnchorOffsetY = this.settings.getPartyAnchorOffsetY();
    this.fontSize = this.settings.getFontSize();
    this._fontSize = Object.values(FontSize).indexOf(this.fontSize);
  }

  onMutedChange(checked: boolean) {
    if (checked) {
      this.sound.play(Sound.Check);
    }
    this.settings.muteAudio(checked);
    if (!checked) {
      this.sound.play(Sound.Uncheck);
    }
    this.settingsUpdated.next();
  }

  onVolumeChange(volume: number) {
    this.settings.setVolume(volume);
    this.settingsUpdated.next();
  }

  onShowDebugChange(checked: boolean) {
    this.settings.setDebugShown(checked);
    this.client.debugStateChange.next(checked);
    this.sound.play(checked ? Sound.Check : Sound.Uncheck);
    this.settingsUpdated.next();
  }

  onShowPartyChange(checked: boolean) {
    this.settings.showParty(checked);
    this.sound.play(checked ? Sound.Check : Sound.Uncheck);
    this.settingsUpdated.next();
  }

  onAutoHidePartyChange(checked: boolean) {
    this.settings.autoHideParty(checked);
    this.autoHideParty = checked;
    this.sound.play(checked ? Sound.Check : Sound.Uncheck);
    this.settingsUpdated.next();
  }

  onAutoHideTimeChange(time: number) {
    this.settings.setAutoHideTime(time);
    this.autoHideTime = time;
    this.sound.play(Sound.Check);
    this.settingsUpdated.next();
  }

  onPartyAnchorChange(anchor: PartyAnchor) {
    this.settings.setPartyAnchor(anchor);
    this.partyAnchor = anchor;
    this.settingsUpdated.next();
  }

  onPartyAnchorChangeOffsetX(offset: number) {
    this.settings.setPartyAnchorOffsetX(offset);
    this.partyAnchorOffsetX = offset;
    this.settingsUpdated.next();
  }

  onPartyAnchorChangeOffsetY(offset: number) {
    this.settings.setPartyAnchorOffsetY(offset);
    this.partyAnchorOffsetY = offset;
    this.settingsUpdated.next();
  }

  onFontSizeChange(size: number) {
    if (size >= this.minFontSize && size <= this.maxFontSize) {
      const fontSize = Object.values(FontSize)[size];
      this.settings.setFontSize(fontSize);
      this.fontSize = fontSize;
      this._fontSize = size;
      this.settingsUpdated.next();
    }
  }

  onFontSizeUp() {
    this.onFontSizeChange(this._fontSize + 1)
  }

  onFontSizeDown() {
    this.onFontSizeChange(this._fontSize - 1)
  }

  public autoHideTimeSelected(number: number): boolean {
    return this.settings.getAutoHideTime() === number;
  }

  close() {
    this.done.next();
    this.sound.play(Sound.Ok);
  }

  @HostListener('window:keydown.escape', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    this.close();
    event.stopPropagation();
    event.preventDefault();
  }
}
