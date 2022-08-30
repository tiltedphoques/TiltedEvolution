import { ChatService, MessageTypes } from "../chat.service";

export interface Command {
  readonly name: string,
  readonly executor: (args: string[]) => Promise<void>
}

export class CommandHandler {

  private Help: Command = { name: 'help', executor: async () => {
    const cmds = [...this.commands.keys()].join(', ');
    this.chatService.pushSystemMessage('SERVICE.COMMANDS.AVAILABLE_COMMANDS', {cmds});
  }}

  private readonly commands = new Map<string, Command>;

  public constructor (
    private readonly chatService: ChatService
  ) {
    this.register(this.Help);
  }

  public readonly COMMAND_PREFIX = '/';

  public register(cmd: Command ) {
    if (!this.commands.has(cmd.name)) {
      this.commands.set(cmd.name, cmd);
    }
  }

  public async tryExecute(input: string) {
    const inputWithoutPrefix = input.slice(this.COMMAND_PREFIX.length);
    const [commandName, ...args] = inputWithoutPrefix.split(' ');
    const command = this.commands.get(commandName);
    if(command) {
      command.executor(args);
    } else {
      this.chatService.pushSystemMessage('SERVICE.COMMANDS.COMMAND_NOT_FOUND', {cmd: inputWithoutPrefix});
    }
  }

}
