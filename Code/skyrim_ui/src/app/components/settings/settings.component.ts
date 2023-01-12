import { Component, EventEmitter, HostListener, Output } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { lastValueFrom, map, Observable } from 'rxjs';
import { Tag } from 'src/app/models/tag';
import {
  autoHideTimerLengths,
  FontSize,
  PartyAnchor,
  SettingService,
} from 'src/app/services/setting.service';
import { Sound, SoundService } from '../../services/sound.service';
import { environment } from 'src/environments/environment';
import { HttpClient } from '@angular/common/http';
import { ClientService } from 'src/app/services/client.service';

@Component({
  selector: 'app-settings',
  templateUrl: './settings.component.html',
  styleUrls: ['./settings.component.scss'],
})
export class SettingsComponent {
  readonly availableLanguages = this.translocoService.getAvailableLangs();
  readonly availableFontSizes: { id: FontSize; label: string }[] = [
    { id: FontSize.XS, label: 'COMPONENT.SETTINGS.FONT_SIZES.XS' },
    { id: FontSize.S, label: 'COMPONENT.SETTINGS.FONT_SIZES.S' },
    { id: FontSize.M, label: 'COMPONENT.SETTINGS.FONT_SIZES.M' },
    { id: FontSize.L, label: 'COMPONENT.SETTINGS.FONT_SIZES.L' },
    { id: FontSize.XL, label: 'COMPONENT.SETTINGS.FONT_SIZES.XL' },
  ];
  readonly availablePartyAnchors: { id: PartyAnchor; label: string }[] = [
    {
      id: PartyAnchor.TOP_LEFT,
      label: 'COMPONENT.SETTINGS.PARTY_ANCHOR_POSITION.TOP_LEFT',
    },
    {
      id: PartyAnchor.TOP_RIGHT,
      label: 'COMPONENT.SETTINGS.PARTY_ANCHOR_POSITION.TOP_RIGHT',
    },
    {
      id: PartyAnchor.BOTTOM_RIGHT,
      label: 'COMPONENT.SETTINGS.PARTY_ANCHOR_POSITION.BOTTOM_LEFT',
    },
    {
      id: PartyAnchor.BOTTOM_LEFT,
      label: 'COMPONENT.SETTINGS.PARTY_ANCHOR_POSITION.BOTTOM_RIGHT',
    },
  ];
  readonly availableAutoHideTimes = autoHideTimerLengths;

  public settings = this.settingService.settings;
  public autoHideTime: number;
  public partyAnchor: PartyAnchor;
  public partyAnchorOffsetX: number;
  public partyAnchorOffsetY: number;
  public fontSize: FontSize;
  public maxFontSize = Object.values(FontSize).length - 1;
  public minFontSize = 0;

  clientVersion$: Observable<string>;
  isVersionOutdated: Promise<boolean>;

  @Output() public done = new EventEmitter<void>();
  @Output() public settingsUpdated = new EventEmitter<void>();

  constructor(
    private readonly settingService: SettingService,
    private readonly sound: SoundService,
    private readonly translocoService: TranslocoService,
    private readonly http: HttpClient,
    private readonly client: ClientService,
  ) {
    this.clientVersion$ = this.client.versionSet.pipe(map(version => version.split('-')[0]));
  }

  ngOnInit(): void {
    this.isVersionOutdated = this.isGameVersionOutdated();
  }

  close() {
    this.done.next();
    this.sound.play(Sound.Ok);
  }

  private getVersionTagList(): Promise<Tag[]> {
    return lastValueFrom(
      this.http
        .get<Tag[]>(`${ environment.githubUrl }`));
  }

  async isGameVersionOutdated(): Promise<boolean> {
    let usedVersion = this.client.getVersion();

    const tags = await this.getVersionTagList();
    const usedVersionIndex = tags.findIndex(tag => tag.name === usedVersion);

    console.log(usedVersionIndex);
    console.log(usedVersionIndex > 0 || usedVersionIndex === -1)
    console.log(usedVersionIndex > 0 )
    console.log(usedVersionIndex === -1)

    return usedVersionIndex > 0 || usedVersionIndex === -1;
  }

  @HostListener('window:keydown.escape', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    this.close();
    event.stopPropagation();
    event.preventDefault();
  }
}
