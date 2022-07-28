import { Injectable } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { firstValueFrom, fromEvent, takeUntil, withLatestFrom } from 'rxjs';
import { filter } from 'rxjs/operators';
import { PartyInfo } from '../models/party-info';
import { Player } from '../models/player';
import { PlayerManagerTab } from '../models/player-manager-tab.enum';
import { NotificationType } from '../models/popup-notification';
import { View } from '../models/view.enum';
import { UiRepository } from '../store/ui.repository';
import { ClientService } from './client.service';
import { DestroyService } from './destroy.service';
import { PopupNotificationService } from './popup-notification.service';


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
        filter((e: KeyboardEvent) => e.key === 'n'),
        withLatestFrom(this.translocoService.selectTranslate('SERVICE.PLAYER_LIST.PARTY_INVITE', { from: 'Dumbeldor' })),
      )
      .subscribe(([, inviteMessage]) => {
        const message = this.popupNotificationService
          .addMessage(inviteMessage, {
            type: NotificationType.Connection,
            duration: 10000,
            player: new Player({
                name: 'Dumbeldor',
                online: true,
                avatar: 'https://skyrim-together.com/images/float/avatars/random3.jpg',
              },
            ),
          });
        firstValueFrom(message.onClose)
          .then(clicked => {
            if (clicked) {
              this.uiRepository.openView(View.PLAYER_MANAGER);
              this.uiRepository.openPlayerManagerTab(PlayerManagerTab.PARTY_MENU);
            }
          });
      });
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
