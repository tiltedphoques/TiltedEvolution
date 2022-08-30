import { Injectable } from '@angular/core';
import { ReplaySubject } from 'rxjs';
import { environment } from 'src/environments/environment';
import { Command, CommandHandler } from './chat/commands';

export enum MessageTypes {
  SYSTEM_MESSAGE = 0,
  GLOBAL_CHAT = 1,
  PLAYER_DIALOGUE = 2,
  PARTY_CHAT = 3,
  LOCAL_CHAT = 4
}

export interface ChatMessage {
  senderName?: string;
  translationParams?: Record<string, any>;
  content: string;
  type: MessageTypes;
}

@Injectable({
    providedIn: 'root',
})
export class ChatService {

  public messageList = new ReplaySubject<ChatMessage>();
  
  /**
   * Send chat message to the Server. Does not add anything to the local mesesage list.
   */
  public sendMessage(type: MessageTypes, content: string) {
    if (content.length === 0) {
      return
    }

    if (content.length > environment.chatMessageLengthLimit) {
      this.pushSystemMessage('COMPONENT.CHAT.MESSAGE_TOO_LONG',{ chatMessageLengthLimit: environment.chatMessageLengthLimit });
      return
    } 

    if (content.startsWith(this.CommandHandler.COMMAND_PREFIX)) {
      this.CommandHandler.tryExecute(content);
      return
    }

    skyrimtogether.sendMessage(type, content);
  }

  /**
   * Pushes a message to the chat window, without sending anything to the Server.
   */
  public pushMessage(message: ChatMessage) {
    this.messageList.next(message);
  }

  /**
   * For pushing translateable system messages to the chat window.
   */
  public pushSystemMessage(translationKey: string, params: Record<string, any> = undefined) {
    this.pushMessage({type: MessageTypes.SYSTEM_MESSAGE, content: translationKey, translationParams: params});
  }

  private onMessageRecieved(type: MessageTypes, content: string, senderName: string | undefined = undefined): void {
    this.messageList.next({ type, content, senderName });
  }

  private CommandHandler: CommandHandler;

  private LocalChat: Command = { name: 'local', executor: async (args) => {
    const content = args.join(' ');
    this.sendMessage(MessageTypes.LOCAL_CHAT, content);
  }}

  private PartyChat: Command = { name: 'party', executor: async (args) => {
    const content = args.join(' ');
    this.sendMessage(MessageTypes.PARTY_CHAT, content);
  }}

  constructor () {
    skyrimtogether.on('message', this.onMessageRecieved.bind(this));

    this.CommandHandler = new CommandHandler(this);
    this.CommandHandler.register(this.LocalChat);
    this.CommandHandler.register(this.PartyChat);
  }  
}