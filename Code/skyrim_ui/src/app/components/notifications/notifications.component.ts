import { Component, ViewEncapsulation } from '@angular/core';
import { takeUntil } from 'rxjs';
import { DestroyService } from '../../services/asset/destroy.service';
import { ClientService } from '../../services/client.service';


interface Notification {
  content: string;
  timer: number;
}

@Component({
  selector: 'app-notifications',
  templateUrl: './notifications.component.html',
  styleUrls: ['./notifications.component.scss'],
  encapsulation: ViewEncapsulation.None,
  providers: [DestroyService],
})
export class NotificationsComponent {

  public notifications = [] as Notification[];

  public constructor(
    private readonly destroy$: DestroyService,
    private readonly client: ClientService,
  ) {
    this.client.messageReception
      .pipe(takeUntil(this.destroy$))
      .subscribe(message => {
        this.notifications.push({
          content: `${ message.name ? `Message from ${ message.name }: ` : '' }${ message.content }`,
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
