import { Component, OnInit, ViewEncapsulation, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';
import { ClientService } from '../../services/client.service';
import { animation as popupsAnimation } from '../root/popups.animation';
import { Debug } from '../../models/debug';
import { StoreService } from '../../services/store.service';
import { faWifi, faArrowDown, faArrowUp, IconDefinition } from "@fortawesome/free-solid-svg-icons";

@Component({
  selector: 'app-debug',
  templateUrl: './debug.component.html',
  styleUrls: ['./debug.component.scss'],
  encapsulation: ViewEncapsulation.None,
  animations: [ popupsAnimation ]
})
export class DebugComponent implements OnInit, OnDestroy {
  isShown = false;
  debugData = new Debug();
  debugState$: Subscription;
  debugData$: Subscription;
  faWifi: IconDefinition = faWifi;
  faArrowDown: IconDefinition = faArrowDown;
  faArrowUp: IconDefinition = faArrowUp;

  constructor(private client: ClientService,
              private storeService: StoreService) {
      this.isShown = JSON.parse(this.storeService.get('debug_isShow', true));
  }

  ngOnInit() {
    this.debugState$ = this.client.debugStateChange.subscribe((state: boolean) => {
      this.storeService.set('debug_isShown', state);
      this.isShown = state;
    });

    this.debugData$ = this.client.debugDataChange.subscribe((data: Debug) => {
      this.debugData = data;
    })
  }

  ngOnDestroy() {
    this.debugState$.unsubscribe();
    this.debugData$.unsubscribe();
  }

  public toggle() {
    this.isShown = !this.isShown;
    this.storeService.set('debug_isShow', this.isShown);
  }

  public get active(): boolean {
    return this.client.activationStateChange.value;
  }
}
