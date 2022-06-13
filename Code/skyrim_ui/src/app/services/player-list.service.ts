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

    private isConnect = false;

    constructor(private clientService: ClientService) {
        this.onConnectionStateChanged();
        this.onPlayerConnected();
        this.onPlayerDisconnected();
    }

    ngOnDestroy() {
        this.connectionSubscription.unsubscribe();
        this.playerConnectedSubscription.unsubscribe();
        this.playerDisconnectedSubscription.unsubscribe();
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
}