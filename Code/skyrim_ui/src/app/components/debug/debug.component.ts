import { Component } from '@angular/core';
import {
  faArrowDown,
  faArrowUp,
  faWifi,
  IconDefinition,
} from '@fortawesome/free-solid-svg-icons';
import { map, Observable, startWith } from 'rxjs';
import { ScreenCorner, SettingService } from 'src/app/services/setting.service';
import { Debug } from '../../models/debug';
import { ClientService } from '../../services/client.service';

const cornerClasses: Record<ScreenCorner, string> = {
  [ScreenCorner.TOP_LEFT]: 'top-left',
  [ScreenCorner.TOP_RIGHT]: 'top-right',
  [ScreenCorner.BOTTOM_RIGHT]: 'bottom-right',
  [ScreenCorner.BOTTOM_LEFT]: 'bottom-left',
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
  cornerClass$: Observable<string>;

  constructor(
    private readonly client: ClientService,
    private readonly settingService: SettingService,
  ) {
    this.isShown$ = this.settingService.settings.isDebugShown;
    this.debugData$ = this.client.debugDataChange.pipe(startWith(new Debug()));
    this.cornerClass$ = this.settingService.settings.debugAnchor.pipe(
      map(corner => cornerClasses[corner]),
    );
  }
}
