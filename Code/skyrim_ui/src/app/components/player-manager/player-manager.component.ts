import { ChangeDetectionStrategy, Component, EventEmitter, HostListener, Output } from '@angular/core';
import { PlayerManagerTab } from '../../models/player-manager-tab.enum';
import { Sound, SoundService } from '../../services/sound.service';
import { UiRepository } from '../../store/ui.repository';


@Component({
  selector: 'app-player-manager',
  templateUrl: './player-manager.component.html',
  styleUrls: ['./player-manager.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class PlayerManagerComponent {

  /* ### ENUMS ### */
  readonly PlayerManagerTab = PlayerManagerTab;

  activeTab$ = this.uiRepository.playerManagerTab$;

  @Output() public done = new EventEmitter<void>();

  constructor(
    private readonly sound: SoundService,
    private readonly uiRepository: UiRepository,
  ) {
  }

  switchTab(tab: PlayerManagerTab) {
    this.uiRepository.openPlayerManagerTab(tab);
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
