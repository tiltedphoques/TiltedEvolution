import { Injectable, OnDestroy } from '@angular/core';
import { Group, State } from '../models/group';
import { BehaviorSubject, Subscription, Observable } from 'rxjs';
import { Player } from '../models/player';
import { ClientService } from './client.service';
import { PopupNotificationService } from './popup-notification.service';
import { NotificationType } from '../models/popup-notification';
import { ErrorService } from './error.service';
import { Sound, SoundService } from './sound.service';
import { PartyInfo } from '../models/party-info';
import { PlayerListService } from './player-list.service';
import { LoadingService } from './loading.service';

@Injectable({
  providedIn: 'root'
})
export class GroupService implements OnDestroy {

  public group = new BehaviorSubject<Group | undefined>(undefined);

  private connectionSubscription: Subscription;
  private userHealthSubscription: Subscription;
  private partyInfoSubscription: Subscription;
  private playerDisconnectedSubscription: Subscription;
  private levelSubscription: Subscription;
  private cellSubscription: Subscription;
  private loadedSubscription: Subscription;

  private isConnect = false;

  constructor(private errorService: ErrorService,
              private popupNotificationService: PopupNotificationService,
              private soundService: SoundService,
              private clientService: ClientService,
              private playerListService: PlayerListService,
              private loadingService: LoadingService) {
    this.onConnectionStateChanged();
    this.subscribeChangeHealth();
    this.onPartyInfo();
    this.onPlayerDisconnected();
    this.onLevelChange();
    this.onCellChange();
    this.onLoadedChange();
  }

  ngOnDestroy() {
    this.connectionSubscription.unsubscribe();
    this.userHealthSubscription.unsubscribe();
    this.partyInfoSubscription.unsubscribe();
    this.playerDisconnectedSubscription.unsubscribe();
    this.levelSubscription.unsubscribe();
    this.cellSubscription.unsubscribe();
    this.loadedSubscription.unsubscribe();
  }

  private onConnectionStateChanged() {
    this.connectionSubscription = this.clientService.connectionStateChange.subscribe((connect: boolean) => {
      if (this.isConnect == connect) {
        return;
      }

      this.isConnect = connect;

      if (connect) {
        this.createGroup(this.group.value);
      }
      else {
        this.group.next(undefined);
      }

      this.updateGroup();
    });
  }

  private subscribeChangeHealth() {
    this.userHealthSubscription = this.clientService.healthChange.subscribe((p: Player) => {

      const group = this.group.value;

      if (group) {
        const foundPlayer = group.members.get(p.serverId);

        if (foundPlayer) {
          foundPlayer.health = p.health;
          this.updateGroup();
        }
      }

    })
  }

  private onPartyInfo() {
    this.partyInfoSubscription = this.clientService.partyInfoChange.subscribe((partyInfo: PartyInfo) => {
      
      const group = this.createGroup(this.group.value);
      const playerList = this.playerListService.getPlayerList();

      if (group && playerList) {
        // TODO: this is very primitive, im sure there's some fancy js way to do this
        group.members.clear();

        for (const id of partyInfo.serverIds) {
          const player = this.playerListService.getPlayerList().players.get(id);
          group.members[id] = player;
        }

        this.updateGroup();
      }
    })
  }

  private onPlayerDisconnected() {
    this.playerDisconnectedSubscription = this.clientService.playerDisconnectedChange.subscribe((player: Player) => {
      const group = this.createGroup(this.group.value);

      if (group) {
        group.members.delete(player.serverId);

        this.group.next(group);
      }
    });
  }

  private onLevelChange() {
    this.levelSubscription = this.clientService.levelChange.subscribe((player: Player) => {
      const group = this.createGroup(this.group.value);

      if (group) {
        const p = group.members.get(player.serverId);
        if (p) {
          p.level = player.level;

          this.clientService.messageReception.next({content: `${p.name} has reached level ${player.level}.`});
        }
      }
    });
  }

  private onCellChange() {
    this.cellSubscription = this.clientService.cellChange.subscribe((player: Player) => {
      const group = this.createGroup(this.group.value);

      if (group) {
        const p = group.members.get(player.serverId);
        if (p) {
          p.cellName = player.cellName;
        }
      }
    })
  }

  private onLoadedChange() {
    this.loadedSubscription = this.clientService.isLoadedChange.subscribe((player: Player) => {
      const group = this.createGroup(this.group.value);

      if (group) {
        const p = group.members.get(player.serverId);
        if (p) {
          p.isLoaded = player.isLoaded;
          p.health = player.health;
        }
      }
    });
  }


  public launch() {
    this.clientService.launchParty();
    this.soundService.play(Sound.Focus);
    this.loadingService.setLoading(true);
  }

  public leave() {
    const group = this.createGroup(this.group.value);

    if (group) {
      this.soundService.play(Sound.Ok);

      this.clientService.leaveParty();
    }
  }

  public invite(playerId: number) {
    this.soundService.play(Sound.Ok);
    this.clientService.createPartyInvite(playerId);
  }

  public accept(inviterId: number) {
    const group = this.createGroup(this.group.value);

    if (group) {
      if (group.owner || group.members.size > 0) {
        this.errorService.error("You are already in a group. To join another group, please leave your current group.");
        return;
      }

      this.soundService.play(Sound.Ok);
  
      this.clientService.acceptPartyInvite(inviterId);
    }
  }

  public kick(playerId: number) {
    const group = this.createGroup(this.group.value);

    if (group) {
      if (group.owner.serverId !== this.clientService.localServerId) {
        this.errorService.error("You cannot kick other members as you are not the party leader.");
        return;
      }

      this.soundService.play(Sound.Ok);

      this.clientService.kickPartyMember(playerId);
    }
  }

  public changeLeader(playerId: number) {
    const group = this.createGroup(this.group.value);

    if (group) {
      if (group.owner.serverId !== this.clientService.localServerId) {
        this.errorService.error("You cannot make another member the leader as you are not the party leader.");
        return;
      }

      this.soundService.play(Sound.Ok);

      this.clientService.changePartyLeader(playerId);
    }
  }

  public getSizeMembers(): number {
    
    return (this.group.value ? this.group.value.members.size : 0);
  }

  private updateGroup() {
    this.group.next(this.group.value);
  }

  private createGroup(group: Group | undefined) {
    if (!group) {
      group = new Group();
      this.group.next(group);

      if (!this.clientService.connectionStateChange.value) {
        this.soundService.play(Sound.Success);
      }
    }
    return this.group.value;
  }
}
