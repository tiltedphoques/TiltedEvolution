import { Injectable } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { fromEvent, takeUntil } from 'rxjs';
import { filter } from 'rxjs/operators';
import { PartyInfo } from '../models/party-info';
import { PlayerManagerTab } from '../models/player-manager-tab.enum';
import { View } from '../models/view.enum';
import { UiRepository } from '../store/ui.repository';
import { ClientService } from './client.service';
import { DestroyService } from './destroy.service';
import { PopupNotificationService } from './popup-notification.service';
import { faHandshakeSimple } from '@fortawesome/free-solid-svg-icons';


// TODO: this is bad. We shouldn't take different paths to accommodate for
// mock testing. Instead, the skyrimtogether global should be mockable.
@Injectable({
  providedIn: 'root',
})
export class MockClientService extends DestroyService {

  private readonly partyInfo: PartyInfo;

  constructor(
    private readonly clientService: ClientService,
    private readonly popupNotificationService: PopupNotificationService,
    private readonly uiRepository: UiRepository,
    private readonly translocoService: TranslocoService,
  ) {
    super();
    this.partyInfo = new PartyInfo({
      playerIds: [],
      leaderId: undefined,
    });

    this.onDebug();
    this.onPartyLaunched();
    this.onPartyInvite();
    this.onPartyJoined();
    this.onMemberKicked();
    this.onPartyLeft();
    this.onLeaderChanged();
  }

  private onDebug() {
    this.clientService.debugChange
      .pipe(takeUntil(this))
      .subscribe(() => {
        console.log(this.partyInfo);
      });
  }

  private onPartyLaunched() {
    this.clientService.partyLaunchedChange
      .pipe(takeUntil(this))
      .subscribe(() => {
        this.partyInfo.leaderId = this.clientService.localPlayerId;
      });
  }

  private onPartyInvite() {
    this.clientService.partyInviteChange
      .pipe(takeUntil(this))
      .subscribe((playerId: number) => {
        this.partyInfo.playerIds.push(playerId);
        this.clientService.onPartyInfo(this.partyInfo.playerIds, this.partyInfo.leaderId);
      });
      fromEvent(window, 'keydown')
      .pipe(
        takeUntil(this),
        filter((e: KeyboardEvent) => e.key === 'n')
      )
      .subscribe(() => {
        this.popupNotificationService.addMessage({
          messageKey: 'Notifications can show an image, an icon or both. Supports multiple actions or none. Text that is too long will be wrapped. Supports live translation',
          duration: 10000,
          imageUrl: 'https://skyrim-together.com/images/logo.png',
          icon: faHandshakeSimple
        })
        this.popupNotificationService.addMessage({
          messageKey: 'SERVICE.GROUP.LEVEL_UP',
          messageParams: { name: 'Dumbeldor', level: 100 },
          duration: 10000,
          actions: [
          {
            nameKey: "OPTION",
            callback: () => {console.log("1")}
          },
          {
            nameKey: "OPTION",
            callback: () => {console.log("2")}
          },
          {
            nameKey: "OPTION",
            callback: () => {console.log("3")}
          },
          
        ]
        })
        this.popupNotificationService.addPartyInvite('Cosideci', () => console.log("ACCEPTED!"));
      })
  }

  private onPartyJoined() {
    this.clientService.partyJoinedChange
      .pipe(takeUntil(this))
      .subscribe((inviterId: number) => {

      });
  }

  private onMemberKicked() {
    this.clientService.memberKickedChange
      .pipe(takeUntil(this))
      .subscribe((playerId: number) => {
        const index = this.partyInfo.playerIds.indexOf(playerId);
        if (index > -1) {
          this.partyInfo.playerIds.splice(index, 1);
        }
        this.clientService.onPartyInfo(this.partyInfo.playerIds, this.partyInfo.leaderId);
      });
  }

  private onPartyLeft() {
    this.clientService.partyLeftChange
      .pipe(takeUntil(this))
      .subscribe(() => {
        this.partyInfo.playerIds.splice(0);
        this.partyInfo.leaderId = -1;
      });
  }

  private onLeaderChanged() {
    this.clientService.partyLeaderChange
      .pipe(takeUntil(this))
      .subscribe((playerId: number) => {
        this.partyInfo.leaderId = playerId;
        this.clientService.onPartyInfo(this.partyInfo.playerIds, this.partyInfo.leaderId);
      });
  }

}
