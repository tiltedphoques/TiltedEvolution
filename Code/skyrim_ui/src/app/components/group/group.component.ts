import { Component, OnInit, ViewEncapsulation, OnDestroy } from '@angular/core';
import { GroupService } from '../../services/group.service';
import { Subscription, timer } from 'rxjs';
import { Group } from '../../models/group';
import { animation as popupsAnimation } from '../root/popups.animation';
import { ClientService } from '../../services/client.service';
import { Player } from '../../models/player';
import { UserService } from '../../services/user.service';
import { ErrorService } from '../../services/error.service';
import { SoundService, Sound } from '../../services/sound.service';
import { LoadingService } from '../../services/loading.service';
import { StoreService } from '../../services/store.service';


@Component({
  selector: 'app-group',
  templateUrl: './group.component.html',
  styleUrls: ['./group.component.scss'],
  encapsulation: ViewEncapsulation.None,
  animations: [popupsAnimation ]
})
export class GroupComponent implements OnInit, OnDestroy {

  public isAutoHide = true;
  public isShow = true;
  public waitLaunch = false;

  private connectionSubscription: Subscription;
  private isLoading: Subscription;
  private timerSubscription: Subscription;
  private userHealthSubscription: Subscription;
  private playerConnectedSubscription: Subscription;
  private connectionStateSubscription: Subscription;

  constructor(public groupService: GroupService,
              private clientService: ClientService,
              private userService: UserService,
              private errorService: ErrorService,
              private soundService: SoundService,
              private loadingService: LoadingService,
              private storeService: StoreService)
  {
    this.isShow = JSON.parse(this.storeService.get("isShow", "true"));
    this.isAutoHide = JSON.parse(this.storeService.get("isAutoHide", "true"));
  }


  ngOnInit() {

    this.isLoading = this.loadingService.getLoading().subscribe((isLoading: boolean) => {
      this.waitLaunch = isLoading;
    })

    this.connectionSubscription = this.clientService.connectionStateChange.subscribe((state) => {
      if (this.waitLaunch) {
        this.loadingService.setLoading(false);
        this.clientService.messageReception.next({content:"Connected!"});

        if (state) {
          this.soundService.play(Sound.Success);
        }
        else {
          this.soundService.play(Sound.Fail);

          this.errorService.error(
            'Unable to reach the game server. Please try again later.'
          );
        }
      }
    });

    this.subscribeChangeHealth();
    this.onPlayerConnected();
    this.onConnectionState();
  }

  private subscribeChangeHealth() {
    this.userHealthSubscription = this.clientService.healthChange.subscribe((p: Player) => {
      if (this.userService.player.value && p.serverId !== this.userService.player.value.serverId) {
        this.changeUIGroup();
      }
    })
  }

  private onPlayerConnected() {
    this.playerConnectedSubscription = this.clientService.playerConnectedChange.subscribe(() => {
      this.changeUIGroup();
    })
  }

  private onConnectionState() {
    this.connectionStateSubscription = this.clientService.connectionStateChange.subscribe((state: boolean) => {
      if (this.isAutoHide) {
        if (state) {
          this.changeUIGroup();
        }
        else {
          if (this.timerSubscription) {
            this.timerSubscription.unsubscribe();
          }
        }
      }

    })
  }

  ngOnDestroy() {
    this.connectionSubscription.unsubscribe();
    this.isLoading.unsubscribe();
    this.userHealthSubscription.unsubscribe();
    this.playerConnectedSubscription.unsubscribe();
    this.connectionStateSubscription.unsubscribe();

    if (this.timerSubscription) {
      this.timerSubscription.unsubscribe();
    }
  }

  private changeUIGroup() {
    if (this.isAutoHide && this.clientService.connectionStateChange.value) {
      if (!this.isShow) {
        this.isShow = true;
      }

      if (this.timerSubscription) {
        this.timerSubscription.unsubscribe();
      }

      const source = timer(3000);

      this.timerSubscription = source.subscribe(() => {
        console.log("FIN TIMER IS SHOW => FALSE");
        this.isShow = false;
      })
    }
  }

  public get isConnected(): boolean {
    return this.clientService.connectionStateChange.value;
  }

  public get group(): Group | undefined {
    return this.groupService.group.value;
  }

  public get active(): boolean {
    return this.clientService.activationStateChange.value;
  }

  public get player(): Player {
    return this.userService.player.value;
  }

  public leave() {
    this.groupService.leave();
  }

  public clickToggle() {
    this.soundService.play(Sound.Focus);
    this.isShow = !this.isShow;
    this.isAutoHide = false;

    this.storeService.set("isShow", this.isShow);
    this.storeService.set("isAutoHide", this.isAutoHide);
  }

  public clickAutoHide() {
    this.isAutoHide = !this.isAutoHide;
    if (this.isAutoHide) {
      this.changeUIGroup();
    }

    this.storeService.set("isAutoHide", this.isAutoHide);
  }

  isLaunchPartyDisable(): boolean {
    return (this.waitLaunch || (this.groupService.getSizeMembers() < 1));
  }

  public launchParty() {
    this.soundService.play(Sound.Focus);
    this.loadingService.setLoading(true);
    this.clientService.messageReception.next({content: "Your session is being created..."});
    this.groupService.launch().subscribe(
      () => {},
      () => {
        this.errorService.error('Failed to create a session. Please try again later.');
        this.loadingService.setLoading(false);
      }
    )
  }
}
