import { Component, OnDestroy, OnInit, ViewEncapsulation } from '@angular/core';
import { BehaviorSubject, combineLatestWith, Observable, Subscription, takeUntil, timer } from 'rxjs';
import { map } from 'rxjs/operators';
import { SettingService } from 'src/app/services/setting.service';
import { Group } from '../../models/group';
import { Player } from '../../models/player';
import { ClientService } from '../../services/client.service';
import { DestroyService } from '../../services/destroy.service';
import { GroupService } from '../../services/group.service';
import { animation as popupsAnimation } from '../root/popups.animation';


@Component({
  selector: 'app-group',
  templateUrl: './group.component.html',
  styleUrls: ['./group.component.scss'],
  encapsulation: ViewEncapsulation.None,
  animations: [popupsAnimation],
  providers: [DestroyService],
})
export class GroupComponent implements OnInit, OnDestroy {

  timerSubscription: Subscription;

  groupMembers$: Observable<(Player & { isOwner: boolean })[]>;
  group$: Observable<Group>;

  public isAutoHide = new BehaviorSubject(true);
  public isShown = new BehaviorSubject(true);
  public waitLaunch = new BehaviorSubject(false);

  constructor(
    private readonly destroy$: DestroyService,
    private readonly groupService: GroupService,
    private readonly clientService: ClientService,
    private readonly settings: SettingService,
  ) {
    this.isShown.next(this.settings.isPartyShown());
    this.isAutoHide.next(this.settings.isPartyAutoHidden());

    this.group$ = this.groupService.group.asObservable();
    this.groupMembers$ = this.groupService.selectMembers()
      .pipe(
        combineLatestWith(this.groupService.group),
        map(([members, group]) => {
            if (!group) {
              return [];
            }
            return members
              .map(member => ({ ...member, isOwner: member.id === group.owner }))
              .sort((a, b) => ((group.owner === a.id) === (group.owner === b.id) ? 0 : group.owner === a.id ? -1 : 1));
          },
        ),
      );
  }

  ngOnInit() {
    this.onPartyShownState();
    this.onPartyAutoHideState();

    this.subscribeChangeHealth();
    this.onPartyInfo();
    this.onConnectionState();
    this.subscribeActivation();
  }

  private subscribeActivation() {
    this.clientService.activationStateChange
      .pipe(takeUntil(this.destroy$))
      .subscribe(() => {
        this.flashGroup();
      });
  }

  private subscribeChangeHealth() {
    this.clientService.healthChange
      .pipe(takeUntil(this.destroy$))
      .subscribe(() => {
        this.flashGroup();
      });
  }

  private onPartyInfo() {
    this.clientService.partyInfoChange
      .pipe(takeUntil(this.destroy$))
      .subscribe(() => {
        this.flashGroup();
      });
  }

  private onConnectionState() {
    this.clientService.connectionStateChange
      .pipe(takeUntil(this.destroy$))
      .subscribe((state: boolean) => {
        if (this.isAutoHide.getValue()) {
          if (state) {
            this.flashGroup();
          } else {
            if (this.timerSubscription) {
              this.timerSubscription.unsubscribe();
            }
          }
        }
      });
  }

  private onPartyShownState() {
    this.settings.partyShownChange
      .pipe(takeUntil(this.destroy$))
      .subscribe((state: boolean) => {
        this.isShown.next(state);
        this.isAutoHide.next(false);
        this.flashGroup();
      });
  }

  private onPartyAutoHideState() {
    this.settings.partyAutoHideChange
      .pipe(takeUntil(this.destroy$))
      .subscribe((state: boolean) => {
        this.isAutoHide.next(state);
        if (this.settings.isPartyShown()) {
          this.isShown.next(true);
        }
        this.flashGroup();
      });
  }

  ngOnDestroy() {
    if (this.timerSubscription) {
      this.timerSubscription.unsubscribe();
    }
  }

  private flashGroup() {
    if (this.isAutoHide.getValue() && this.clientService.connectionStateChange.getValue()) {
      if (!this.isShown.getValue()) {
        this.isShown.next(true);
      }

      if (this.timerSubscription) {
        this.timerSubscription.unsubscribe();
      }

      if (!this.clientService.activationStateChange.getValue()) {
        let timerLength = this.settings.getAutoHideTime() * 1000;
        let source = timer(timerLength);
        this.timerSubscription = source.subscribe(() => {
          this.isShown.next(false);
        });
      }
    }
  }

  public invite(playerId: number) {
    this.groupService.invite(playerId);
  }

  public accept(inviterId: number) {
    return this.groupService.accept(inviterId);
  }

  public kick(playerId: number) {
    return this.groupService.kick(playerId);
  }

  public changeLeader(playerId: number) {
    return this.groupService.changeLeader(playerId);
  }

}
