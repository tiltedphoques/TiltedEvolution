import { Component, HostListener, OnInit } from '@angular/core';
import { takeUntil } from 'rxjs';
import { environment } from '../../../environments/environment';
import { fadeInOutAnimation } from '../../animations/fade-in-out.animation';
import { Player } from '../../models/player';
import { NotificationType, PopupNotification } from '../../models/popup-notification';
import { DestroyService } from '../../services/destroy.service';
import { GroupService } from '../../services/group.service';
import { PopupNotificationService } from '../../services/popup-notification.service';
import { animation as popupTestAnimation } from '../popup/popup.animation';


@Component({
  selector: 'app-notification-popup',
  templateUrl: './notification-popup.component.html',
  styleUrls: ['./notification-popup.component.scss'],
  animations: [fadeInOutAnimation, popupTestAnimation],
  providers: [DestroyService],
})
export class NotificationPopupComponent implements OnInit {

  isActive = false;
  notification: PopupNotification;

  constructor(
    private readonly destroy$: DestroyService,
    private readonly groupService: GroupService,
    private readonly popupNotificationService: PopupNotificationService,
  ) {
  }

  ngOnInit() {
    this.popupNotificationService.message
      .pipe(takeUntil(this.destroy$))
      .subscribe((notification: PopupNotification) => {
        this.notification = notification;
        this.isActive = (notification.message !== '');
      });
  }

  get isConnected(): boolean {
    return this.notification.type === NotificationType.Connection;
  }

  async clickNotification() {
    this.isActive = false;
    if (this.notification.type === NotificationType.Invitation) {
      if (this.notification.player) {
        await this.groupService.accept(this.notification.player.id);

        this.popupNotificationService.clearMessage();
      }
    }
  }

  @HostListener('window:keydown.n', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    // Just for deskop
    if (!environment.game) {
      this.notification = {
        message: 'test notif',
        player: new Player({
          name: 'Dumbeldor',
          online: true,
          avatar: 'https://skyrim-together.com/images/float/avatars/random3.jpg',
        }),
        type: NotificationType.Connection,
      };
      this.isActive = !this.isActive;
    }
  }

}
