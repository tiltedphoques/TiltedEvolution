import { ComponentPortal } from '@angular/cdk/portal';
import { ApplicationRef, Injectable } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { first, takeUntil, timer } from 'rxjs';
import { filter, map } from 'rxjs/operators';
import { RootComponent } from '../components/root/root.component';
import { ClientService } from '../services/client.service';
import { DestroyService } from '../services/destroy.service';
import { PopupNotificationService } from '../services/popup-notification.service';
import { UiRepository } from '../store/ui.repository';
import { DebugHelperComponent } from './debug-helper/debug-helper.component';
import { SkyrimtogetherMock } from './skyrimtogether.mock';


@Injectable({
  providedIn: 'root',
})
export class MockClientService extends DestroyService {

  public readonly skyrimtogether: SkyrimtogetherMock = skyrimtogether as SkyrimtogetherMock;

  constructor(
    private readonly clientService: ClientService,
    private readonly popupNotificationService: PopupNotificationService,
    private readonly uiRepository: UiRepository,
    private readonly translocoService: TranslocoService,
    private readonly appRef: ApplicationRef,
  ) {
    super();

    this.skyrimtogether.initMock();

    timer(100)
      .pipe(
        takeUntil(this),
        map(() => this.appRef.components[0]?.instance),
        filter((overlay) => !!overlay),
        first(),
      )
      .subscribe((root: RootComponent) => this.init(root));
  }

  private init(root: RootComponent) {
    const overlayRef = root.overlay.create();
    const userProfilePortal = new ComponentPortal(DebugHelperComponent);
    overlayRef.attach(userProfilePortal);
  }

}
