import { Component } from '@angular/core';
import { filter, takeUntil } from 'rxjs';
import { ChatMessage, ChatService } from 'src/app/services/chat.service';
import { DestroyService } from '../../services/destroy.service';
import { PlayerUiService } from '../../services/player-ui.service';

interface Notification extends ChatMessage {
  timer: number;
}

@Component({
  selector: 'app-notifications',
  templateUrl: './notifications.component.html',
  styleUrls: ['./notifications.component.scss'],
  providers: [DestroyService],
})
export class NotificationsComponent {
  public notifications = [] as Notification[];

  public constructor(
    private readonly destroy$: DestroyService,
    private readonly chatService: ChatService,
    private readonly playerUiService: PlayerUiService,
  ) {
    this.chatService.messageList
      .pipe(
        filter(message => this.playerUiService.showsInMiniChat(message)),
        takeUntil(this.destroy$),
      )
      .subscribe(message => {
        this.notifications.push({
          ...message,
          timer: setTimeout(() => {
            this.notifications.shift();
          }, 5000),
        });

        if (this.notifications.length > 5) {
          for (let notification of this.notifications.splice(
            0,
            this.notifications.length - 5,
          )) {
            clearTimeout(notification.timer);
          }
        }
      });
  }
}
