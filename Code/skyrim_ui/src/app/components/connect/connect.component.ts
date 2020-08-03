import {
  Component, ViewEncapsulation, Output, EventEmitter,
  OnDestroy, HostListener, ViewChild, ElementRef, AfterViewInit
} from '@angular/core';

import { Subscription } from 'rxjs';

import { ClientService } from '../../services/client.service';
import { SoundService, Sound } from '../../services/sound.service';
import { ErrorService } from '../../services/error.service';

@Component({
  selector: 'app-connect',
  templateUrl: './connect.component.html',
  styleUrls: [ './connect.component.scss' ],
  encapsulation: ViewEncapsulation.None
})
export class ConnectComponent implements OnDestroy, AfterViewInit {
  @Output()
  public done = new EventEmitter();
  @Output()
  public setView = new EventEmitter<string>();

  public address = '';
  public token = '';

  public connecting = false;

  public constructor(
    private client: ClientService,
    private sound: SoundService,
    private errorService: ErrorService
  ) {
    this.connectionSubscription = this.client.connectionStateChange.subscribe(state => {
      if (this.connecting) {
        this.connecting = false;

        if (state) {
          this.sound.play(Sound.Success);
          this.done.next();
        }
        else {
          this.sound.play(Sound.Fail);

          this.errorService.error(
            'Could not connect to the specified server. Please make sure you\'ve entered the ' +
            'correct address and that you\'re not experiencing network issues.'
          );
        }
      }
    });

    this.protocolMismatchSubscription = this.client.protocolMismatchChange.subscribe(state => {
      if (state) {
        this.connecting = false;
        this.errorService.error("You cannot connect to the server because it does not have the same version of Skyrim Together as you.");
      }
    })
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

  public connect(): void {
    const address = this.address.trim().match(/^(.+?)(?::([0-9]+))?$/);

    if (!address) {
      this.sound.play(Sound.Fail);
      this.errorService.error('The address is of invalid format.');
      return;
    }

    this.connecting = true;

    this.sound.play(Sound.Ok);
    this.client.connect(address[1], address[2] ? Number.parseInt(address[2]) : 10578, this.token);
  }

  public cancel(): void {
    this.sound.play(Sound.Cancel);
    this.done.next();
  }

  public openServerList(): void {
    this.setView.next("serverList");
  }

  @ViewChild('input')
  private inputRef!: ElementRef;

  private connectionSubscription: Subscription;

  private protocolMismatchSubscription: Subscription;

  @HostListener('window:keydown.escape', [ '$event' ])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    if (this.errorService.error$.value) {
      this.errorService.removeError();
    }
    else {
      this.done.next();
    }

    event.stopPropagation();
    event.preventDefault();
  }
}
