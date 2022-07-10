import { Component, EventEmitter, HostListener, OnInit, Output, ViewEncapsulation } from '@angular/core';
import { ClientService } from 'src/app/services/client.service';
import { SettingService } from 'src/app/services/setting.service';
import { Sound, SoundService } from '../../services/sound.service';
import { RootView } from '../root/root.component';


@Component({
  selector: 'app-settings',
  templateUrl: './settings.component.html',
  styleUrls: ['./settings.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class SettingsComponent implements OnInit {

  public volume: number;
  public muted: boolean;
  public showDebug: boolean;
  public autoHideParty: boolean;
  public showParty: boolean;
  public autoHideTime: number;

  @Output() public done = new EventEmitter<void>();
  @Output() public setView = new EventEmitter<RootView>();

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
  }

  onMutedChange(event: any) {
    if (event.target.checked) {
      this.sound.play(Sound.Check);
    }
    this.settings.muteAudio(event.target.checked);
    if (!event.target.checked) {
      this.sound.play(Sound.Uncheck);
    }
  }

  onVolumeChange(event: any) {
    this.settings.setVolume(event.target.value);
  }

  onShowDebugChange(event: any) {
    this.settings.setDebugShown(event.target.checked);
    this.client.debugStateChange.next(event.target.checked);
    this.sound.play(event.target.checked ? Sound.Check : Sound.Uncheck);
  }

  onShowPartyChange(event: any) {
    this.settings.showParty(event.target.checked);
    this.sound.play(event.target.checked ? Sound.Check : Sound.Uncheck);
  }

  onAutoHidePartyChange(event: any) {
    this.settings.autoHideParty(event.target.checked);
    this.autoHideParty = event.target.checked;
    this.sound.play(event.target.checked ? Sound.Check : Sound.Uncheck);
  }

  onAutoHideTimeChange(event: any) {
    this.settings.setAutoHideTime(event.target.value);
    this.autoHideTime = event.target.value;
    this.sound.play(Sound.Check);
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
