import {
  Component, ViewEncapsulation, OnDestroy
} from '@angular/core';

import { Subscription } from 'rxjs';

import { ClientService } from '../../services/client.service';

interface Notification {
  content: string;
  timer: number;
}

@Component({
  selector: 'app-notifications',
  templateUrl: './notifications.component.html',
  styleUrls: [ './notifications.component.scss' ],
  encapsulation: ViewEncapsulation.None
})
export class NotificationsComponent implements OnDestroy {
  public notifications = [] as Notification[];

  public constructor(private client: ClientService) {
    this.messageSubscription = this.client.messageReception.subscribe(message => {
      this.notifications.push({
        content: `${message.name ? `Message from ${message.name}: ` : ''}${message.content}`,
        timer: setTimeout(() => {
          this.notifications.shift();
        }, 5000)
      });

      if (this.notifications.length > 5) {
        for (let notification of this.notifications.splice(0, this.notifications.length - 5)) {
          clearTimeout(notification.timer);
        }
      }
    })
  }

  public ngOnDestroy(): void {
    this.messageSubscription.unsubscribe();
  }

  private messageSubscription: Subscription;
}
