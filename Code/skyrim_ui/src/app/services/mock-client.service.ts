import { Injectable, OnDestroy } from '@angular/core';
import { BehaviorSubject, Subscription } from 'rxjs';
import { PartyInfo } from '../models/party-info';
import { ClientService } from './client.service';

// TODO: this is bad. We shouldn't take different paths to accommodate for
// mock testing. Instead, the skyrimtogether global should be mockable.
@Injectable({
  providedIn: 'root'
})
export class MockClientService implements OnDestroy {

  private partyInfo: PartyInfo;

  private debugSubscription: Subscription;
  private partyInviteSubscription: Subscription;
  private partyJoinedSubscription: Subscription;
  private memberKickedSubscription: Subscription;

  constructor(private clientService: ClientService) {
    this.onDebug();
	this.onPartyInvite();
	this.onPartyJoined();
	this.onMemberKicked();
  }

  ngOnDestroy() {
    this.debugSubscription.unsubscribe();
	this.partyInviteSubscription.unsubscribe();
	this.partyJoinedSubscription.unsubscribe();
	this.memberKickedSubscription.unsubscribe();
  }

  private onDebug() {
    this.debugSubscription = this.clientService.debugChange.subscribe(() => {
      console.log(this.partyInfo);
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
		// TODO
    });
  }

  private onMemberKicked() {
    this.memberKickedSubscription = this.clientService.memberKickedChange.subscribe((playerId: number) => {
		const index = this.partyInfo.playerIds.indexOf(playerId);
		if (index > -1) {
			this.partyInfo.playerIds.splice(index);
		}
		this.clientService.onPartyInfo(this.partyInfo.playerIds, this.partyInfo.leaderId);
    });
  }
}
