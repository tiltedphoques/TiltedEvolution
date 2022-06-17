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

  private connectionSubscription: Subscription;
  private playerConnectedSubscription: Subscription;
  private playerDisconnectedSubscription: Subscription;
  private partyInviteReceivedSubscription: Subscription;

  private isConnect = false;

  constructor(private clientService: ClientService) {
    this.onConnectionStateChanged();
    this.onPlayerConnected();
    this.onPlayerDisconnected();
    this.onPartyInviteReceived();
  }

  ngOnDestroy() {
    this.connectionSubscription.unsubscribe();
    this.playerConnectedSubscription.unsubscribe();
    this.playerDisconnectedSubscription.unsubscribe();
    this.partyInviteReceivedSubscription.unsubscribe();
  }

  private onConnectionStateChanged() {
    this.connectionSubscription = this.clientService.connectionStateChange.subscribe((connect: boolean) => {
      if (this.isConnect == connect) {
        return;
      }
      this.isConnect = connect;
      //this.playerList.next(undefined);

      this.updatePlayerList();
    });
  }


  private onPlayerConnected() {
    this.playerConnectedSubscription = this.clientService.playerConnectedChange.subscribe((player: Player) => {

      console.log(player);

      const playerList = this.createPlayerList(this.playerList.value);

      if (playerList) {

        playerList.players.set(player.serverId, player);

        this.playerList.next(playerList);
      }
    })
  }

  private onPlayerDisconnected() {
    this.playerDisconnectedSubscription = this.clientService.playerDisconnectedChange.subscribe((player: Player) => {

      console.log(player);

      const playerList = this.createPlayerList(this.playerList.value);

      if (playerList) {
        playerList.players.delete(player.serverId);

        this.playerList.next(playerList);
      }
    });
  }

  private onPartyInviteReceived() {
    this.partyInviteReceivedSubscription = this.clientService.partyInviteReceivedChange.subscribe((inviterId: number) => {
      const playerList = this.createPlayerList(this.playerList.value);

      if (playerList) {
        playerList.players.get(inviterId).invitationReceived = true;

        this.playerList.next(playerList);
      }
    })
  }

  public getPlayerList() {
    return this.createPlayerList(this.playerList.value);
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

  public acceptPartyInvite(inviterId: number) {
    this.clientService.acceptPartyInvite(inviterId);
    
    const playerList = this.createPlayerList(this.playerList.value);

    if (playerList) {
      playerList.players.get(inviterId).invitationReceived = false;

      this.playerList.next(playerList);
    }
  }
}