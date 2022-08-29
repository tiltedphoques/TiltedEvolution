import { Component } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { takeUntil, withLatestFrom } from 'rxjs';
import { ChatService } from 'src/app/services/chat.service';
import { DestroyService } from '../../services/destroy.service';


interface Notification {
  content: string;
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
    private readonly translocoService: TranslocoService,
  ) {
    this.chatService.messageList
      .pipe(
        takeUntil(this.destroy$),
        withLatestFrom(this.translocoService.selectTranslate<string>('COMPONENT.NOTIFICATIONS.MESSAGE_FROM')),
      )
      .subscribe(([message, text]) => {
        this.notifications.push({
          content: `${ message.senderName ? `${ text } ${ message.senderName }: ` : '' }${ message.content }`,
          timer: setTimeout(() => {
            this.notifications.shift();
          }, 5000),
        });

        if (this.notifications.length > 5) {
          for (let notification of this.notifications.splice(0, this.notifications.length - 5)) {
            clearTimeout(notification.timer);
          }
        }
      });
  }

}
