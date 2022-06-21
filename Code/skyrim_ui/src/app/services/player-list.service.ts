import { Injectable } from '@angular/core';
import { BehaviorSubject, Subscription } from 'rxjs';
import { Player } from '../models/player';
import { PlayerList } from '../models/player-list';
import { ClientService } from './client.service';

@Injectable({
  providedIn: 'root'
})
export class PlayerListService {

  public playerList = new BehaviorSubject<PlayerList | undefined>(undefined);

  private debugSubscription: Subscription;
  private connectionSubscription: Subscription;
  private playerConnectedSubscription: Subscription;
  private playerDisconnectedSubscription: Subscription;
  private cellSubscription: Subscription;
  private partyInviteReceivedSubscription: Subscription;

  private isConnect = false;

  constructor(private clientService: ClientService) {
    this.onDebug();
    this.onConnectionStateChanged();
    this.onPlayerConnected();
    this.onPlayerDisconnected();
    this.onCellChange();
    this.onPartyInviteReceived();
  }

  ngOnDestroy() {
    this.debugSubscription.unsubscribe();
    this.connectionSubscription.unsubscribe();
    this.playerConnectedSubscription.unsubscribe();
    this.playerDisconnectedSubscription.unsubscribe();
    this.cellSubscription.unsubscribe();
    this.partyInviteReceivedSubscription.unsubscribe();
  }

  private onDebug() {
    this.debugSubscription = this.clientService.debugChange.subscribe(() => {
      console.log(this.playerList);
    });
  }

  private onConnectionStateChanged() {
    this.connectionSubscription = this.clientService.connectionStateChange.subscribe((connect: boolean) => {
      if (this.isConnect == connect) {
        return;
      }
      this.isConnect = connect;
      this.playerList.next(undefined);

      this.updatePlayerList();
    });
  }

  private onPlayerConnected() {
    this.playerConnectedSubscription = this.clientService.playerConnectedChange.subscribe((player: Player) => {

      console.log(player);

      const playerList = this.getPlayerList();

      if (playerList) {

        playerList.players.push(player);

        this.playerList.next(playerList);
      }
    })
  }

  private onPlayerDisconnected() {
    this.playerDisconnectedSubscription = this.clientService.playerDisconnectedChange.subscribe((playerDisco: Player) => {

      const playerList = this.getPlayerList();

      if (playerList) {
        playerList.players = playerList.players.filter(player => player.id !== playerDisco.id);

        this.playerList.next(playerList);
      }
    });
  }

  private onCellChange() {
    this.cellSubscription = this.clientService.cellChange.subscribe((player: Player) => {
      const playerList = this.getPlayerList();

      if (playerList) {
        const p = this.getPlayerById(player.id);
        if (p) {
          p.cellName = player.cellName;
        }
      }
    })
  }

  private onPartyInviteReceived() {
    this.partyInviteReceivedSubscription = this.clientService.partyInviteReceivedChange.subscribe((inviterId: number) => {
      const playerList = this.getPlayerList();

      if (playerList) {
        this.getPlayerById(inviterId).hasInvitedLocalPlayer = true;

        this.playerList.next(playerList);
      }
    })
  }

  public getPlayerList() {
    return this.createPlayerList(this.playerList.value);
  }

  public getListLength(): number {
    return this.getPlayerList() ? this.getPlayerList().players.length : 0;
  }

  private createPlayerList(playerList: PlayerList | undefined) {
    if (!playerList) {
      playerList = new PlayerList();
      this.playerList.next(playerList);
    }
    return this.playerList.value;
  }

  private updatePlayerList() {
    this.playerList.next(this.playerList.value);
  }

  public sendPartyInvite(inviteeId: number) {
    this.clientService.createPartyInvite(inviteeId);
    
    const playerList = this.getPlayerList();

    if (playerList) {
      this.getPlayerById(inviteeId).hasBeenInvited = true;

      this.playerList.next(playerList);
    }
  }

  public acceptPartyInvite(inviterId: number) {
    this.clientService.acceptPartyInvite(inviterId);
    
    const playerList = this.getPlayerList();

    if (playerList) {
      this.getPlayerById(inviterId).hasInvitedLocalPlayer = false;

      this.playerList.next(playerList);
    }
  }

  public getPlayerById(playerId: number) : Player {
    return this.getPlayerList().players.find(player => player.id === playerId);
  }
}