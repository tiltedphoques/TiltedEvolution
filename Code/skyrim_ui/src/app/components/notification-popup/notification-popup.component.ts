import { Component, EventEmitter, Input, OnDestroy, OnInit, Output } from '@angular/core';
import { NotificationType, PopupNotification } from '../../models/popup-notification';
import { DestroyService } from '../../services/destroy.service';


@Component({
  selector: 'app-notification-popup',
  templateUrl: './notification-popup.component.html',
  styleUrls: ['./notification-popup.component.scss'],
  providers: [DestroyService],
})
export class NotificationPopupComponent implements OnInit, OnDestroy {

  eraseTimer: number | null = null;
  eraseTimingStart?: number;
  eraseTTL: number;

  @Input() notification: PopupNotification;
  @Output() remove = new EventEmitter<boolean>();

  ngOnInit() {
    this.eraseTTL = this.notification.duration ?? 5000;
    if (this.eraseTTL !== 0) {
      this.startTimer();
    }
  }

  ngOnDestroy() {
    this.stopTimer();
  }

  onEnter() {
    if (this.eraseTTL !== 0) {
      this.eraseTTL -= Date.now() - this.eraseTimingStart!;
      this.stopTimer();
    }
  }

  onLeave() {
    if (this.eraseTTL !== 0) {
      this.startTimer();
    }
  }

  startTimer() {
    this.stopTimer();
    this.eraseTimer = setTimeout(() => this.remove.emit(false), this.eraseTTL);
    this.eraseTimingStart = Date.now();
  }

  private stopTimer(): void {
    if (this.eraseTimer !== null) {
      clearTimeout(this.eraseTimer);
      this.eraseTimer = null;
    }
  }

  get isConnected(): boolean {
    return this.notification.type === NotificationType.Connection;
  }

  async clickNotification() {
    this.remove.emit(true);
  }

}
