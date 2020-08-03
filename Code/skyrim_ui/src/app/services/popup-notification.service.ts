import { Injectable, OnDestroy } from '@angular/core';
import { Subject, timer, Subscription, Observable } from 'rxjs';
import { PopupNotification, NotificationType } from '../models/popup-notification';
import { Player } from '../models/player';
import { SoundService, Sound } from './sound.service';

@Injectable({
  providedIn: 'root'
})
export class PopupNotificationService implements OnDestroy {

  private messageSubject = new Subject<PopupNotification>();
  private timerSubscription: Subscription;

  constructor(private soundService: SoundService) { }

  ngOnDestroy() {
    if (this.timerSubscription) {
      this.timerSubscription.unsubscribe();
    }
  }

  public get message(): Observable<PopupNotification> {
    return this.messageSubject.asObservable();
  }

  public setMessage(msg: string, typeNotification: NotificationType, player?: Player) {
    if (this.timerSubscription) {
      this.timerSubscription.unsubscribe();
    }

    this.messageSubject.next({message: msg, type: typeNotification, player: player});

    this.soundService.play(Sound.Focus);

    const source = timer(3000);

    this.timerSubscription = source.subscribe(() => {
      this.messageSubject.next({message: "", type: NotificationType.Connection, player: undefined});
    })
  }

  public clearMessage() {
    this.messageSubject.next({message: ""});
  }
}
