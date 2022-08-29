import { ChatService, MessageTypes } from "../chat.service";

export interface Command {
  readonly name: string,
  readonly executor: (args: string[]) => Promise<void>
}

export class CommandHandler {

  private Help: Command = { name: 'help', executor: async () => {
    const cmds = [...this.commands.keys()].join(', ');
    this.chatService.pushSystemMessage(`Available commands: ${cmds}`);
  }}

  private PartyChat: Command = { name: 'party', executor: async (args) => {
    const content = args.join(' ');
    this.chatService.sendMessage(MessageTypes.PARTY_MESSAGE, content);
  }}

  private LocalChat: Command = { name: 'local', executor: async (args) => {
    const content = args.join(' ');
    this.chatService.sendMessage(MessageTypes.LOCAL_MESSAGE, content);
  }}

  private readonly commands: Map<string, Command>;

  public readonly COMMAND_PREFIX = '/';

  private readonly chatService

  public constructor (
    chatService: ChatService
  ) {
    this.chatService = chatService;

    this.commands = new Map<string, Command>();
    this.register(this.Help);
    this.register(this.PartyChat);
    this.register(this.LocalChat);
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
      this.chatService.pushSystemMessage(`${commandName} is not a recognized command`);
    }
  }

}
