import { Component, ViewEncapsulation, HostListener, OnDestroy, OnInit } from '@angular/core';
import { animation as popupsAnimation } from '../root/popups.animation';
import { animation as popupTestAnimation } from '../popup/popup.animation';
import { Subscription } from 'rxjs';
import { PopupNotificationService } from '../../services/popup-notification.service';
import { PopupNotification, NotificationType } from '../../models/popup-notification';
import { GroupService } from '../../services/group.service';
import { environment } from '../../../environments/environment';
import { Player } from '../../models/player';

@Component({
  selector: 'app-notification-popup',
  templateUrl: './notification-popup.component.html',
  styleUrls: ['./notification-popup.component.scss'],
  encapsulation: ViewEncapsulation.None,
  animations: [ popupsAnimation, popupTestAnimation ]
})
export class NotificationPopupComponent implements OnDestroy, OnInit {

  public isActive = false;
  public notification: PopupNotification;

  private notificationSubscription: Subscription;

  constructor(private groupService: GroupService,
              private popupNotificationService: PopupNotificationService) {}

  ngOnInit() {
    this.notificationSubscription = this.popupNotificationService.message.subscribe((notification: PopupNotification) => {
      this.notification = notification;
      this.isActive = (notification.message !== "");
    });
  }

  ngOnDestroy() {
    this.notificationSubscription.unsubscribe();
  }

  public get isConnected(): boolean {
    return this.notification.type === NotificationType.Connection;
  }

  public clickNotif() {
    this.isActive = false;
    if (this.notification.type === NotificationType.Invitation) {
      if (this.notification.player) {
        this.groupService.accept(this.notification.player.id);

        this.popupNotificationService.clearMessage();
      }
    }
  }

  @HostListener('window:keydown.n', [ '$event' ])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    // Just for deskop
    if (!environment.game) {
      this.notification = {
        message: "test notif",
        player: new Player({
          name: "Dumbeldor",
          online: true,
          avatar: "https://skyrim-together.com/images/float/avatars/random3.jpg"}),
        type: NotificationType.Connection};
      this.isActive = !this.isActive;
    }
  }


}
