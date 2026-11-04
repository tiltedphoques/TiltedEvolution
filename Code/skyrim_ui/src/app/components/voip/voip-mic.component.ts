import { ChangeDetectionStrategy, Component, OnDestroy } from '@angular/core';
import {
  faMicrophone,
  faMicrophoneSlash,
} from '@fortawesome/free-solid-svg-icons';
import { map } from 'rxjs';
import { VoipService } from '../../services/voip.service';

/** "Voip" column header with the local microphone button and its popup: mic volume and noise gate. */
@Component({
  selector: 'app-voip-mic',
  templateUrl: './voip-mic.component.html',
  styleUrls: ['./voip.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class VoipMicComponent implements OnDestroy {
  readonly faMicrophone = faMicrophone;
  readonly faMicrophoneSlash = faMicrophoneSlash;

  isOpen = false;

  readonly settings$ = this.voipService.settings;
  readonly micLevel$ = this.voipService.micLevel;
  readonly state$ = this.voipService.settings.pipe(
    map(settings => {
      if (!settings.connect || !settings.inputDevice) {
        return 'off';
      }
      return settings.micVolume === 0 ? 'muted' : 'on';
    }),
  );

  constructor(private readonly voipService: VoipService) {}

  toggle(): void {
    this.setOpen(!this.isOpen);
  }

  close(): void {
    this.setOpen(false);
  }

  setMicVolume(value: number): void {
    this.voipService.update({ micVolume: Number(value) });
  }

  setNoiseGate(value: number): void {
    this.voipService.update({ noiseGate: Number(value) });
  }

  ngOnDestroy(): void {
    if (this.isOpen) {
      this.voipService.setMeter(false);
    }
  }

  private setOpen(open: boolean): void {
    if (open === this.isOpen) {
      return;
    }
    this.isOpen = open;
    this.voipService.setMeter(open);
  }
}
