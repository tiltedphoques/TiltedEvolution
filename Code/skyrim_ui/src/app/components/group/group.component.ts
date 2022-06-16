import { Component, OnInit, ViewEncapsulation, OnDestroy } from '@angular/core';
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

  constructor(public groupService: GroupService,
    private clientService: ClientService,
    private errorService: ErrorService,
    private soundService: SoundService,
    private loadingService: LoadingService,
    private settings: SettingService) {
    this.isShown = this.settings.isPartyShown();
    this.isAutoHide = this.settings.isPartyAutoHidden();
  }


  ngOnInit() {

    this.isLoading = this.loadingService.getLoading().subscribe((isLoading: boolean) => {
      this.waitLaunch = isLoading;
    })

    this.connectionSubscription = this.clientService.connectionStateChange.subscribe((state) => {
      if (this.waitLaunch) {
        this.loadingService.setLoading(false);
        this.clientService.messageReception.next({ content: "Connected!" });

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

    this.onPartyShownState();
    this.onPartyAutoHideState();

    this.subscribeChangeHealth();
    this.onPartyInfo();
    this.onConnectionState();
  }

  private subscribeChangeHealth() {
    this.userHealthSubscription = this.clientService.healthChange.subscribe((p: Player) => {
      if (this.clientService.localServerId !== p.serverId) {
        this.changeUIGroup();
      }
    })
  }
  
  private onPartyInfo() {
    this.partyInfoSubscription = this.clientService.partyInfoChange.subscribe(() => {
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

  private onPartyShownState() {
    this.partyShownSubscription = this.settings.partyShownChange.subscribe((state: boolean) => {
      console.log("partyshow");
      this.isShown = state;
      this.isAutoHide = false;
    });
  }

  private onPartyAutoHideState() {
    this.partyAutoHideSubscription = this.settings.partyAutoHideChange.subscribe((state: boolean) => {
      console.log("partyAuto");
      this.isAutoHide = state;
      if (this.isAutoHide) {
        this.changeUIGroup();
      }
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

  private changeUIGroup() {
    if (this.isAutoHide && this.clientService.connectionStateChange.value) {
      if (!this.isShown) {
        this.isShown = true;
      }

      if (this.timerSubscription) {
        this.timerSubscription.unsubscribe();
      }

      const source = timer(3000);

      this.timerSubscription = source.subscribe(() => {
        console.log("FIN TIMER IS SHOW => FALSE");
        this.isShown = false;
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

  isLaunchPartyDisable(): boolean {
    return (this.waitLaunch /*|| (this.groupService.getSizeMembers() < 1)*/);
  }

  public launchParty() {
    this.groupService.launch();
  }

  public leave() {
    this.groupService.leave();
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
}
