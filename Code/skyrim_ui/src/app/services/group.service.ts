import { Injectable, OnDestroy } from '@angular/core';
import { Group, State } from '../models/group';
import { BehaviorSubject, Subscription, Observable } from 'rxjs';
import { WebSocketService } from './web-socket.service';
import { UserService } from './user.service';
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

  constructor(private wsService: WebSocketService,
              private userService: UserService,
              private errorService: ErrorService,
              private popupNotificationService: PopupNotificationService,
              private soundService: SoundService,
              private clientService: ClientService,
              private playerListService: PlayerListService) {
    this.onConnectionStateChanged();
    this.subscribeWebSocket();
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

        for (const id of partyInfo.serverIds) {
          const player = this.playerListService.playerList.getValue().players.get(id);
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

  private subscribeWebSocket() {
    this.wsSubscription = this.wsService.message.subscribe(
      (msg) => {


        let group = this.group.value;
        const user = this.userService.user.value;
        const player = this.userService.player.value;

        if (msg.operation === "party_invite") {

          if (user) {
            this.partyInvite(msg, user);
          }

        }

        else if (msg.operation === "party_leave") {

          if (group) {
            this.playerLeave(msg, group);
          }

        }

        // If the player is connected to a server that ignores the other messages of the API
        else if (!this.isConnect) {

          if (msg.operation === "party_status") {

            // Create group
            if (msg.state) {
              group = this.createGroup(group);

              if (group) {
                group!.state = msg.state.toLowerCase();

                if (group.state === State.Running && msg.ip && msg.token) {
                  this.soundService.play(Sound.Ok);
                  this.clientService.messageReception.next({content: 'Session created. Connection in progress'});

                  this.clientService.connect(msg.ip, msg.port ? msg.port : 10578, msg.token);
                }
              }
            }

            this.updateGroup();
          }

          else if (msg.operation === "party_become_owner") {
            group = this.createGroup(group);

            if (group) {
              group.owner = player;
              group.members.delete(player.id);
              this.updateGroup();
            }
          }

          else if (msg.operation === "party_owner") {
            group = this.createGroup(group);

            if (user && group && msg.id) {
              this.partyOwner(msg, group, user);
            }

          }

          else if (msg.operation === "party_join") {

            if (msg.id && msg.name && user && group) {
              this.partyJoin(msg, user, group);
            }

          }

          else if (msg.operation === "party_members") {
            group = this.createGroup(group);

            if (group && msg.members) {
              this.partyMembers(msg, group, player);
            }

          }

          else if (msg.operation === "server_request") {

            // to see with Ananace
            /*
            if (!msg.status) {
              this.errorService.error('Failed to create a session. Please try again later.');
            }
            */

            if (group) {

              /**
               * TODO: Wait Ananace FIX :'( :'(
               */

            }

          }

        }

      }
    );
  }

  public leave() {
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
  }

  public invite(userID: number) {
    this.soundService.play(Sound.Ok);
    this.wsService.send({operation:'invite', id: userID}).subscribe(
      () => {},
      () => this.errorService.error('Could not send invitation. Please try again later.')
    );
  }

  public accept(player: Player) {
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
  }

  public launch(): Observable<any> {
    if (this.getSizeMembers() > 0) {
      return this.wsService.send({operation: 'start'});
    }
    return Observable.throw("You can't start a party alone.");
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

  private partyInvite(msg: any, user: User) {
    if (user) {
      const playerInvite = user.friends.find((p: Player) => p.name === msg.name);

      // If it's a friend, add your invitation to the friends list
      if (playerInvite) {
        // @TODO
        playerInvite.invitation = msg.id;
        this.userService.user.next(user);
      }

      this.popupNotificationService.setMessage(`${msg.name} has invited you to join their group.`, NotificationType.Invitation, playerInvite);
    }
  }

  private partyJoin(msg: any, user: User, group: Group) {
    if (!msg.name) {
      return;
    }

    this.clientService.messageReception.next({content: `${msg.name} joined your group.`});

    const playerJoin = user.friends.find((p: Player) => p.name === msg.name);

    if (playerJoin) {
      this.removeInvite(playerJoin);
      // @Todo Change the presence API to send the user's id
      group.members.set(msg.id, playerJoin);
    }
    else {
      group.members.set(msg.id, new Player({
        name: msg.name,
        online: true
      }))
    }

    this.soundService.play(Sound.PlayerJoined);
    this.updateGroup();
  }

  private partyOwner(msg: any, group: Group, user: User) {

    // Find if the owner is already in the group members list
    let owner = group.members.get(msg.id);

    if (owner) {
      group.members.delete(msg.id);
      group.owner = owner;
    }
    else {
      owner = user.friends.find((p: Player) => p.id === msg.id)

      if (owner) {
        group!.owner = owner;
      }
      // If the owner is not in your friend list
      else {
        group!.owner = new Player({id: msg.id});
      }
    }

    this.updateGroup();
  }

  private partyMembers(msg: any, group: Group, player: Player) {
    msg.members.map((memberData: any) => {
      group.members.set(memberData.id, new Player({
        id: memberData.id,
        name: memberData.name
      }));

      if (player.id === 0 && player.name === memberData.name) {
        player.id = memberData.id;
      }
    });

    if (group.owner && group.owner.name) {
      group.members.delete(group.owner.id);
    }

    this.updateGroup();
  }

  private removeInvite(player: Player) {
    player.invitation = "";
  }

  private playerLeave(msg: any, group: Group) {
    if (msg.id) {

      if (group.owner && msg.id === group.owner.id) {
        group.owner = undefined;
      }
      else {
        group.members.delete(msg.id);
      }

      this.group.next(group);

      this.clientService.messageReception.next({content: `${msg.name} left your group.`});
      this.soundService.play(Sound.Cancel);
    }
  }
}
