import { Injectable, OnDestroy } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { BehaviorSubject, firstValueFrom, Observable, of, pluck, Subscription } from 'rxjs';
import { filter, map } from 'rxjs/operators';
import { Group } from '../models/group';
import { PartyInfo } from '../models/party-info';
import { Player } from '../models/player';
import { ClientService } from './client.service';
import { ChatService } from './chat.service';
import { ErrorService } from './error.service';
import { LoadingService } from './loading.service';
import { PlayerListService } from './player-list.service';
import { Sound, SoundService } from './sound.service';


@Injectable({
  providedIn: 'root',
})
export class GroupService implements OnDestroy {

  public group = new BehaviorSubject<Group | undefined>(undefined);

  private debugSubscription: Subscription;
  private connectionSubscription: Subscription;
  private userHealthSubscription: Subscription;
  private partyInfoSubscription: Subscription;
  private partyLeftSubscription: Subscription;
  private playerDisconnectedSubscription: Subscription;
  private levelSubscription: Subscription;
  private cellSubscription: Subscription;
  private loadedSubscription: Subscription;

  private isConnect = false;

  constructor(
    private readonly errorService: ErrorService,
    private readonly soundService: SoundService,
    private readonly clientService: ClientService,
    private readonly chatService: ChatService,
    private readonly playerListService: PlayerListService,
    private readonly loadingService: LoadingService,
    private readonly translocoService: TranslocoService,
  ) {
    this.onDebug();
    this.onConnectionStateChanged();
    this.subscribeChangeHealth();
    this.onPartyInfo();
    this.onPartyLeft();
    this.onPlayerDisconnected();
    this.onLevelChange();
    this.onCellChange();
    this.onLoadedChange();
  }

  ngOnDestroy() {
    this.debugSubscription.unsubscribe();
    this.connectionSubscription.unsubscribe();
    this.userHealthSubscription.unsubscribe();
    this.partyInfoSubscription.unsubscribe();
    this.partyLeftSubscription.unsubscribe();
    this.playerDisconnectedSubscription.unsubscribe();
    this.levelSubscription.unsubscribe();
    this.cellSubscription.unsubscribe();
    this.loadedSubscription.unsubscribe();
  }

  private onDebug() {
    this.debugSubscription = this.clientService.debugChange.subscribe(() => {
      console.log(this.group);
    });
  }

  private onConnectionStateChanged() {
    this.connectionSubscription = this.clientService.connectionStateChange.subscribe((connect: boolean) => {
      if (this.isConnect == connect) {
        return;
      }

      this.isConnect = connect;

      if (connect) {
        this.createGroup(this.group.getValue());
      } else {
        this.group.next(undefined);
      }

      this.updateGroup();
    });
  }

  private subscribeChangeHealth() {
    this.userHealthSubscription = this.clientService.healthChange.subscribe((player: Player) => {

      const group = this.group.getValue();

      if (group) {
        const foundPlayer = this.playerListService.getPlayerById(player.id);

        if (foundPlayer) {
          foundPlayer.health = player.health;
          this.updateGroup();
        }
      }
    });
  }

  private onPartyInfo() {
    this.partyInfoSubscription = this.clientService.partyInfoChange.subscribe((partyInfo: PartyInfo) => {

      const group = this.createGroup(this.group.getValue());
      const playerList = this.playerListService.getPlayerList();

      if (group && playerList) {
        group.members.splice(0);

        for (let id of partyInfo.playerIds) {
          group.members.push(id);

          for (const player of playerList.players) {
            if (player.id === id) {
              player.hasBeenInvited = false;
              break;
            }
          }
        }

        group.owner = partyInfo.leaderId;
        group.isEnabled = true;

        this.updateGroup();
        this.playerListService.updatePlayerList();
      }
    });
  }

  private onPartyLeft() {
    this.partyLeftSubscription = this.clientService.partyLeftChange.subscribe(() => {
      const group = this.createGroup(this.group.getValue());

      if (group) {
        this.playerListService.resetHasBeenInvitedFlags();

        group.isEnabled = false;
        group.owner = undefined;
        group.members.splice(0);

        this.updateGroup();
      }
    });
  }

  private onPlayerDisconnected() {
    this.playerDisconnectedSubscription = this.clientService.playerDisconnectedChange.subscribe((player: Player) => {
      const group = this.createGroup(this.group.getValue());

      if (group) {

        group.members = group.members.filter(playerId => playerId !== player.id);

        this.group.next(group);
      }
    });
  }

  private onLevelChange() {
    this.levelSubscription = this.clientService.levelChange.subscribe(async (player: Player) => {
      const group = this.createGroup(this.group.getValue());

      if (group) {
        const p = this.playerListService.getPlayerById(player.id);
        if (p) {
          p.level = player.level;
          this.chatService.pushSystemMessage('SERVICE.GROUP.LEVEL_UP', { name: p.name, level: player.level });
        }
      }
    });
  }

