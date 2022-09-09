import { DragDropModule } from '@angular/cdk/drag-drop';
import { OverlayModule } from '@angular/cdk/overlay';
import { CommonModule } from '@angular/common';
import { ChangeDetectionStrategy, Component, OnInit, Renderer2 } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { FontAwesomeModule } from '@fortawesome/angular-fontawesome';
import { faArrowsUpDownLeftRight, faCode } from '@fortawesome/free-solid-svg-icons';
import { TranslocoService } from '@ngneat/transloco';
import { BehaviorSubject, of, takeUntil, timer } from 'rxjs';
import { switchMap } from 'rxjs/operators';
import { fadeInOutAnimation } from '../../animations/fade-in-out.animation';
import { ClientService } from '../../services/client.service';
import { DestroyService } from '../../services/destroy.service';
import { StoreService } from '../../services/store.service';
import { MockClientService } from '../mock-client.service';


@Component({
  selector: 'app-debug-helper',
  templateUrl: './debug-helper.component.html',
  styleUrls: ['./debug-helper.component.scss'],
  animations: [fadeInOutAnimation],
  providers: [DestroyService],
  changeDetection: ChangeDetectionStrategy.OnPush,
  standalone: true,
  imports: [
    CommonModule,
    FontAwesomeModule,
    DragDropModule,
    OverlayModule,
    FormsModule,
  ],
})
export class DebugHelperComponent implements OnInit {

  readonly faCode = faCode;
  readonly faArrowsUpDownLeftRight = faArrowsUpDownLeftRight;

  visible = new BehaviorSubject(false);
  backgroundClass = 'dark';
  randomizeDebug = new BehaviorSubject(false);

  players$ = this.mockClientService.skyrimtogether.players$;
  connected$ = this.clientService.connectionStateChange.asObservable();

  constructor(
    private readonly destroy$: DestroyService,
    private readonly translocoService: TranslocoService,
    private readonly renderer: Renderer2,
    private readonly storeService: StoreService,
    private readonly mockClientService: MockClientService,
    private readonly clientService: ClientService,
  ) {
  }

  ngOnInit() {
    this.setBackground(this.storeService.get('debug_background', 'dark'));
    this.visible.next(this.storeService.get('debug_helper', false) === 'true');
    this.randomizeDebug.asObservable()
      .pipe(
        takeUntil(this.destroy$),
        switchMap((enabled) => enabled ? timer(0, 1000) : of(null)),
      )
      .subscribe((val) => {
        if (val !== null) {
          this.mockClientService.skyrimtogether.updateMockDebugData();
        }
      });
  }

  toggleVisibility() {
    this.visible.next(!this.visible.getValue());
    this.storeService.set('debug_helper', this.visible.getValue());
  }

  setBackground(newClass: string) {
    const rootEl = document.getElementsByTagName('app-root')[0];
    this.renderer.removeClass(rootEl, this.backgroundClass);
    this.renderer.addClass(rootEl, newClass);
    this.backgroundClass = newClass;
    this.storeService.set('debug_background', newClass);
  }

  addMockPlayer() {
    this.mockClientService.skyrimtogether.addMockPlayer();
  }

  disconnectMockPlayer(playerId: number) {
    this.mockClientService.skyrimtogether.disconnectMockPlayer(playerId);
  }

  accteptMockPlayerInvite(playerId: number) {
    this.mockClientService.skyrimtogether.accteptMockPlayerInvite(playerId);
  }

  inviteToPlayerMockParty(playerId: number) {
    this.mockClientService.skyrimtogether.inviteToPlayerMockParty(playerId);
  }

  startPlayerMockParty(playerId: number) {
    this.mockClientService.skyrimtogether.startPlayerMockParty(playerId);
  }

  mockPlayerLeaveParty(playerId: number) {
    this.mockClientService.skyrimtogether.mockPlayerLeaveParty(playerId);
  }
}
