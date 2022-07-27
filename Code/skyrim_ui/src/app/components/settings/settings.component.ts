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

  public settings = this.settingService.settings;
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
    private readonly settingService: SettingService,
    private readonly client: ClientService,
    private readonly sound: SoundService,
    private readonly translocoService: TranslocoService,
  ) {
  }

  ngOnInit(): void {
    this.muted = this.settingService.isAudioMuted();
    this.showDebug = this.settingService.isDebugShown();
    this.autoHideParty = this.settingService.isPartyAutoHidden();
    this.showParty = this.settingService.isPartyShown();
    this.autoHideTime = this.settingService.getAutoHideTime();
    this.partyAnchor = this.settingService.getPartyAnchor();
    this.partyAnchorOffsetX = this.settingService.getPartyAnchorOffsetX();
    this.partyAnchorOffsetY = this.settingService.getPartyAnchorOffsetY();
    this.language = this.settingService.getLanguage();
  }

  onMutedChange(checked: boolean) {
    if (checked) {
      this.sound.play(Sound.Check);
    }
    this.settingService.muteAudio(checked);
    if (!checked) {
      this.sound.play(Sound.Uncheck);
    }
    this.settingsUpdated.next();
  }

  onShowDebugChange(checked: boolean) {
    this.settingService.setDebugShown(checked);
    this.client.debugStateChange.next(checked);
    this.sound.play(checked ? Sound.Check : Sound.Uncheck);
    this.settingsUpdated.next();
  }

  onShowPartyChange(checked: boolean) {
    this.settingService.showParty(checked);
    this.sound.play(checked ? Sound.Check : Sound.Uncheck);
    this.settingsUpdated.next();
  }

  onAutoHidePartyChange(checked: boolean) {
    this.settingService.autoHideParty(checked);
    this.autoHideParty = checked;
    this.sound.play(checked ? Sound.Check : Sound.Uncheck);
    this.settingsUpdated.next();
  }

  onAutoHideTimeChange(time: number) {
    this.settingService.setAutoHideTime(time);
    this.autoHideTime = time;
    this.sound.play(Sound.Check);
    this.settingsUpdated.next();
  }

  onPartyAnchorChange(anchor: PartyAnchor) {
    this.settingService.setPartyAnchor(anchor);
    this.partyAnchor = anchor;
    this.settingsUpdated.next();
  }

  onPartyAnchorOffsetXChange(offset: number) {
    this.settingService.setPartyAnchorOffsetX(offset);
    this.partyAnchorOffsetX = offset;
    this.settingsUpdated.next();
  }

  onPartyAnchorOffsetYChange(offset: number) {
    this.settingService.setPartyAnchorOffsetY(offset);
    this.partyAnchorOffsetY = offset;
    this.settingsUpdated.next();
  }

  onLanguageChange(language: string) {
    this.settingService.setLanguage(language);
    this.language = language;
    this.settingsUpdated.next();
    this.translocoService.setActiveLang(language);
  }

  public autoHideTimeSelected(number: number): boolean {
    return this.settingService.getAutoHideTime() === number;
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