  private onCellChange() {
    this.cellSubscription = this.clientService.cellChange.subscribe((player: Player) => {
      const group = this.createGroup(this.group.getValue());

      if (group) {
        const p = this.playerListService.getPlayerById(player.id);
        if (p) {
          p.cellName = player.cellName;
        }
      }
    });
  }

  private onLoadedChange() {
    this.loadedSubscription = this.clientService.isLoadedChange.subscribe((player: Player) => {
      const group = this.createGroup(this.group.getValue());

      if (group) {
        const p = this.playerListService.getPlayerById(player.id);
        if (p) {
          p.isLoaded = player.isLoaded;
          p.health = player.health;
        }
      }
    });
  }


  public launch() {
    const group = this.createGroup(this.group.getValue());

    if (group) {
      this.soundService.play(Sound.Focus);
      this.loadingService.setLoading(true);
      this.clientService.launchParty();

      group.isEnabled = true;
      this.updateGroup();
    }
  }

  public leave() {
    const group = this.createGroup(this.group.getValue());

    if (group) {
      if (!group.isEnabled) {
        return;
      }

      this.soundService.play(Sound.Ok);
      this.clientService.leaveParty();

      group.isEnabled = false;
      group.owner = undefined;
      group.members.splice(0);

      this.updateGroup();
    }
  }

  public invite(playerId: number) {
    this.soundService.play(Sound.Ok);
    this.clientService.createPartyInvite(playerId);
  }

  async accept(inviterId: number) {
    const group = this.createGroup(this.group.getValue());

    if (group) {
      if (group.owner || group.members.length > 0) {
        const message = await firstValueFrom(
          this.translocoService.selectTranslate<string>('SERVICE.GROUP.ALREADY_IN_GROUP'),
        );
        await this.errorService.setError(message);
        return;
      }

      this.soundService.play(Sound.Ok);

      this.clientService.acceptPartyInvite(inviterId);
    }
  }

  async kick(playerId: number) {
    const group = this.createGroup(this.group.getValue());

    if (group) {
      if (group.owner !== this.clientService.localPlayerId) {
        const message = await firstValueFrom(
          this.translocoService.selectTranslate<string>('SERVICE.GROUP.KICK_NO_PARTY_LEADER'),
        );
        await this.errorService.setError(message);
        return;
      }

      this.soundService.play(Sound.Ok);

      this.clientService.kickPartyMember(playerId);
    }
  }

  async changeLeader(playerId: number) {
    const group = this.createGroup(this.group.getValue());

    if (group) {
      if (group.owner !== this.clientService.localPlayerId) {
        const message = await firstValueFrom(
          this.translocoService.selectTranslate<string>('SERVICE.GROUP.MAKE_LEADER_NO_PARTY_LEADER'),
        );
        await this.errorService.setError(message);
        return;
      }

      this.soundService.play(Sound.Ok);

      this.clientService.changePartyLeader(playerId);
    }
  }

  public selectMembers(): Observable<Player[]> {
    if (this.group) {
      return this.playerListService.playerList
        .asObservable()
        .pipe(
          filter(playerlist => !!playerlist),
          pluck('players'),
          map(players => players.filter(player => this.group.getValue().members.includes(player.id))),
        );
    } else {
      return of([]);
    }
  }

  public getMembers(): Array<Player> {
    if (this.group) {
      return this.playerListService.getPlayerList().players.filter(player => this.group.getValue().members.includes(player.id));
    } else {
      return [];
    }
  }

  public selectMembersLength(excludeLocal: boolean): Observable<number> {
    return this.selectMembers()
      .pipe(
        map(members => {
          if (excludeLocal) {
            members = members.filter(member => member !== this.clientService.localPlayerId);
          }
          return members.length;
        }),
      );
  }

  public getMembersLength(excludeLocal: boolean): number {
    if (!this.group.getValue()) {
      return 0;
    }

    let members = this.group.getValue().members;
    if (excludeLocal) {
      members = members.filter(member => member !== this.clientService.localPlayerId);
    }

    return members.length;
  }

  public isPartyEnabled(): boolean {
    return (this.group.value ? this.group.getValue().isEnabled : false);
  }

  private updateGroup() {
    this.group.next(this.group.getValue());
  }

  private createGroup(group: Group | undefined) {
    if (!group) {
      group = new Group();
      this.group.next(group);

      if (!this.clientService.connectionStateChange.getValue()) {
        this.soundService.play(Sound.Success);
      }
    }
    return this.group.getValue();
  }

  public getLeaderName(): string {
    const group = this.group.getValue();
    if (group) {
      let player = this.playerListService.getPlayerById(group.owner);
      if (player) {
        return player.name;
      }
    }
    return '';
  }
}
