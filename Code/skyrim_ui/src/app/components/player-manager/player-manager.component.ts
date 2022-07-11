import { Component, EventEmitter, HostListener, Output, ViewEncapsulation } from '@angular/core';
import { BehaviorSubject } from 'rxjs';
import { Sound, SoundService } from '../../services/sound.service';


export enum PlayerManagerTab {
  PLAYER_LIST,
  PARTY_MENU
}

@Component({
  selector: 'app-player-manager',
  templateUrl: './player-manager.component.html',
  styleUrls: ['./player-manager.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class PlayerManagerComponent {

  /* ### ENUMS ### */
  readonly PlayerManagerTab = PlayerManagerTab;

  activeTab = new BehaviorSubject(PlayerManagerTab.PLAYER_LIST);

  @Output() public done = new EventEmitter<void>();

  constructor(
    private readonly sound: SoundService,
  ) {
  }

  switchTab(tab: PlayerManagerTab) {
    this.activeTab.next(tab);
    this.sound.play(Sound.Focus);
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
