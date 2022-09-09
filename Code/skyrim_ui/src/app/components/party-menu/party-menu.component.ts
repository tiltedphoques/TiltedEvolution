import { ChangeDetectionStrategy, Component } from '@angular/core';
import { Observable, pluck, startWith } from 'rxjs';
import { filter, map } from 'rxjs/operators';
import { Player } from 'src/app/models/player';
import { ClientService } from 'src/app/services/client.service';
import { GroupService } from 'src/app/services/group.service';
import { LoadingService } from 'src/app/services/loading.service';
import { PlayerListService } from 'src/app/services/player-list.service';


@Component({
  selector: 'app-party-menu',
  templateUrl: './party-menu.component.html',
  styleUrls: ['./party-menu.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class PartyMenuComponent {

  isLoading$ = this.loadingService.getLoading();
  members$ = this.groupService.selectMembers();
  memberCount$ = this.groupService.selectMembersLength(true);
  group$ = this.groupService.group.asObservable();
  isLaunchPartyDisabled$: Observable<boolean>;
  invitations$: Observable<Player[]>;
  isPartyLeader$: Observable<boolean>;

  constructor(
    private readonly groupService: GroupService,
    private readonly loadingService: LoadingService,
    private readonly playerListService: PlayerListService,
    private readonly clientService: ClientService,
  ) {
    this.invitations$ = this.playerListService.playerList
      .asObservable()
      .pipe(
        filter(playerlist => !!playerlist),
        pluck('players'),
        map(players => players.filter(player => player.hasInvitedLocalPlayer)),
        startWith([]),
      );
    this.isPartyLeader$ = this.groupService.group
      .asObservable()
      .pipe(
        map(group => group.isEnabled && group.owner == this.clientService.localPlayerId),
      );
    this.isLaunchPartyDisabled$ = this.groupService.selectMembersLength(false).pipe(
      map(count => count > 1),
    );
  }

  public launchParty() {
    this.groupService.launch();
  }

  public leave() {
    this.groupService.leave();
  }

  public teleportToPlayer(playerId: number) {
    this.clientService.teleportToPlayer(playerId);
  }

  public acceptPartyInvite(inviterId: number) {
    this.playerListService.acceptPartyInvite(inviterId);
  }

  public kickMember(playerId: number) {
    this.clientService.kickPartyMember(playerId);
  }

  public changeLeader(playerId: number) {
    this.clientService.changePartyLeader(playerId);
  }

}
