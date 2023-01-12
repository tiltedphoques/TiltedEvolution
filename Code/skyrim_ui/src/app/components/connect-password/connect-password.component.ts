import { AfterViewInit, Component, ElementRef, EventEmitter, HostListener, OnDestroy, Output, ViewChild } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { firstValueFrom, Subscription } from 'rxjs';
import { View } from '../../models/view.enum';
import { ClientService } from '../../services/client.service';
import { ErrorService } from '../../services/error.service';
import { Sound, SoundService } from '../../services/sound.service';
import { StoreService } from '../../services/store.service';
import { UiRepository } from '../../store/ui.repository';


@Component({
  selector: 'app-connect-password',
  templateUrl: './connect-password.component.html',
  styleUrls: ['./connect-password.component.scss'],
})
export class ConnectPasswordComponent implements OnDestroy, AfterViewInit {

  public address = '';
  public name = '';
  public port = 10578;
  public password = '';
  public savePassword = false;
  public hidePassword = true;

  public connecting = false;

  @Output() public done = new EventEmitter<void>();

  public constructor(
    private readonly client: ClientService,
    private readonly sound: SoundService,
    private readonly errorService: ErrorService,
    private readonly storeService: StoreService,
    private readonly translocoService: TranslocoService,
    private readonly uiRepository: UiRepository,
  ) {
    this.connectionSubscription = this.client.connectionStateChange.subscribe(async state => {
      if (this.connecting) {
        this.connecting = false;

        if (state) {
          this.sound.play(Sound.Success);
          this.done.next();
        } else if (this.errorService.getError() === '') { // show connection error when there is no more specific error
          const message = await firstValueFrom(
            this.translocoService.selectTranslate<string>('COMPONENT.CONNECT.ERROR.CONNECTION'),
          );
          await this.errorService.setError(message);
        }
      }
    });

    this.protocolMismatchSubscription = this.client.protocolMismatchChange.subscribe(async state => {
      if (state) {
        this.connecting = false;
        const message = await firstValueFrom(
          this.translocoService.selectTranslate<string>('COMPONENT.CONNECT.ERROR.VERSION_MISMATCH'),
        );
        await this.errorService.setError(message);
      }
    });

    this.name = this.uiRepository.getConnectName();
    this.address = this.uiRepository.getConnectIp();
    this.port = this.uiRepository.getConnectPort();
    this.password = this.getStoredPasswordForAddress(this.address, this.port);
    this.savePassword = this.password !== '';
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
    if (!this.address) {
      this.sound.play(Sound.Fail);
      const message = await firstValueFrom(
        this.translocoService.selectTranslate('COMPONENT.CONNECT.ERROR.INVALID_ADDRESS'),
      );
      await this.errorService.setError(message);
      return;
    }

    this.connecting = true;

    this.sound.play(Sound.Ok);
    if (this.savePassword) {
      this.storePasswordLocally(this.address, this.port, this.password);
    }

    this.client.connect(this.address, this.port, this.password);
  }

  public cancel(): void {
    this.uiRepository.openView(View.SERVER_LIST);
  }

  private getStoredPasswordForAddress(ip: string, port: number): string {
    let savedServerList = JSON.parse(this.storeService.get('savedServerList', '[]'));
    let savedServer = savedServerList.find(saved => saved.ip === ip && saved.port === port);

    if (savedServer) {
      return savedServer.password;
    } else {
      return '';
    }
  }

  private storePasswordLocally(ip: string, port: number, password: string): void {
    let savedServerList = JSON.parse(this.storeService.get('savedServerList', '[]'));
    let savedServer = savedServerList.find(saved => saved.ip === ip && saved.port === port);

    if (savedServer) {
      savedServer.password = password;
    } else {
      savedServerList.push({ip:ip, port:port, password:password});
    }
    this.storeService.set('savedServerList', JSON.stringify(savedServerList));
  }

  @ViewChild('input')
  private inputRef!: ElementRef;

  private connectionSubscription: Subscription;

  private protocolMismatchSubscription: Subscription;

  @HostListener('window:keydown.escape', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    if (this.errorService.getError()) {
      this.errorService.removeError();
    } else {
      this.cancel();
    }

    event.stopPropagation();
    event.preventDefault();
  }
}