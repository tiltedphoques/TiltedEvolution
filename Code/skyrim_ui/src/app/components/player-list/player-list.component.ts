import { ChangeDetectionStrategy, Component } from '@angular/core';
import { combineLatest, Observable, pluck, ReplaySubject, share } from 'rxjs';
import { map } from 'rxjs/operators';
import { ClientService } from 'src/app/services/client.service';
import { TradeUiService } from 'src/app/services/trade-ui.service';
import { GroupService } from 'src/app/services/group.service';
import { PlayerListService } from 'src/app/services/player-list.service';
import { Player } from '../../models/player';

@Component({
  selector: 'app-player-list',
  templateUrl: './player-list.component.html',
  styleUrls: ['./player-list.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class PlayerListComponent {
  playerList$: Observable<
    (Player & {
      isMember: boolean;
      isTrading: boolean;
      tradeBusy: boolean;
      pendingInvite: boolean;
    })[]
  >;
  playerListLength$: Observable<number>;
  isPartyLeader$: Observable<boolean>;

  constructor(
    private readonly playerListService: PlayerListService,
    private readonly clientService: ClientService,
    private readonly groupService: GroupService,
    private readonly tradeUiService: TradeUiService,
  ) {
    this.playerList$ = combineLatest([
      this.playerListService.playerList.asObservable().pipe(pluck('players')),
      this.groupService.group.asObservable().pipe(pluck('members')),
      this.tradeUiService.session$,
      this.tradeUiService.pendingOutgoing$,
    ]).pipe(
      map(([players, members, session, pendingOutgoing]) => {
        if (!players) {
          return [];
        }
        const localId = this.clientService.localPlayerId;
        const activePartnerId =
          session && session.active ? session.partnerId : undefined;
        const memberList = Array.isArray(members) ? members : [];
        const outgoing = new Set<number>(
          pendingOutgoing ? Array.from(pendingOutgoing) : [],
        );
        const tradeBusy = Boolean(session?.active) || outgoing.size > 0;
        return players
          .filter(player => player.id !== localId)
          .map(player => ({
            ...player,
            isMember: memberList.includes(player.id),
            isTrading: activePartnerId === player.id,
            tradeBusy,
            pendingInvite: outgoing.has(player.id),
          }));
      }),
      share({
        connector: () => new ReplaySubject(1),
        resetOnRefCountZero: true,
      }),
    );
    this.playerListLength$ = this.playerList$.pipe(
      map(players => players?.length ?? 0),
    );
    this.isPartyLeader$ = this.groupService.group.asObservable().pipe(
      map(
        group =>
          group.isEnabled && group.owner == this.clientService.localPlayerId,
      ),
      share({
        connector: () => new ReplaySubject(1),
        resetOnRefCountZero: true,
      }),
    );
  }

  public sendPartyInvite(inviteeId: number) {
    this.playerListService.sendPartyInvite(inviteeId);
  }

  public sendTradeInvite(inviteeId: number) {
    this.tradeUiService.sendInvite(inviteeId);
  }

  public cancelTradeInvite(inviteeId: number) {
    this.tradeUiService.cancelInvite(inviteeId);
  }
}
