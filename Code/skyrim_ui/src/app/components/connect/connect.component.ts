import { AfterViewInit, Component, ElementRef, EventEmitter, HostListener, OnDestroy, Output, ViewChild, ViewEncapsulation } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { firstValueFrom, Subscription } from 'rxjs';
import { ClientService } from '../../services/client.service';
import { ErrorService } from '../../services/error.service';
import { Sound, SoundService } from '../../services/sound.service';
import { StoreService } from '../../services/store.service';
import { RootView } from '../root/root.component';


@Component({
  selector: 'app-connect',
  templateUrl: './connect.component.html',
  styleUrls: ['./connect.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class ConnectComponent implements OnDestroy, AfterViewInit {

  public address = '';
  public token = '';
  public savePassword = false;

  public connecting = false;

  @Output() public done = new EventEmitter<void>();
  @Output() public setView = new EventEmitter<RootView>();

  public constructor(
    private readonly client: ClientService,
    private readonly sound: SoundService,
    private readonly errorService: ErrorService,
    private readonly storeService: StoreService,
    private readonly translocoService: TranslocoService,
  ) {
    this.connectionSubscription = this.client.connectionStateChange.subscribe(async state => {
      if (this.connecting) {
        this.connecting = false;

        if (state) {
          this.sound.play(Sound.Success);
          this.done.next();
        } else {
          this.sound.play(Sound.Fail);

          const message = await firstValueFrom(
            this.translocoService.selectTranslate<string>('COMPONENT.CONNECT.ERROR.CONNECTION'),
          );
          this.errorService.error(message);
        }
      }
    });

    this.protocolMismatchSubscription = this.client.protocolMismatchChange.subscribe(async state => {
      if (state) {
        this.connecting = false;
        const message = await firstValueFrom(
          this.translocoService.selectTranslate<string>('COMPONENT.CONNECT.ERROR.VERSION_MISMATCH'),
        );
        this.errorService.error(message);
      }
    });

    this.address = this.storeService.get('last_connected_address', '');
    this.token = this.storeService.get('last_connected_token', '');
    this.savePassword = !!this.storeService.get('last_connected_token', null);
  }

  public ngAfterViewInit(): void {
    setTimeout(() => {
      this.inputRef.nativeElement.focus();
    }, 100);
  }

  public ngOnDestroy(): void {
    this.connectionSubscription.unsubscribe();
    this.protocolMismatchSubscription.unsubscribe();
  }

  async connect(): Promise<void> {
    const address = this.address.trim().match(/^(.+?)(?::([0-9]+))?$/);

    if (!address) {
      this.sound.play(Sound.Fail);
      const message = await firstValueFrom(
        this.translocoService.selectTranslate('COMPONENT.CONNECT.ERROR.INVALID_ADDRESS'),
      );
      this.errorService.error(message);
      return;
    }

    this.connecting = true;

    this.storeService.set('last_connected_address', this.address);
    if (this.savePassword) {
      this.storeService.set('last_connected_token', this.token);
    } else {
      this.storeService.remove('last_connected_token');
    }

    this.sound.play(Sound.Ok);
    this.client.connect(address[1], address[2] ? Number.parseInt(address[2]) : 10578, this.token);
  }

  public cancel(): void {
    this.sound.play(Sound.Cancel);
    this.done.next();
  }

  public openServerList(): void {
    this.setView.next(RootView.SERVER_LIST);
  }

  @ViewChild('input')
  private inputRef!: ElementRef;

  private connectionSubscription: Subscription;

  private protocolMismatchSubscription: Subscription;

  @HostListener('window:keydown.escape', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    if (this.errorService.error$.getValue()) {
      this.errorService.removeError();
    } else {
      this.done.next();
    }

    event.stopPropagation();
    event.preventDefault();
  }
}
