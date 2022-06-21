import { Component, EventEmitter, OnDestroy, OnInit, Output, ViewEncapsulation } from "@angular/core";
import { Player } from "src/app/models/player";
import { PlayerList } from "src/app/models/player-list";
import { ClientService } from "src/app/services/client.service";
import { GroupService } from "src/app/services/group.service";
import { PlayerListService } from "src/app/services/player-list.service";

@Component({
  selector: 'app-player-list',
  templateUrl: './player-list.component.html',
  styleUrls: ['./player-list.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class PlayerListComponent implements OnInit, OnDestroy {

  constructor(public playerListService: PlayerListService,
              private clientService: ClientService,
              private groupService: GroupService
  ) { }

  ngOnInit(): void {
  }

  ngOnDestroy(): void {
  }

  public get playerList(): PlayerList | undefined {
    return this.playerListService.playerList.value;
  }

  public getListLength(): number {
    return this.playerListService.getListLength();
  }

  public get isConnected(): boolean {
    return this.clientService.connectionStateChange.value;
  }

  public get isPartyLeader(): boolean {
    return this.groupService.group.value.isEnabled && this.groupService.group.value.owner == this.clientService.localPlayerId;
  }

  public teleportToPlayer(playerId: number) {
    this.clientService.teleportToPlayer(playerId);
  }

  public sendPartyInvite(inviteeId: number) {
    this.playerListService.sendPartyInvite(inviteeId);
  }
}