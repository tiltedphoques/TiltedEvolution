import { ChangeDetectionStrategy, Component, Input } from '@angular/core';
import {
  faVolumeHigh,
  faVolumeLow,
  faVolumeXmark,
} from '@fortawesome/free-solid-svg-icons';
import { BehaviorSubject, combineLatest, map } from 'rxjs';
import { VoipChannel, VoipService } from '../../services/voip.service';

type SpeakerState = 'off' | 'muted' | 'quiet' | 'talking';

/** A player's speaker icon in the Voip column, and the popup with their volume. */
@Component({
  selector: 'app-voip-speaker',
  templateUrl: './voip-speaker.component.html',
  styleUrls: ['./voip.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class VoipSpeakerComponent {
  readonly faVolumeHigh = faVolumeHigh;
  readonly faVolumeLow = faVolumeLow;
  readonly faVolumeXmark = faVolumeXmark;

  private readonly player$ = new BehaviorSubject<{
    id: number;
    name: string;
    isMember: boolean;
  }>({
    id: 0,
    name: '',
    isMember: false,
  });

  @Input() set playerId(value: number) {
    this.player$.next({ ...this.player$.value, id: value });
  }
  @Input() set playerName(value: string) {
    this.player$.next({ ...this.player$.value, name: value });
  }
  @Input() set isMember(value: boolean) {
    this.player$.next({ ...this.player$.value, isMember: !!value });
  }

  isOpen = false;

  readonly settings$ = this.voipService.settings;
  readonly volume$ = combineLatest([
    this.player$,
    this.voipService.volumes,
  ]).pipe(map(([player]) => this.voipService.getVolume(player.name)));
  readonly state$ = combineLatest([
    this.player$,
    this.voipService.settings,
    this.voipService.volumes,
    this.voipService.talking,
  ]).pipe(
    map(([player, settings, , talking]): SpeakerState => {
      const heard =
        settings.channel === VoipChannel.EVERYONE ||
        (settings.channel === VoipChannel.PARTY && player.isMember) ||
        (settings.channel === VoipChannel.STRANGERS && !player.isMember);
      if (!settings.connect || !heard) {
        return 'off';
      }
      if (this.voipService.getVolume(player.name) === 0) {
        return 'muted';
      }
      return talking.has(player.id) ? 'talking' : 'quiet';
    }),
  );
  readonly isMember$ = this.player$.pipe(map(player => player.isMember));
  readonly name$ = this.player$.pipe(map(player => player.name));

  constructor(private readonly voipService: VoipService) {}

  icon(state: SpeakerState) {
    switch (state) {
      case 'talking':
        return this.faVolumeHigh;
      case 'quiet':
        return this.faVolumeLow;
      default:
        return this.faVolumeXmark;
    }
  }

  toggle(): void {
    this.isOpen = !this.isOpen;
  }

  close(): void {
    this.isOpen = false;
  }

  setVolume(value: number): void {
    const player = this.player$.value;
    this.voipService.setVolume(player.id, player.name, Number(value));
  }
}
