import { Component, OnInit, ViewEncapsulation, OnDestroy, setTestabilityGetter } from '@angular/core';
import { GroupService } from '../../services/group.service';
import { Subscription, timer } from 'rxjs';
import { Group } from '../../models/group';
import { animation as popupsAnimation } from '../root/popups.animation';
import { ClientService } from '../../services/client.service';
import { Player } from '../../models/player';
import { ErrorService } from '../../services/error.service';
import { SoundService, Sound } from '../../services/sound.service';
import { LoadingService } from '../../services/loading.service';
import { SettingService } from 'src/app/services/setting.service';
import { PlayerListService } from 'src/app/services/player-list.service';


@Component({
  selector: 'app-group',
  templateUrl: './group.component.html',
  styleUrls: ['./group.component.scss'],
  encapsulation: ViewEncapsulation.None,
  animations: [popupsAnimation]
})
export class GroupComponent implements OnInit, OnDestroy {

  public isAutoHide = true;
  public isShown = true;
  public waitLaunch = false;

  private connectionSubscription: Subscription;
  private isLoading: Subscription;
  private timerSubscription: Subscription;
  private userHealthSubscription: Subscription;
  private connectionStateSubscription: Subscription;
  private partyInfoSubscription: Subscription;

  private partyShownSubscription: Subscription;
  private partyAutoHideSubscription: Subscription;

  private activationSubscription: Subscription;

  constructor(public groupService: GroupService,
    private clientService: ClientService,
    private errorService: ErrorService,
    private soundService: SoundService,
    private loadingService: LoadingService,
    private settings: SettingService,
    private playerListService: PlayerListService) {
    this.isShown = this.settings.isPartyShown();
    this.isAutoHide = this.settings.isPartyAutoHidden();
  }


  ngOnInit() {

    //TODO Delete? - i see no reason keeping this

    // this.isLoading = this.loadingService.getLoading().subscribe((isLoading: boolean) => {
    //   this.waitLaunch = isLoading;
    // })

    // this.connectionSubscription = this.clientService.connectionStateChange.subscribe((state) => {
    //   if (this.waitLaunch) {
    //     this.loadingService.setLoading(false);
    //     this.clientService.messageReception.next({ content: "Connected!" });

    //     if (state) {
    //       this.soundService.play(Sound.Success);
    //     }
    //     else {
    //       this.soundService.play(Sound.Fail);

    //       this.errorService.error(
    //         'Unable to reach the game server. Please try again later.'
    //       );
    //     }
    //   }
    // });

    this.onPartyShownState();
    this.onPartyAutoHideState();

    this.subscribeChangeHealth();
    this.onPartyInfo();
    this.onConnectionState();
    this.subscribeActivation();
  }

  private subscribeActivation() {
    this.activationSubscription = this.clientService.activationStateChange.subscribe((state: boolean) => {
      this.flashGroup();
    })
  }

  private subscribeChangeHealth() {
    this.userHealthSubscription = this.clientService.healthChange.subscribe((p: Player) => {
      this.flashGroup();
    })
  }

  private onPartyInfo() {
    this.partyInfoSubscription = this.clientService.partyInfoChange.subscribe(() => {
      this.flashGroup();
    })
  }

  private onConnectionState() {
    this.connectionStateSubscription = this.clientService.connectionStateChange.subscribe((state: boolean) => {
      if (this.isAutoHide) {
        if (state) {
          this.flashGroup();
        }
        else {
          if (this.timerSubscription) {
            this.timerSubscription.unsubscribe();
          }
        }
      }
    })
  }

  private onPartyShownState() {
    this.partyShownSubscription = this.settings.partyShownChange.subscribe((state: boolean) => {
      this.isShown = state;
      this.isAutoHide = false;
      this.flashGroup();
    });
  }

  private onPartyAutoHideState() {
    this.partyAutoHideSubscription = this.settings.partyAutoHideChange.subscribe((state: boolean) => {
      this.isAutoHide = state;
      if (this.settings.isPartyShown()) {
        this.isShown = true;
      }
      this.flashGroup();
    })
  }

  ngOnDestroy() {
    this.connectionSubscription.unsubscribe();
    this.isLoading.unsubscribe();
    this.userHealthSubscription.unsubscribe();
    this.partyInfoSubscription.unsubscribe();
    this.connectionStateSubscription.unsubscribe();

    this.partyShownSubscription.unsubscribe();
    this.partyAutoHideSubscription.unsubscribe();

    if (this.timerSubscription) {
      this.timerSubscription.unsubscribe();
    }
  }

  private flashGroup() {
    if (this.isAutoHide && this.clientService.connectionStateChange.value) {
      if (!this.isShown) {
        this.isShown = true;
      }

      if (this.timerSubscription) {
        this.timerSubscription.unsubscribe();
      }

      if (!this.active) {
        let timerLength = this.settings.getAutoHideTime() * 1000;
        let source = timer(timerLength);
        this.timerSubscription = source.subscribe(() => {
          this.isShown = false;
        })
      }
    }
  }

  public get isConnected(): boolean {
    return this.clientService.connectionStateChange.value;
  }

  public get group(): Group | undefined {
    return this.groupService.group.value;
  }

  public get groupMembers(): Array<Player> {
    let members = this.groupService.getMembers();
    return members.sort((a, b) => (this.isOwner(a.id) === this.isOwner(b.id)) ? 0 : this.isOwner(a.id) ? -1 : 1);
  }

  public get getOwner(): Player {
    return this.playerListService.getPlayerById(this.group.owner);
  }

  public isOwner(playerId): boolean {
    return this.group.owner === playerId;
  }

  public get active(): boolean {
    return this.clientService.activationStateChange.value;
  }

  isLeaveDisabled(): boolean {
    return (this.waitLaunch || !this.groupService.isPartyEnabled());
  }

  public invite(playerId: number) {
    this.groupService.invite(playerId);
  }

  public accept(inviterId: number) {
    this.groupService.accept(inviterId);
  }

  public kick(playerId: number) {
    this.groupService.kick(playerId);
  }

  public changeLeader(playerId: number) {
    this.groupService.changeLeader(playerId);
  }

  public getLeaderName(): string {
    return this.groupService.getLeaderName();
  }
}
