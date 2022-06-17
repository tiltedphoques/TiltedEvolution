import { Component, EventEmitter, OnDestroy, OnInit, Output, ViewEncapsulation } from "@angular/core";
import { PlayerList } from "src/app/models/player-list";
import { ClientService } from "src/app/services/client.service";
import { PlayerListService } from "src/app/services/player-list.service";

@Component({
  selector: 'app-player-list',
  templateUrl: './player-list.component.html',
  styleUrls: ['./player-list.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class PlayerListComponent implements OnInit, OnDestroy {

  @Output()
  public done = new EventEmitter();

  constructor(public playerListService: PlayerListService,
    private clientService: ClientService
  ) { }

  ngOnInit(): void {
  }

  ngOnDestroy(): void {

  }

  public get playerList(): PlayerList | undefined {
    console.log("##LIST##");
    console.log("list " + this.playerListService.playerList);
    console.log("list " + this.playerListService.playerList.value);
    return this.playerListService.playerList.value;
  }

  public get getListSize(): number {
    console.log("##SIZE##");
    console.log("service " + this.playerListService);
    console.log("list " + this.playerList);
    console.log("players" + this.playerList.players);
    return this.playerList.players.size;
  }

  public get isConnected(): boolean {
    return this.clientService.connectionStateChange.value;
  }

  public teleportToPlayer(playerId: number) {
    this.clientService.teleportToPlayer(playerId);
  }

  public acceptPartyInvite(inviterId: number) {
    this.playerListService.acceptPartyInvite(inviterId);
  }

  public cancel(): void {
    this.done.next();
  }
}