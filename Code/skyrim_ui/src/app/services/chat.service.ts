import { Injectable } from '@angular/core';
import { BehaviorSubject, ReplaySubject } from 'rxjs';
import { CommandHandler } from './chat/commands';

export enum MessageTypes {
  SYSTEM_MESSAGE = 0,
  GLOBAL_CHAT = 1,
  PLAYER_DIALOGUE = 2,
  PARTY_MESSAGE = 3,
  LOCAL_MESSAGE = 4
}

export interface ChatMessage {
  senderName?: string;
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
  sendMessage(type: MessageTypes, content: string) {
    if (content.startsWith(this.CommandHandler.COMMAND_PREFIX)) {
      this.CommandHandler.tryExecute(content);
    } else {
      console.log("send msg")
      skyrimtogether.sendMessage(type, content);
    }
  }

  /**
   * Only adds the message to the local message list, without sending anything to the Server.
   */
  pushMessage(message: ChatMessage) {
    this.messageList.next(message);
  }

  pushSystemMessage(content: string) {
    this.pushMessage({type: MessageTypes.SYSTEM_MESSAGE, content})
  }

  private onMessageRecieved(type: MessageTypes, content: string, senderName: string | undefined = undefined): void {
    console.log("reciv msg")
    this.messageList.next({ type, content, senderName });
  }

  private CommandHandler: CommandHandler

  constructor ( 
    ) {
    console.log("sky", skyrimtogether)
    skyrimtogether.on('message', this.onMessageRecieved.bind(this));
    this.CommandHandler = new CommandHandler(this)
  }  
}