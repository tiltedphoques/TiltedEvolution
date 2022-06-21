import { Component, EventEmitter, OnDestroy, OnInit, Output, ViewEncapsulation } from "@angular/core";
import { Subscription } from "rxjs";
import { GroupService } from "src/app/services/group.service";
import { LoadingService } from "src/app/services/loading.service";
import { Player } from "src/app/models/player";
import { PlayerList } from "src/app/models/player-list";
import { PlayerListService } from "src/app/services/player-list.service";

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
                private playerListService: PlayerListService) 
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

    isLaunchPartyDisabled(): boolean {
        return (this.waitLaunch || (this.groupService.getMembersLength() > 1));
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
}