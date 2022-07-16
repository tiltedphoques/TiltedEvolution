import { Injectable, OnDestroy } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { firstValueFrom, Observable, Subject, Subscription, timer } from 'rxjs';
import { Player } from '../models/player';
import { NotificationType, PopupNotification } from '../models/popup-notification';
import { Sound, SoundService } from './sound.service';


@Injectable({
  providedIn: 'root',
})
export class PopupNotificationService implements OnDestroy {

  private messageSubject = new Subject<PopupNotification>();
  private timerSubscription: Subscription;

  constructor(
    private readonly soundService: SoundService,
    private readonly translocoService: TranslocoService
  ) {
  }

  ngOnDestroy() {
    if (this.timerSubscription) {
      this.timerSubscription.unsubscribe();
    }
  }

  public get message(): Observable<PopupNotification> {
    return this.messageSubject.asObservable();
  }

  public async setPartyInviteMessage(invitingPlayer: Player) {
    const msg = await firstValueFrom(
      this.translocoService.selectTranslate<string>('COMPONENT.NOTIFICATIONS.INVITE', 
      { name: invitingPlayer.name}),
    );
    this.setMessage(msg, NotificationType.Invitation, invitingPlayer);
  }

  public setMessage(msg: string, typeNotification: NotificationType, player?: Player) {
    if (this.timerSubscription) {
      this.timerSubscription.unsubscribe();
    }

    this.messageSubject.next({ message: msg, type: typeNotification, player: player });

    this.soundService.play(Sound.Focus);

    const source = timer(3000);

    this.timerSubscription = source.subscribe(() => {
      this.clearMessage();
    });
  }

  public clearMessage() {
    this.messageSubject.next({ message: '', type: NotificationType.Connection, player: undefined });
  }

}
