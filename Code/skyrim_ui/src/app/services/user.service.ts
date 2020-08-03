import { Injectable, OnDestroy } from '@angular/core';

import { BehaviorSubject, Subscription, Observable } from 'rxjs';

import { WebSocketService } from './web-socket.service';
import { Player, Friend } from '../models/player';
import { ErrorService } from './error.service';
import { environment } from '../../environments/environment';
import { User } from '../models/user';
import { PopupNotificationService } from './popup-notification.service';
import { NotificationType } from '../models/popup-notification';

/** User service. */
@Injectable()
export class UserService implements OnDestroy {
  /** User data update. */
  public user = new BehaviorSubject<User | undefined>(undefined);

  /** Player date update. */
  public player = new BehaviorSubject<Player>(new Player());
  private wsSubscription: Subscription;

  private _token: string;

  /**
   * Instantiate.
   *
   * @param api API service.
   */
  public constructor(private wsService: WebSocketService,
                     private popupNotificationService: PopupNotificationService,
                     private errorService: ErrorService) { }

  /**
   * Dispose.
   */
  public ngOnDestroy(): void {
    if (this.wsSubscription) {
      this.wsSubscription.unsubscribe();
    }
    this.logout();
  }

  /**
   * Normal login through the launcher
   *
   * @param token
   */
  public login(token: string, username: string) {
    this._token = token;

    this.wsSubscription = this.wsService.message.subscribe(
      (msg) => {

        if (msg.operation === "token") {

          if (!msg.status && environment.production && !environment.nightlyBuild) {

            if (msg.error === "user_already_online") {
              this.errorService.error("You are already connected, please restart the game using the launcher.")
              this.wsSubscription.unsubscribe();
            }

            if (msg.error === "invalid_token") {
              this.errorService.error("You have been disconnected, please restart the game using the launcher.");
              this.wsSubscription.unsubscribe();
            }
          }
        }

        else if (msg.operation === "friends") {

          let friends: Player[] = [];

          msg.friends.forEach((dataFriends: Friend) => {
            friends.push(new Player(dataFriends));
          });

          this.user.next(new User({
            token: this._token,
            friends: friends
          }));

          this.player.next(new Player({name: username, online: true}));
        }

        else if (msg.operation === "user_status") {

          if (this.user.value) {
            let content: string;
            const user = this.user.value.friends.find((player: Player) => player.id === msg.id);

            if (user) {
              // Update the player status and send the event to update the UI
              user.online = msg.online;

              this.user.next(this.user.value);

              if (msg.online) {
                content = `${user.name} is now online`;
              }
              else {
                content = `${user.name} is now offline`;
              }

              this.popupNotificationService.setMessage(content, NotificationType.Connection, user);
            }

          }
        }

      }
    )

    if (token) {
      this.wsService.send({operation:'token',token: token}).subscribe(
        () => {},
        () => {
          if (environment.production && !environment.nightlyBuild) {
            this.errorService.error("There has been a problem logging in, please restart the game using the launcher.");
          }
        }
      );
    }
  }

  /**
   * Log out.
   */
  public logout(): void {
    this.user.next(undefined);
  }

  public refreshFriends(): Observable<any> {
    return this.wsService.send({operation: 'refresh'});
  }
}
