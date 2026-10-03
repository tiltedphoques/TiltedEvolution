import { Injectable } from '@angular/core';
import { ChatService } from './chat.service';
import { ClientService } from './client.service';
import { PopupNotificationService } from './popup-notification.service';

/** Minimum time between two party hints, in milliseconds. */
const PARTY_HINT_COOLDOWN = 15 * 60 * 1000;

/** Suggests forming a party when the player progresses quests alone.
 *  The client decides when `questUpdated` is worth a hint. */
@Injectable({
  providedIn: 'root',
})
export class PartyHintService {
  /** Set by "Don't show", lasts for the current session only. */
  private disabled = false;
  private lastShownAt = -Infinity;

  constructor(
    chatService: ChatService,
    clientService: ClientService,
    popupNotificationService: PopupNotificationService,
  ) {
    clientService.questUpdatedChange.subscribe(() => {
      const now = Date.now();
      if (this.disabled || now - this.lastShownAt < PARTY_HINT_COOLDOWN) {
        return;
      }
      this.lastShownAt = now;
      popupNotificationService.addPartyHint(
        () => clientService.openPlayGuide(),
        () => (this.disabled = true),
      );
    });

    // Debug helper for testing the hint without waiting out the cooldown
    chatService.registerCommand({
      name: 'resetpartyhint',
      hidden: true,
      executor: async () => {
        this.lastShownAt = -Infinity;
        this.disabled = false;
        chatService.pushSystemMessage('Party hint timer has been reset.');
      },
    });
  }
}
