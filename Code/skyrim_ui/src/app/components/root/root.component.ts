import { Component, HostListener, OnInit, ViewChild } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { takeUntil } from 'rxjs';
import { PartyInfo } from 'src/app/models/party-info';
import { environment } from '../../../environments/environment';
import { fadeInOutActiveAnimation } from '../../animations/fade-in-out-active.animation';
import { Player } from '../../models/player';
import { View } from '../../models/view.enum';
import { ClientService } from '../../services/client.service';
import { DestroyService } from '../../services/destroy.service';
import { SettingService } from '../../services/setting.service';
import { Sound, SoundService } from '../../services/sound.service';
import { UiRepository } from '../../store/ui.repository';
import { ChatComponent } from '../chat/chat.component';
import { GroupComponent } from '../group/group.component';
import { controlsAnimation } from './controls.animation';
import { notificationsAnimation } from './notifications.animation';


@Component({
  selector: 'app-root',
  templateUrl: './root.component.html',
  styleUrls: ['./root.component.scss'],
  animations: [controlsAnimation, fadeInOutActiveAnimation, notificationsAnimation],
  host: { 'data-app-root-game': environment.game.toString() },
  providers: [DestroyService],
})
export class RootComponent implements OnInit {

  /* ### ENUMS ### */
  readonly RootView = View;

  view$ = this.uiRepository.view$;

  connected$ = this.client.connectionStateChange.asObservable();
  menuOpen$ = this.client.openingMenuChange.asObservable();
  inGame$ = this.client.inGameStateChange.asObservable();
  active$ = this.client.activationStateChange.asObservable();
  connectionInProgress$ = this.client.isConnectionInProgressChange.asObservable();

  @ViewChild('chat') private chatComp!: ChatComponent;
  @ViewChild(GroupComponent) private groupComponent: GroupComponent;

  public constructor(
    private readonly destroy$: DestroyService,
    private readonly client: ClientService,
    private readonly sound: SoundService,
    private readonly uiRepository: UiRepository,
    private readonly translocoService: TranslocoService,
    private readonly settingService: SettingService,
  ) {
    this.translocoService.setActiveLang(this.settingService.getLanguage());
  }

  public ngOnInit(): void {
    this.onInGameStateSubscription();
    this.onActivationStateSubscription();
  }

  public onInGameStateSubscription() {
    this.client.inGameStateChange
      .pipe(takeUntil(this.destroy$))
      .subscribe(state => {
        if (!state) {
          this.closeView();
        }
      });
  }

  public onActivationStateSubscription() {
    this.client.activationStateChange
      .pipe(takeUntil(this.destroy$))
      .subscribe(state => {
        if (this.client.inGameStateChange.getValue() && state && !this.uiRepository.isViewOpen()) {
          setTimeout(() => this.chatComp.focus(), 100);
        }
        if (!state) {
          this.closeView();
        }
      });
  }

  public setView(view: View | null) {
    this.uiRepository.openView(view);

    if (view) {
      this.sound.play(Sound.Focus);
    } else if (this.chatComp) {
      this.chatComp.focus();
    }
  }

  public closeView() {
    this.uiRepository.openView(null);
  }

  public reconnect(): void {
    this.client.reconnect();
  }

  @HostListener('window:keydown.escape', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    if (!this.uiRepository.isViewOpen()) {
      if (environment.game) {
        this.client.deactivate();
      } else {
        const active = this.client.activationStateChange.getValue();
        this.client.activationStateChange.next(!active);

        if (!this.client.connectionStateChange.getValue()) {

          this.client.connectionStateChange.next(true);

          this.client.playerConnectedChange.next(new Player(
            {
              id: 1,
              name: 'Dumbeldor',
              online: true,
              connected: true,
              level: 10,
              cellName: 'Falkreath',
              hasInvitedLocalPlayer: true,
            },
          ));
          this.client.playerConnectedChange.next(new Player(
            {
              id: 2,
              name: 'Pokang',
              online: true,
              connected: true,
              level: 12,
              cellName: 'Whiterun',
            },
          ));
          this.client.playerConnectedChange.next(new Player(
            {
              id: 3,
              name: 'Cosideci',
              online: true,
              connected: true,
              level: 69,
              cellName: 'Whiterun',
            },
          ));
          this.client.isLoadedChange.next(new Player(
            {
              id: 1,
              isLoaded: true,
              health: 50,
            },
          ));
          this.client.isLoadedChange.next(new Player(
            {
              id: 2,
              isLoaded: true,
              health: 75,
            },
          ));
          this.client.isLoadedChange.next(new Player(
            {
              id: 3,
              isLoaded: true,
              health: 0,
            },
          ));

          this.client.partyInfoChange.next(new PartyInfo(
            {
              playerIds: [1, 2],
              leaderId: 0,
            },
          ));

          this.client.localPlayerId = 0;

          let name = 'Banana';
          let message = 'Hello Guys';
          let dialogue = true;

          this.client.messageReception.next({ name, content: message, dialogue: dialogue });
        }
      }
    }

    event.stopPropagation();
    event.preventDefault();
  }


  @HostListener('window:keydown.f3', ['$event'])
  // @ts-ignore
  private testGroup(event: KeyboardEvent): void {
    if (!this.uiRepository.isViewOpen()) {
      if (environment.game) {
        this.client.deactivate();
      } else {
        this.client.partyInfoChange.next(new PartyInfo(
          {
            playerIds: [1],
            leaderId: 1,
          },
        ));
      }
    }
    event.stopPropagation();
    event.preventDefault();
  }

  @HostListener('window:keydown.f4', ['$event'])
  // @ts-ignore
  private testUnload(event: KeyboardEvent): void {
    if (!this.uiRepository.isViewOpen()) {
      if (environment.game) {
        this.client.deactivate();
      } else {
        this.client.isLoadedChange.next(new Player(
          {
            id: 2,
            isLoaded: false,
            health: 50,
          },
        ));
      }
    }
    event.stopPropagation();
    event.preventDefault();
  }

  updateGroupPosition() {
    this.groupComponent?.updatePosition();
  }
}
