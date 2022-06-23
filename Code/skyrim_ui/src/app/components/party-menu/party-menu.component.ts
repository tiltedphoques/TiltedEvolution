import { Component, EventEmitter, OnDestroy, OnInit, Output, ViewEncapsulation } from "@angular/core";
import { Subscription } from "rxjs";
import { GroupService } from "src/app/services/group.service";
import { LoadingService } from "src/app/services/loading.service";
import { Player } from "src/app/models/player";
import { PlayerList } from "src/app/models/player-list";
import { PlayerListService } from "src/app/services/player-list.service";
import { Group } from "src/app/models/group";
import { ClientService } from "src/app/services/client.service";

@Component({
    selector: 'app-party-menu',
    templateUrl: './party-menu.component.html',
    styleUrls: ['./party-menu.component.scss'],
    encapsulation: ViewEncapsulation.None
})
export class PartyMenuComponent implements OnInit, OnDestroy {

    public waitLaunch = false;

    private isLoading: Subscription;

    constructor(public groupService: GroupService, 
                private loadingService: LoadingService,
                private playerListService: PlayerListService,
                private clientService: ClientService) 
            { }

    ngOnInit(): void {

        this.isLoading = this.loadingService.getLoading().subscribe((isLoading: boolean) => {
            this.waitLaunch = isLoading;
        })
    }

    ngOnDestroy(): void {
        this.isLoading.unsubscribe();
    }

    public get playerList(): PlayerList | undefined {
        return this.playerListService.playerList.value;
    }

    public get invitees(): Array<Player> | undefined {
        return this.playerListService.playerList.value.players.filter(player => player.hasInvitedLocalPlayer);
    }

    public get group(): Group | undefined {
        return this.groupService.group.value;
    }

    public get members(): Array<Player> | undefined {
        return this.groupService.getMembers();
    }

    public get isPartyLeader(): boolean {
      return this.groupService.group.value.isEnabled && this.groupService.group.value.owner == this.clientService.localPlayerId;
    }

    public get getMembersLength(): number {
      return this.groupService.getMembersLength();
    }

    isLaunchPartyDisabled(): boolean {
        return this.groupService.getMembersLength() > 1;
    }

    public launchParty() {
        this.groupService.launch();
    }

    public leave() {
        this.groupService.leave();
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