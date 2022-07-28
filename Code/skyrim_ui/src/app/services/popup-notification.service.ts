import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { PopupNotifactionOptions, PopupNotification } from '../models/popup-notification';
import { Sound, SoundService } from './sound.service';


@Injectable({
  providedIn: 'root',
})
export class PopupNotificationService {

  private message = new Subject<PopupNotification>();
  public message$ = this.message.asObservable();
  private messagesCleared = new Subject<void>();
  public messagesCleared$ = this.messagesCleared.asObservable();

  constructor(
    private readonly soundService: SoundService,
  ) {
  }

  public addMessage(message: string, options: PopupNotifactionOptions) {
    const notification: PopupNotification = {
      message,
      type: options.type,
      player: options.player,
      duration: options.duration ? (options.duration < 0 ? 500 : options.duration) : 5000,
      onClose: new Subject(),
    };
    this.message.next(notification);
    this.soundService.play(Sound.Focus);
    return notification;
  }

  public clearMessages() {
    this.messagesCleared.next();
  }

}
