import { Injectable } from '@angular/core';
import {
  faHandshakeSimple,
  faUserGroup,
} from '@fortawesome/free-solid-svg-icons';
import { Subject } from 'rxjs';
import { PopupNotification } from '../models/popup-notification';
import { Sound, SoundService } from './sound.service';

@Injectable({
  providedIn: 'root',
})
export class PopupNotificationService {
  private message = new Subject<PopupNotification>();
  public message$ = this.message.asObservable();
  private messagesCleared = new Subject<void>();
  public messagesCleared$ = this.messagesCleared.asObservable();

  constructor(private readonly soundService: SoundService) {}

  public addMessage(notification: PopupNotification) {
    this.message.next(notification);
    this.soundService.play(Sound.Focus);
  }

  public addPartyInvite(from: string, callback: () => void) {
    this.addMessage({
      messageKey: 'SERVICE.PLAYER_LIST.PARTY_INVITE',
      messageParams: { from },
      icon: faHandshakeSimple,
      duration: 30000,
      actions: [
        {
          nameKey: 'COMPONENT.NOTIFICATIONS.ACCEPT',
          callback,
        },
      ],
    });
  }

  public addPartyHint(onOpenGuide: () => void, onDontShow: () => void) {
    this.addMessage({
      messageKey: 'SERVICE.PARTY_HINT.MESSAGE',
      icon: faUserGroup,
      duration: 10000,
      actions: [
        {
          nameKey: 'COMPONENT.NOTIFICATIONS.OPEN_GUIDE',
          callback: onOpenGuide,
        },
        {
          nameKey: 'COMPONENT.NOTIFICATIONS.DISMISS',
          // Clicking anywhere on the popup removes it
          callback: () => {},
        },
        {
          nameKey: 'COMPONENT.NOTIFICATIONS.DONT_SHOW',
          callback: onDontShow,
        },
      ],
    });
  }

  public clearMessages() {
    this.messagesCleared.next();
  }
}
