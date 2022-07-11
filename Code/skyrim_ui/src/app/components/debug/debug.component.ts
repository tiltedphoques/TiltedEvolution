import { Component, ViewEncapsulation } from '@angular/core';
import { faArrowDown, faArrowUp, faWifi, IconDefinition } from '@fortawesome/free-solid-svg-icons';
import { Observable, startWith } from 'rxjs';
import { SettingService } from 'src/app/services/setting.service';
import { Debug } from '../../models/debug';
import { ClientService } from '../../services/client.service';
import { animation as popupsAnimation } from '../root/popups.animation';


@Component({
  selector: 'app-debug',
  templateUrl: './debug.component.html',
  styleUrls: ['./debug.component.scss'],
  encapsulation: ViewEncapsulation.None,
  animations: [popupsAnimation],
})
export class DebugComponent {

  /* ### ICONS ### */
  faWifi: IconDefinition = faWifi;
  faArrowDown: IconDefinition = faArrowDown;
  faArrowUp: IconDefinition = faArrowUp;

  isShown$: Observable<boolean>;
  debugData$: Observable<Debug>;

  constructor(
    private readonly client: ClientService,
    private readonly settings: SettingService,
  ) {
    this.isShown$ = this.client.debugStateChange.pipe(startWith(this.settings.isDebugShown()));
    this.debugData$ = this.client.debugDataChange.pipe(startWith(new Debug()));
  }

}
