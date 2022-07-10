import { Injectable, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';
import { PartyInfo } from '../models/party-info';
import { ClientService } from './client.service';


// TODO: this is bad. We shouldn't take different paths to accommodate for
// mock testing. Instead, the skyrimtogether global should be mockable.
@Injectable({
  providedIn: 'root',
})
export class MockClientService implements OnDestroy {

  private partyInfo: PartyInfo;

  private debugSubscription: Subscription;
  private partyLaunchedSubscription: Subscription;
  private partyInviteSubscription: Subscription;
  private partyJoinedSubscription: Subscription;
  private memberKickedSubscription: Subscription;
  private partyLeftSubscription: Subscription;
  private partyLeaderSubscription: Subscription;

  constructor(
    private readonly clientService: ClientService,
  ) {
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

  ngOnDestroy() {
    this.debugSubscription.unsubscribe();
    this.partyLaunchedSubscription.unsubscribe();
    this.partyInviteSubscription.unsubscribe();
    this.partyJoinedSubscription.unsubscribe();
    this.memberKickedSubscription.unsubscribe();
    this.partyLeftSubscription.unsubscribe();
    this.partyLeaderSubscription.unsubscribe();
  }

  private onDebug() {
    this.debugSubscription = this.clientService.debugChange.subscribe(() => {
      console.log(this.partyInfo);
    });
  }

  private onPartyLaunched() {
    this.partyLaunchedSubscription = this.clientService.partyLaunchedChange.subscribe(() => {
      this.partyInfo.leaderId = this.clientService.localPlayerId;
    });
  }

  private onPartyInvite() {
    this.partyInviteSubscription = this.clientService.partyInviteChange.subscribe((playerId: number) => {
      this.partyInfo.playerIds.push(playerId);
      this.clientService.onPartyInfo(this.partyInfo.playerIds, this.partyInfo.leaderId);
    });
  }

  private onPartyJoined() {
    this.partyJoinedSubscription = this.clientService.partyJoinedChange.subscribe((inviterId: number) => {

    });
  }

  private onMemberKicked() {
    this.memberKickedSubscription = this.clientService.memberKickedChange.subscribe((playerId: number) => {
      const index = this.partyInfo.playerIds.indexOf(playerId);
      if (index > -1) {
        this.partyInfo.playerIds.splice(index, 1);
      }
      this.clientService.onPartyInfo(this.partyInfo.playerIds, this.partyInfo.leaderId);
    });
  }

  private onPartyLeft() {
    this.partyLeftSubscription = this.clientService.partyLeftChange.subscribe(() => {
      this.partyInfo.playerIds.splice(0);
      this.partyInfo.leaderId = -1;
    });
  }

  private onLeaderChanged() {
    this.partyLeaderSubscription = this.clientService.partyLeaderChange.subscribe((playerId: number) => {
      this.partyInfo.leaderId = playerId;
      this.clientService.onPartyInfo(this.partyInfo.playerIds, this.partyInfo.leaderId);
    });
  }

}
