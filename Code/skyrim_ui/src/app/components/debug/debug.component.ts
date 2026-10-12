import { Component } from '@angular/core';
import {
  faArrowDown,
  faArrowUp,
  faWifi,
  IconDefinition,
} from '@fortawesome/free-solid-svg-icons';
import { map, Observable, startWith } from 'rxjs';
import { PartyAnchor, SettingService } from 'src/app/services/setting.service';
import { Debug } from '../../models/debug';
import { ClientService } from '../../services/client.service';

const anchorStyles: Record<PartyAnchor, Record<string, string>> = {
  [PartyAnchor.TOP_LEFT]: { top: '0', left: '0' },
  [PartyAnchor.TOP_RIGHT]: { top: '0', right: '0' },
  [PartyAnchor.BOTTOM_RIGHT]: { bottom: '0', right: '0' },
  [PartyAnchor.BOTTOM_LEFT]: { bottom: '0', left: '0' },
};

@Component({
  selector: 'app-debug',
  templateUrl: './debug.component.html',
  styleUrls: ['./debug.component.scss'],
})
export class DebugComponent {
  /* ### ICONS ### */
  faWifi: IconDefinition = faWifi;
  faArrowDown: IconDefinition = faArrowDown;
  faArrowUp: IconDefinition = faArrowUp;

  isShown$: Observable<boolean>;
  debugData$: Observable<Debug>;
  positionStyle$: Observable<Record<string, string>>;

  constructor(
    private readonly client: ClientService,
    private readonly settingService: SettingService,
  ) {
    this.isShown$ = this.settingService.settings.isDebugShown;
    this.debugData$ = this.client.debugDataChange.pipe(startWith(new Debug()));
    this.positionStyle$ = this.settingService.settings.debugAnchor.pipe(
      map(anchor => anchorStyles[anchor]),
    );
  }
}
