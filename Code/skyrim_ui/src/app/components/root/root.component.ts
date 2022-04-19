import {
  Component, ViewEncapsulation, HostListener, ViewChild,
  OnDestroy,
  OnInit
} from '@angular/core';

import { Subscription } from 'rxjs';

import { ClientService } from '../../services/client.service';
import { UserService } from '../../services/user.service';
import { SoundService, Sound } from '../../services/sound.service';

import { ChatComponent } from '../chat/chat.component';

import { animation as controlsAnimation } from './controls.animation';
import { animation as notificationsAnimation } from './notifications.animation';
import { animation as popupsAnimation } from './popups.animation';

import { environment } from '../../../environments/environment';
import { User } from '../../models/user';
import { Player } from '../../models/player';

@Component({
  selector: 'app-root',
  templateUrl: './root.component.html',
  styleUrls: [ './root.component.scss' ],
  encapsulation: ViewEncapsulation.None,
  animations: [ controlsAnimation, notificationsAnimation, popupsAnimation ],
  host: { 'data-app-root-game': environment.game.toString() }
})
export class RootComponent implements OnInit, OnDestroy {

  @ViewChild('chat')
  private chatComp!: ChatComponent;

  private _view?: string;

  private activationSubscription: Subscription;
  private gameSubscription: Subscription;
  private activationStateChangeSubscription: Subscription;

  public constructor(
    private client: ClientService,
    private user: UserService,
    private sound: SoundService
  ) {
  }

  public ngOnInit(): void {
    this.activationSubscription = this.client.activationStateChange.subscribe(state => {
      if (this.inGame && state && !this.view) {
        setTimeout(() => this.chatComp.focus(), 100);
      }
    });

    this.gameSubscription = this.client.inGameStateChange.subscribe(state => {
      if (!state) {
        this.view = undefined;
      }
    });

    this.onActivationStateSubscription();
  }

  public onActivationStateSubscription() {
    this.activationStateChangeSubscription = this.client.activationStateChange.subscribe((state: boolean) => {
      if (!state) {
        this.view = undefined;
      }
    })
  }

  public ngOnDestroy(): void {
    this.activationSubscription.unsubscribe();
    this.gameSubscription.unsubscribe();
    this.activationStateChangeSubscription.unsubscribe();
  }

  public get currentUser(): User | undefined {
    return this.user.user.value;
  }

  public get openingMenu(): boolean {
    return this.client.openingMenuChange.value;
  }

  public get connected(): boolean {
    return this.client.connectionStateChange.value;
  }

  public get active(): boolean {
    return this.client.activationStateChange.value;
  }

  public get version(): string {
    return this.client.versionSet.value;
  }

  public get inGame(): boolean {
    return this.client.inGameStateChange.value;
  }

  public get isConnectionInProgress(): boolean {
    return this.client.isConnectionInProgressChange.value;
  }

  public set view(view: string | undefined) {
    this._view = view;

    if (view) {
      this.sound.play(Sound.Focus);
    }
    else if (this.chatComp) {
      this.chatComp.focus();
    }
  }

  public get view(): string | undefined {
    return this._view;
  }

  public get isProduction(): boolean {
    return environment.production;
  }

  public get isnightly(): boolean {
    return environment.nightlyBuild;
  }

  public reconnect(): void {
    this.client.reconnect();
  }

  @HostListener('window:keydown.escape', [ '$event' ])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    if (!this.view) {
      if (environment.game) {
        this.client.deactivate();
      }
      else {
        this.user.login('1', 'Dumbeldor');
        this.client.playerConnectedChange.next(new Player(
          {
            serverId: 1,
            name: 'Dumbeldor',
            online: true,
            connected: true,
            level: 10,
            health: 75
          }
        ));
        this.client.playerConnectedChange.next(new Player(
          {
            serverId: 2,
            name: 'Pokang',
            online: true,
            connected: true,
            level: 12
          }
        ));
        this.client.isLoadedChange.next(new Player(
          {
            serverId: 1,
            isLoaded: true
          }
        ));
        this.client.activationStateChange.next(!this.active);

        let name = "Banana";
        let message = "Hello Guys";
        let whisper = true;

        this.client.messageReception.next({ name, content: message, whisper})
      }
    }

    event.stopPropagation();
    event.preventDefault();
  }
}
