import { Component, OnInit } from '@angular/core';
import {
  faArrowDown,
  faArrowUp,
  faWifi,
  IconDefinition,
} from '@fortawesome/free-solid-svg-icons';
import { BehaviorSubject, Observable, startWith, takeUntil } from 'rxjs';
import { DestroyService } from 'src/app/services/destroy.service';
import { PartyAnchor, SettingService } from 'src/app/services/setting.service';
import { Debug } from '../../models/debug';
import { ClientService } from '../../services/client.service';

interface DebugPosition {
  top?: string;
  right?: string;
  bottom?: string;
  left?: string;
}

@Component({
  selector: 'app-debug',
  templateUrl: './debug.component.html',
  styleUrls: ['./debug.component.scss'],
  providers: [DestroyService],
})
export class DebugComponent implements OnInit {
  /* ### ICONS ### */
  faWifi: IconDefinition = faWifi;
  faArrowDown: IconDefinition = faArrowDown;
  faArrowUp: IconDefinition = faArrowUp;

  isShown$: Observable<boolean>;
  debugData$: Observable<Debug>;
  positionStyle$ = new BehaviorSubject<DebugPosition>({
    top: '0',
    left: '0',
  });

  constructor(
    private readonly destroy$: DestroyService,
    private readonly client: ClientService,
    private readonly settingService: SettingService,
  ) {
    this.isShown$ = this.settingService.settings.isDebugShown;
    this.debugData$ = this.client.debugDataChange.pipe(startWith(new Debug()));
  }

  ngOnInit(): void {
    this.settingService.settings.debugAnchor
      .pipe(takeUntil(this.destroy$))
      .subscribe(anchor => {
        const newPosition: DebugPosition = {};

        switch (anchor) {
          case PartyAnchor.TOP_LEFT:
            newPosition.top = '0';
            newPosition.left = '0';
            break;
          case PartyAnchor.TOP_RIGHT:
            newPosition.top = '0';
            newPosition.right = '0';
            break;
          case PartyAnchor.BOTTOM_RIGHT:
            newPosition.bottom = '0';
            newPosition.right = '0';
            break;
          case PartyAnchor.BOTTOM_LEFT:
            newPosition.bottom = '0';
            newPosition.left = '0';
            break;
        }

        this.positionStyle$.next(newPosition);
      });
  }
}
