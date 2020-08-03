import {
  Component, ViewEncapsulation, Output, EventEmitter, HostListener
} from '@angular/core';

import { SoundService, Sound } from '../../services/sound.service';
import { ClientService } from '../../services/client.service';

@Component({
  selector: 'app-disconnect',
  templateUrl: './disconnect.component.html',
  styleUrls: [ './disconnect.component.scss' ],
  encapsulation: ViewEncapsulation.None
})
export class DisconnectComponent {
  @Output()
  public done = new EventEmitter();

  public constructor(private client: ClientService, private sound: SoundService) {}

  public disconnect(): void {
    this.client.disconnect();
    this.sound.play(Sound.Ok);
    this.done.next();
  }

  public cancel(): void {
    this.sound.play(Sound.Cancel);
    this.done.next();
  }

  @HostListener('window:keydown.escape', [ '$event' ])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    this.done.next();

    event.stopPropagation();
    event.preventDefault();
  }
}
