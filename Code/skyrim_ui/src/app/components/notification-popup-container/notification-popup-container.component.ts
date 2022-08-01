import { ChangeDetectionStrategy, ChangeDetectorRef, Component, OnInit } from '@angular/core';
import { takeUntil } from 'rxjs';
import { fadeInOutAnimation } from '../../animations/fade-in-out.animation';
import { PopupNotification } from '../../models/popup-notification';
import { DestroyService } from '../../services/destroy.service';
import { GroupService } from '../../services/group.service';
import { PopupNotificationService } from '../../services/popup-notification.service';


@Component({
  selector: 'app-notification-popup-container',
  templateUrl: './notification-popup-container.component.html',
  styleUrls: ['./notification-popup-container.component.scss'],
  animations: [fadeInOutAnimation],
  providers: [DestroyService],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class NotificationPopupContainerComponent implements OnInit {

  notifications: PopupNotification[] = [];

  constructor(
    private readonly destroy$: DestroyService,
    private readonly groupService: GroupService,
    private readonly popupNotificationService: PopupNotificationService,
    private readonly cdr: ChangeDetectorRef,
  ) {
  }

  ngOnInit() {
    this.popupNotificationService.message$
      .pipe(takeUntil(this.destroy$))
      .subscribe((notification: PopupNotification) => {
        this.create(notification);
      });
    this.popupNotificationService.messagesCleared$
      .pipe(takeUntil(this.destroy$))
      .subscribe(() => {
        this.removeAll();
      });
  }

  create(notification: PopupNotification) {
    this.notifications = [...this.notifications, notification];
    this.cdr.detectChanges();
  }

  remove(notification: PopupNotification, userAction: boolean) {
    notification.onClose.next(userAction);
    notification.onClose.complete();

    const index = this.notifications.indexOf(notification);
    this.notifications.splice(index, 1);
    this.notifications = [...this.notifications];

    this.cdr.detectChanges();
  }

  removeAll() {
    for (const notification of this.notifications) {
      notification.onClose.next(false);
      notification.onClose.complete();
    }
    this.notifications = [];
    this.cdr.detectChanges();
  }

}
