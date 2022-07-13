import { Component, EventEmitter, HostListener, Output } from '@angular/core';
import { ClientService } from '../../services/client.service';
import { Sound, SoundService } from '../../services/sound.service';


@Component({
  selector: 'app-disconnect',
  templateUrl: './disconnect.component.html',
  styleUrls: ['./disconnect.component.scss'],
})
export class DisconnectComponent {

  @Output() public done = new EventEmitter<void>();

  public constructor(
    private readonly client: ClientService,
    private readonly sound: SoundService,
  ) {
  }

  public disconnect(): void {
    this.client.disconnect();
    this.sound.play(Sound.Ok);
    this.done.next();
  }

  public cancel(): void {
    this.sound.play(Sound.Cancel);
    this.done.next();
  }

  @HostListener('window:keydown.escape', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    this.done.next();
    this.sound.play(Sound.Cancel);

    event.stopPropagation();
    event.preventDefault();
  }
}
