import { Injectable, OnDestroy } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { BehaviorSubject, firstValueFrom, Subscription } from 'rxjs';
import { Player } from '../models/player';
import { PlayerList } from '../models/player-list';
import { PlayerManagerTab } from '../models/player-manager-tab.enum';
import { NotificationType } from '../models/popup-notification';
import { View } from '../models/view.enum';
import { UiRepository } from '../store/ui.repository';
import { ClientService } from './client.service';
import { PopupNotificationService } from './popup-notification.service';


@Injectable({
  providedIn: 'root',
})
export class PlayerListService implements OnDestroy {

  public playerList = new BehaviorSubject<PlayerList | undefined>(undefined);

  private debugSubscription: Subscription;
  private connectionSubscription: Subscription;
  private playerConnectedSubscription: Subscription;
  private playerDisconnectedSubscription: Subscription;
  private memberKickedSubscription: Subscription;
  private cellSubscription: Subscription;
  private partyInviteReceivedSubscription: Subscription;

  private isConnect = false;

  constructor(
    private readonly clientService: ClientService,
    private readonly popupNotificationService: PopupNotificationService,
    private readonly uiRepository: UiRepository,
    private readonly translocoService: TranslocoService,
  ) {
    this.onDebug();
    this.onConnectionStateChanged();
    this.onPlayerConnected();
    this.onPlayerDisconnected();
    this.onMemberKicked();
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
      const playerList = this.getPlayerList();

      if (playerList) {

        playerList.players.push(player);

        this.playerList.next(playerList);
      }
    });
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

  private onMemberKicked() {
    this.memberKickedSubscription = this.clientService.memberKickedChange.subscribe((playerId: number) => {
      const playerList = this.getPlayerList();
      const players = playerList.players.find(player => player.id !== playerId);
      players.hasBeenInvited = false;
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
    });
  }

  private onPartyInviteReceived() {
    this.partyInviteReceivedSubscription = this.clientService.partyInviteReceivedChange.subscribe(async (inviterId: number) => {
      const playerList = this.getPlayerList();

      if (playerList) {
        const invitingPlayer = this.getPlayerById(inviterId);
        invitingPlayer.hasInvitedLocalPlayer = true;
        this.playerList.next(playerList);
        const inviteMessage = await firstValueFrom(
          this.translocoService.selectTranslate('SERVICE.PLAYER_LIST.PARTY_INVITE', { from: invitingPlayer.name }),
        );
        const message = this.popupNotificationService
          .addMessage(inviteMessage, {
            type: NotificationType.Invitation,
            player: invitingPlayer,
            duration: 10000,
          });
        firstValueFrom(message.onClose)
          .then(clicked => {
            if (clicked) {
              this.uiRepository.openView(View.PLAYER_MANAGER);
              this.uiRepository.openPlayerManagerTab(PlayerManagerTab.PARTY_MENU);
            }
          });
      }
    });
  }

  public getLocalPlayer(): Player {
    let localPlayerId = this.clientService.localPlayerId;
    return this.getPlayerById(localPlayerId);
  }

  public getPlayerList() {
    return this.createPlayerList(this.playerList.getValue());
  }

  public getListLength(): number {
    return this.getPlayerList() ? this.getPlayerList().players.length : 0;
  }

  private createPlayerList(playerList: PlayerList | undefined) {
    if (!playerList) {
      playerList = new PlayerList();
      this.playerList.next(playerList);
    }
    return this.playerList.getValue();
  }

  public updatePlayerList() {
    this.playerList.next(this.playerList.getValue());
  }

  public sendPartyInvite(inviteeId: number) {
    const playerList = this.getPlayerList();

    if (playerList) {
      this.getPlayerById(inviteeId).hasBeenInvited = true;

      this.updatePlayerList();

      this.clientService.createPartyInvite(inviteeId);
    }
  }

  public acceptPartyInvite(inviterId: number) {
    const playerList = this.getPlayerList();

    if (playerList) {
      this.getPlayerById(inviterId).hasInvitedLocalPlayer = false;

      this.clientService.acceptPartyInvite(inviterId);

      this.playerList.next(playerList);
    }
  }

  public getPlayerById(playerId: number): Player {
    return this.getPlayerList().players.find(player => player.id === playerId);
  }

  public resetHasBeenInvitedFlags() {
    const playerList = this.getPlayerList();

    if (playerList) {
      for (const player of playerList.players) {
        player.hasBeenInvited = false;
      }

      this.updatePlayerList();
    }
  }

}
