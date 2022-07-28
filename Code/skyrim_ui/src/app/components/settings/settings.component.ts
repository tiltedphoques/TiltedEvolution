import { Component, EventEmitter, HostListener, OnInit, Output } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { ClientService } from 'src/app/services/client.service';
import { SettingService } from 'src/app/services/setting.service';
import { Sound, SoundService } from '../../services/sound.service';


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

  readonly availableLanguages = this.translocoService.getAvailableLangs();

  public volume: number;
  public muted: boolean;
  public showDebug: boolean;
  public autoHideParty: boolean;
  public showParty: boolean;
  public autoHideTime: number;
  public partyAnchor: PartyAnchor;
  public partyAnchorOffsetX: number;
  public partyAnchorOffsetY: number;
  public language: string;

  @Output() public done = new EventEmitter<void>();
  @Output() public settingsUpdated = new EventEmitter<void>();

  constructor(
    private readonly settings: SettingService,
    private readonly client: ClientService,
    private readonly sound: SoundService,
    private readonly translocoService: TranslocoService,
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
    this.language = this.settings.getLanguage();
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

  onPartyAnchorOffsetXChange(offset: number) {
    this.settings.setPartyAnchorOffsetX(offset);
    this.partyAnchorOffsetX = offset;
    this.settingsUpdated.next();
  }

  onPartyAnchorOffsetYChange(offset: number) {
    this.settings.setPartyAnchorOffsetY(offset);
    this.partyAnchorOffsetY = offset;
    this.settingsUpdated.next();
  }

  onLanguageChange(language: string) {
    this.settings.setLanguage(language);
    this.language = language;
    this.settingsUpdated.next();
    this.translocoService.setActiveLang(language);
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
