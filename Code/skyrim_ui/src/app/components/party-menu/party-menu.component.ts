import { Component, EventEmitter, OnDestroy, OnInit, Output, ViewEncapsulation } from "@angular/core";
import { Subscription } from "rxjs";
import { GroupService } from "src/app/services/group.service";
import { LoadingService } from "src/app/services/loading.service";

@Component({
    selector: 'app-party-menu',
    templateUrl: './party-menu.component.html',
    styleUrls: ['./party-menu.component.scss'],
    encapsulation: ViewEncapsulation.None
})
export class PartyMenuComponent implements OnInit, OnDestroy {

    public waitLaunch = false;

    private isLoading: Subscription;

    constructor(public groupService: GroupService, private loadingService: LoadingService) { }

    ngOnInit(): void {

        this.isLoading = this.loadingService.getLoading().subscribe((isLoading: boolean) => {
            this.waitLaunch = isLoading;
        })
    }

    ngOnDestroy(): void {
        this.isLoading.unsubscribe();
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
}