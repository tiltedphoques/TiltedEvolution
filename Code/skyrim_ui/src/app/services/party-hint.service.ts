import { Injectable, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';
import { ClientService } from './client.service';
import { GroupService } from './group.service';
import { PlayerListService } from './player-list.service';
import { PopupNotificationService } from './popup-notification.service';

/** Minimum time between two party hints, in milliseconds. */
const PARTY_HINT_COOLDOWN = 15 * 60 * 1000;

/**
 * Suggests forming a party when the player progresses quests alone
 * while other players are on the server.
 */
@Injectable({
  providedIn: 'root',
})
export class PartyHintService implements OnDestroy {
  private questUpdatedSubscription: Subscription;

  /** Set by "Don't show", lasts for the current session only. */
  private disabled = false;
  private lastShownAt?: number;

  constructor(
    private readonly clientService: ClientService,
    private readonly groupService: GroupService,
    private readonly playerListService: PlayerListService,
    private readonly popupNotificationService: PopupNotificationService,
  ) {
    this.questUpdatedSubscription =
      this.clientService.questUpdatedChange.subscribe(() => {
        if (this.shouldShow()) {
          this.show();
        }
      });
  }

  ngOnDestroy() {
    this.questUpdatedSubscription.unsubscribe();
  }

  private shouldShow(): boolean {
    if (this.disabled) {
      return false;
    }

    if (
      this.lastShownAt !== undefined &&
      Date.now() - this.lastShownAt < PARTY_HINT_COOLDOWN
    ) {
      return false;
    }

    if (!this.clientService.connectionStateChange.getValue()) {
      return false;
    }

    if (this.groupService.isPartyEnabled()) {
      return false;
    }

    const otherPlayers = (
      this.playerListService.getPlayerList()?.players ?? []
    ).filter(player => player.id !== this.clientService.localPlayerId);

    return otherPlayers.length > 0;
  }

  private show() {
    this.lastShownAt = Date.now();
    this.popupNotificationService.addPartyHint(
      () => this.clientService.openPlayGuide(),
      () => (this.disabled = true),
    );
  }
}
