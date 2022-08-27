import { Injectable } from '@angular/core';
import { ClientService, MessageType } from './client.service';

export interface Command {
    readonly name: string,
    readonly executor: (args: string[]) => Promise<void>
}

@Injectable({
  providedIn: 'root',
})
export class CommandService {

  private Help: Command = { name: 'help', executor: async () => {
    const cmds = [...this.commands.keys()].join(', ');
    this.client.messageReception.next({ content: `Available commands: ${cmds}`, type: MessageType.SYSTEM_MESSAGE });
  }}

  private PartyChat: Command = { name: 'party', executor: async (args) => {
    const content = args.join(' ');
    this.client.sendMessage({ content, type: MessageType.PARTY_MESSAGE });
  }}

  private readonly commands: Map<string, Command>;

  public readonly COMMAND_PREFIX = '/';

  public constructor (
    private readonly client: ClientService,
  ) {
    this.commands = new Map<string, Command>();
    this.register(this.Help);
    this.register(this.PartyChat);
  }

  public register(cmd: Command ) {
    if (!this.commands.has(cmd.name)) {
        this.commands.set(cmd.name, cmd);
    }
  }

  public async tryExecute(input: string) {
    if (!input.startsWith(this.COMMAND_PREFIX) || input.length < this.COMMAND_PREFIX.length) {
        return;
    }

    const inputWithoutPrefix = input.slice(this.COMMAND_PREFIX.length);
    const [commandName, ...args] = inputWithoutPrefix.split(' ');
    const command = this.commands.get(commandName);
    if(command) {
        command.executor(args);
    } else {
        this.client.messageReception.next({ content: `${commandName} is not a recognized command`, type: MessageType.SYSTEM_MESSAGE });
    }
  }

}
