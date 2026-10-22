import { Injectable } from '@angular/core';
import {
  faHandshakeSimple,
  faUserGroup,
  faScaleBalanced,
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

  public addTradeInvite(
    from: string,
    acceptCallback: () => void,
    declineCallback: () => void,
  ) {
    this.addMessage({
      messageKey: 'SERVICE.PLAYER_LIST.TRADE_INVITE',
      messageParams: { from },
      icon: faScaleBalanced,
      duration: 30000,
      actions: [
        {
          nameKey: 'COMPONENT.NOTIFICATIONS.ACCEPT',
          callback: acceptCallback,
        },
        {
          nameKey: 'COMPONENT.NOTIFICATIONS.DECLINE',
          callback: declineCallback,
        },
      ],
    });
  }

  public addTradeInviteSent(target: string, cancelCallback: () => void): void {
    this.addMessage({
      messageKey: 'SERVICE.TRADE.INVITE_SENT',
      messageParams: { target },
      icon: faScaleBalanced,
      duration: 10000,
      actions: [
        {
          nameKey: 'COMPONENT.NOTIFICATIONS.CANCEL',
          callback: cancelCallback,
        },
      ],
    });
  }

  public addTradeCancelled(partner: string, reason: string) {
    this.addMessage({
      messageKey: 'SERVICE.TRADE.CANCELLED',
      messageParams: { partner, reason },
      icon: faScaleBalanced,
      duration: 8000,
    });
  }

  public addTradeCompleted(partner: string) {
    this.addMessage({
      messageKey: 'SERVICE.TRADE.COMPLETED',
      messageParams: { partner },
      icon: faScaleBalanced,
      duration: 8000,
    });
  }

  public addPartyHint(onOpenGuide: () => void, onDontShow: () => void) {
    this.addMessage({
      messageKey: 'SERVICE.PARTY_HINT.MESSAGE',
      icon: faUserGroup,
      duration: 15000,
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
