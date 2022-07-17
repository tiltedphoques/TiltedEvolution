import { AfterViewChecked, Component, ElementRef, HostListener, QueryList, ViewChild, ViewChildren } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { firstValueFrom, takeUntil } from 'rxjs';
import { environment } from '../../../environments/environment';
import { ClientService, Message } from '../../services/client.service';
import { DestroyService } from '../../services/destroy.service';
import { Sound, SoundService } from '../../services/sound.service';
import { MessageHistory } from './message-history';

interface ChatMessage extends Message {
  date: number;
  odd: boolean;
}

@Component({
  selector: 'app-chat',
  templateUrl: './chat.component.html',
  styleUrls: ['./chat.component.scss'],
  providers: [DestroyService],
})
export class ChatComponent implements AfterViewChecked {

  public padding = 0;

  public message = '';
  public messages = [] as ChatMessage[];

  private odd = false;
  private autoScroll = true;
  private newMessage = false;

  private scrollBack = 0;

  private history = new MessageHistory({maxHistoryLength: 50});

  private get maxScroll(): number {
    return this.logRef.nativeElement.scrollHeight - this.logRef.nativeElement.clientHeight;
  }

  @ViewChild('input') private inputRef!: ElementRef;
  @ViewChildren('entry') private entryRefQuery!: QueryList<ElementRef>;
  @ViewChild('log') private logRef!: ElementRef;

  public constructor(
    private readonly destroy$: DestroyService,
    private readonly client: ClientService,
    private readonly sound: SoundService,
    private readonly translocoService: TranslocoService,

  ) {
    client.messageReception
      .pipe(takeUntil(this.destroy$))
      .subscribe(message => {
        this.messages.push({ ...message, date: Date.now(), odd: this.odd });

        if (this.messages.length > 100) {
          if (this.entryRefQuery && this.entryRefQuery.first) {
            const entryElem = this.entryRefQuery.first.nativeElement;

            this.messages.splice(0, this.messages.length - 100);
            this.scrollBack = entryElem.getBoundingClientRect().height;
          } else {
            // We still delete the first message
            this.messages.shift();
          }
        }

        this.odd = !this.odd;
        this.newMessage = true;

        this.sound.play(Sound.Message);
      });

    client.activationStateChange
      .pipe(takeUntil(this.destroy$))
      .subscribe(state => {
        if (!state && this.inputRef) {
          this.inputRef.nativeElement.blur();
          this.message = '';
        }
      });
  }

  public ngAfterViewChecked(): void {
    if (this.newMessage) {
      if (this.autoScroll) {
        this.logScroll();
      } else if (this.logRef.nativeElement.scrollTop !== this.maxScroll) {
        this.scrollBack += this.padding;

        this.logRef.nativeElement.scrollTop -= Math.floor(this.scrollBack);

        setTimeout(() => {
          this.padding = this.scrollBack % 1;
          this.scrollBack = 0;
        }, 0);
      }

      this.newMessage = false;
    }
  }

  async sendMessage(): Promise<void> {
    if (this.message) {
      if (this.message.length > environment.chatMessageLengthLimit) {
        const content = await firstValueFrom(
          this.translocoService.selectTranslate<string>(
            'COMPONENT.CHAT.MESSAGE_TOO_LONG',
            { chatMessageLengthLimit: environment.chatMessageLengthLimit },
          ),
        );
        this.client.messageReception.next({ content });
        return;
      }

      this.client.sendMessage(this.message);
      this.sound.play(Sound.Focus);

      this.history.push(this.message)

      this.message = '';
    }

    this.focusMessage();
  }

  public onLogScroll(): void {
    this.autoScroll = this.maxScroll - this.logRef.nativeElement.scrollTop < 3;
  }

  public logScroll(): void {
    this.logRef.nativeElement.scrollTop = this.maxScroll;
  }

  public focus(): void {
    this.focusMessage();
  }

  public blur(): void {
    this.inputRef.nativeElement.blur();
  }

  private focusMessage(): void {
    this.inputRef.nativeElement.focus();
  }

  @HostListener('keydown.ArrowUp', ['$event'])
  private onArrowUp(event: KeyboardEvent) {
    event.preventDefault();
    event.stopPropagation();

    this.message = this.history.prev(this.message)
  }

  @HostListener('keydown.ArrowDown', ['$event'])
  private onArrowDown(event: KeyboardEvent) {
    event.preventDefault();
    event.stopPropagation();

    this.message = this.history.next()
  }

}
