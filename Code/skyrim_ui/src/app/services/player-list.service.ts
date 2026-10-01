import { Injectable, OnDestroy } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { BehaviorSubject, Subscription } from 'rxjs';
import { PartyInvite } from '../models/party-invite';
import { Player } from '../models/player';
import { PlayerList } from '../models/player-list';
import { PlayerManagerTab } from '../models/player-manager-tab.enum';
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
  private partyInfoSubscription: Subscription;
  private partyLeftSubscription: Subscription;

  private readonly inviteCooldownMs = 3000;
  private readonly sentInviteTimers = new Map<number, number>();
  private readonly receivedInvites = new Map<
    number,
    { expiresAt: number; timer: number }
  >();
  private partyLeaderId: number | undefined;
  private partyMemberIds: number[] = [];

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
    this.onPartyInfo();
    this.onPartyLeft();
  }

  ngOnDestroy() {
    this.resetPartyInvitations();
    this.debugSubscription.unsubscribe();
    this.connectionSubscription.unsubscribe();
    this.playerConnectedSubscription.unsubscribe();
    this.playerDisconnectedSubscription.unsubscribe();
    this.memberKickedSubscription.unsubscribe();
    this.cellSubscription.unsubscribe();
    this.partyInviteReceivedSubscription.unsubscribe();
    this.partyInfoSubscription.unsubscribe();
    this.partyLeftSubscription.unsubscribe();
  }

  private onDebug() {
    this.debugSubscription = this.clientService.debugChange.subscribe(() => {
      console.log(this.playerList);
    });
  }

  private onConnectionStateChanged() {
    this.connectionSubscription =
      this.clientService.connectionStateChange.subscribe((connect: boolean) => {
        if (this.isConnect == connect) {
          return;
        }
        this.isConnect = connect;
        this.resetPartyState();
        this.playerList.next(undefined);

        this.updatePlayerList();
      });
  }

  private onPlayerConnected() {
    this.playerConnectedSubscription =
      this.clientService.playerConnectedChange.subscribe((player: Player) => {
        const playerList = this.getPlayerList();

        if (playerList) {
          playerList.players.push(player);

          this.playerList.next(playerList);
        }
      });
  }

  private onPlayerDisconnected() {
    this.playerDisconnectedSubscription =
      this.clientService.playerDisconnectedChange.subscribe(
        (playerDisco: Player) => {
          this.clearSentInvite(playerDisco.id);
          this.clearReceivedInvite(playerDisco.id);
          const playerList = this.getPlayerList();

          if (playerList) {
            playerList.players = playerList.players.filter(
              player => player.id !== playerDisco.id,
            );

            this.playerList.next(playerList);
          }
        },
      );
  }

  private onMemberKicked() {
    this.memberKickedSubscription =
      this.clientService.memberKickedChange.subscribe((playerId: number) => {
        const playerList = this.getPlayerList();
        const players = playerList.players.find(
          player => player.id !== playerId,
        );
        players.hasBeenInvited = false;
      });
  }

  private onCellChange() {
    this.cellSubscription = this.clientService.cellChange.subscribe(
      (player: Player) => {
        const playerList = this.getPlayerList();

        if (playerList) {
          const p = this.getPlayerById(player.id);
          if (p) {
            p.cellName = player.cellName;
          }
        }
      },
    );
  }

  private onPartyInviteReceived() {
    this.partyInviteReceivedSubscription =
      this.clientService.partyInviteReceivedChange.subscribe(
        ({ inviterId, expiresInMs }: PartyInvite) => {
          const invitingPlayer = this.getPlayerById(inviterId);
          if (
            !this.isConnect ||
            this.partyLeaderId !== undefined ||
            !invitingPlayer ||
            inviterId === this.clientService.localPlayerId
          ) {
            return;
          }

          this.clearReceivedInvite(inviterId);
          if (!Number.isFinite(expiresInMs) || expiresInMs <= 0) {
            this.updatePlayerList();
            return;
          }

          const invitation = {
            expiresAt: performance.now() + expiresInMs,
            timer: setTimeout(() => {
              this.clearReceivedInvite(inviterId);
              this.updatePlayerList();
            }, expiresInMs),
          };
          this.receivedInvites.set(inviterId, invitation);
          invitingPlayer.hasInvitedLocalPlayer = true;
          this.updatePlayerList();
          this.popupNotificationService.addPartyInvite(
            invitingPlayer.name,
            () => {
              // A popup for a replaced invite must not accept the newer one.
              if (this.receivedInvites.get(inviterId) === invitation) {
                this.acceptPartyInvite(inviterId);
              }
            },
          );
        },
      );
  }

  private onPartyInfo() {
    this.partyInfoSubscription = this.clientService.partyInfoChange.subscribe(
      partyInfo => {
        if (this.partyLeaderId !== partyInfo.leaderId) {
          this.resetHasBeenInvitedFlags();
        }
        this.partyLeaderId = partyInfo.leaderId;
        this.partyMemberIds = partyInfo.playerIds;
        this.clearReceivedInvites();
        for (const playerId of partyInfo.playerIds) {
          this.clearSentInvite(playerId);
        }
        this.updatePlayerList();
      },
    );
  }

  private onPartyLeft() {
    this.partyLeftSubscription = this.clientService.partyLeftChange.subscribe(
      () => this.resetPartyState(),
    );
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
    const player = this.getPlayerById(inviteeId);
    if (
      !this.isConnect ||
      !player ||
      this.partyLeaderId !== this.clientService.localPlayerId ||
      inviteeId === this.clientService.localPlayerId ||
      this.partyMemberIds.includes(inviteeId) ||
      this.sentInviteTimers.has(inviteeId)
    ) {
      return;
    }

    player.hasBeenInvited = true;
    this.sentInviteTimers.set(
      inviteeId,
      setTimeout(() => {
        this.clearSentInvite(inviteeId);
        this.updatePlayerList();
      }, this.inviteCooldownMs),
    );
    this.updatePlayerList();
    this.clientService.createPartyInvite(inviteeId);
  }

  public acceptPartyInvite(inviterId: number) {
    const invitation = this.receivedInvites.get(inviterId);
    if (
      !this.isConnect ||
      this.partyLeaderId !== undefined ||
      !this.getPlayerById(inviterId) ||
      !invitation
    ) {
      return;
    }
    if (performance.now() >= invitation.expiresAt) {
      this.clearReceivedInvite(inviterId);
      this.updatePlayerList();
      return;
    }

    // The server can reject this invite. Keep other invitations until partyInfo
    // confirms a successful join, and consume this one before the bridge call.
    this.clearReceivedInvite(inviterId);
    this.updatePlayerList();
    this.clientService.acceptPartyInvite(inviterId);
  }

  public getPlayerById(playerId: number): Player {
    return this.getPlayerList().players.find(player => player.id === playerId);
  }

  public resetHasBeenInvitedFlags() {
    for (const timer of this.sentInviteTimers.values()) {
      clearTimeout(timer);
    }
    this.sentInviteTimers.clear();
    const playerList = this.playerList.getValue();

    if (playerList) {
      for (const player of playerList.players) {
        player.hasBeenInvited = false;
      }

      this.updatePlayerList();
    }
  }

  public resetPartyInvitations() {
    this.resetHasBeenInvitedFlags();
    this.clearReceivedInvites();
    this.updatePlayerList();
  }

  public resetPartyState() {
    this.partyLeaderId = undefined;
    this.partyMemberIds = [];
    this.resetPartyInvitations();
  }

  private clearSentInvite(playerId: number) {
    clearTimeout(this.sentInviteTimers.get(playerId));
    this.sentInviteTimers.delete(playerId);
    const player = this.playerList
      .getValue()
      ?.players.find(player => player.id === playerId);
    if (player) {
      player.hasBeenInvited = false;
    }
  }

  private clearReceivedInvite(playerId: number) {
    clearTimeout(this.receivedInvites.get(playerId)?.timer);
    this.receivedInvites.delete(playerId);
    const player = this.playerList
      .getValue()
      ?.players.find(player => player.id === playerId);
    if (player) {
      player.hasInvitedLocalPlayer = false;
    }
  }

  private clearReceivedInvites() {
    for (const playerId of this.receivedInvites.keys()) {
      this.clearReceivedInvite(playerId);
    }
  }
}
