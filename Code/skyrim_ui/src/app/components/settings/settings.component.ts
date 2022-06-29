import { Component, EventEmitter, HostListener, OnDestroy, OnInit, Output, ViewEncapsulation } from "@angular/core";
import { ClientService } from "src/app/services/client.service";
import { SettingService } from "src/app/services/setting.service";

@Component({
    selector: 'app-settings',
    templateUrl: './settings.component.html',
    styleUrls: ['./settings.component.scss'],
    encapsulation: ViewEncapsulation.None,
})
export class SettingsComponent implements OnInit, OnDestroy {

    @Output()
    public done = new EventEmitter();
    @Output()
    public setView = new EventEmitter<string>();

    public volume: number;
    public muted: boolean;
    public showDebug: boolean;
    public autoHideParty: boolean;
    public showParty: boolean;
    public autoHideTime: number;

    constructor(private settings: SettingService, private client: ClientService) {

    }
    ngOnDestroy(): void {

    }
    ngOnInit(): void {
        this.volume = this.settings.getVolume();
        this.muted = this.settings.isAudioMuted();
        this.showDebug = this.settings.isDebugShown();
        this.autoHideParty = this.settings.isPartyAutoHidden();
        this.showParty = this.settings.isPartyShown();
        this.autoHideTime = this.settings.getAutoHideTime();
    }

    public cancel(): void {
        this.done.next();
    }

    onMutedChange(event: any) {
        this.settings.muteAudio(event.target.checked);
    }
    onVolumeChange(event: any) {
        this.settings.setVolume(event.target.value);
    }

    onShowDebugChange(event: any) {
        this.settings.setDebugShown(event.target.checked);
        this.client.debugStateChange.next(event.target.checked);
    }

    onShowPartyChange(event: any) {
        this.settings.showParty(event.target.checked);
    }
    onAutoHidePartyChange(event: any) {
        this.settings.autoHideParty(event.target.checked);
        this.autoHideParty = event.target.checked;
    }
    onAutoHideTimeChange(event: any) {
        this.settings.setAutoHideTime(event.target.value);
        this.autoHideTime = event.target.value;
    }

    public autoHideTimeSelected(number: number): boolean {
        return this.settings.getAutoHideTime() === number;
    }

    private close() {
        this.done.next();
    }

    @HostListener('window:keydown.escape', ['$event'])
    // @ts-ignore
    private activate(event: KeyboardEvent): void {
        this.close();
        event.stopPropagation();
        event.preventDefault();
    }
}
