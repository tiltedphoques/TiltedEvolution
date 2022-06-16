import { Injectable, OnDestroy } from '@angular/core';
import { Group, State } from '../models/group';
import { BehaviorSubject, Subscription, Observable } from 'rxjs';
import { Player } from '../models/player';
import { User } from '../models/user';
import { ClientService } from './client.service';
import { PopupNotificationService } from './popup-notification.service';
import { NotificationType } from '../models/popup-notification';
import { ErrorService } from './error.service';
import { Sound, SoundService } from './sound.service';
import { PartyInfo } from '../models/party-info';
import { PlayerListService } from './player-list.service';

@Injectable({
  providedIn: 'root'
})
export class GroupService implements OnDestroy {

  public group = new BehaviorSubject<Group | undefined>(undefined);

  private connectionSubscription: Subscription;
  private wsSubscription: Subscription;
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
              private playerListService: PlayerListService) {
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
    if (this.wsSubscription) {
      this.wsSubscription.unsubscribe();
    }
  }

  private onConnectionStateChanged() {
    this.connectionSubscription = this.clientService.connectionStateChange.subscribe((connect: boolean) => {
      if (this.isConnect == connect) {
        return;
      }
      this.isConnect = connect;
      this.group.next(undefined);
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

      if (group) {
        // TODO: this is very primitive, im sure there's some fancy js way to do this
        group.members.clear();

        const playerList = this.playerListService.getPlayerList();

        if (playerList) {
          for (const id of partyInfo.serverIds) {
            const player = this.playerListService.getPlayerList().players.get(id);
            group.members[id] = player;
          }

          this.updateGroup();
        }
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


  public leave() {
    // TODO
    /*
    if (this.isConnect) {
      this.clientService.disconnect();
    }
    else if (this.group.value) {
      this.wsService.send({operation:'leave'}).subscribe(
        () => {},
        () => this.errorService.error('Could not leave the group for the moment. Please try again later.'),
        () => this.group.next(undefined)
      );
    }
    */
  }

  public invite(userID: number) {
    // TODO
    /*
    this.soundService.play(Sound.Ok);
    this.wsService.send({operation:'invite', id: userID}).subscribe(
      () => {},
      () => this.errorService.error('Could not send invitation. Please try again later.')
    );
    */
  }

  public accept(player: Player) {
    // TODO
    /*
    const group = this.group.value;
    if (group && (group.owner || group.members.size > 0)) {
      this.errorService.error("You are already in a group. To join another group, please leave your current group.");
      return;
    }

    this.soundService.play(Sound.Ok);
    this.wsService.send({operation: 'accept', party: player.invitation}).subscribe(
      () => {},
      () => this.errorService.error('Could not accept invitation. Please try again later.'),
      () => player.invitation = ""
    );
    */
  }

  public launch(): Observable<any> {
    // TODO
    /*
    if (this.getSizeMembers() > 0) {
      return this.wsService.send({operation: 'start'});
    }
    return Observable.throw("You can't start a party alone.");
    */
    return Observable.throw("Not implemented");
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
