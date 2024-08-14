import { ChatService } from '../chat.service';

export interface Command {
  readonly name: string;
  readonly executor: (args: string[]) => Promise<void>;
}

export class CommandHandler {
  private Help: Command = {
    name: 'help',
    executor: async () => {
      const cmds = [...this.commands.keys()].join(', ');
      this.chatService.pushSystemMessage(
        'SERVICE.COMMANDS.AVAILABLE_COMMANDS',
        { cmds },
      );
    },
  }
  
  private SetTime: Command = {
    name: 'settime', 
    executor: async (args) => {
      const cmds = [...this.commands.keys()].join(', ');
      if (args.length != 2) {
        this.chatService.pushSystemMessage(
          'COMPONENT.CHAT.SET_TIME_ARGUMENT_COUNT', 
          { cmds },
        );
        return;
      }
      const hoursNum = parseInt(args[0]);
      const minutesNum = parseInt(args[1]);
      if (hoursNum < 0 || hoursNum > 23 || minutesNum < 0 || minutesNum > 59) {
        this.chatService.pushSystemMessage(
          'COMPONENT.CHAT.SET_TIME_INVALID_ARGUMENTS',
          { cmds },
        );
        return;
      }
      skyrimtogether.setTime(hoursNum, minutesNum);
      // TODO (Toe Knee): Ideally send a localizable response string here,
      // currently relies on user making it themselves with serverside scripting
    },
  }

  private readonly commands = new Map<string, Command>();

  public constructor(private readonly chatService: ChatService) {
    this.register(this.Help);
    this.register(this.SetTime);
  }

  public readonly COMMAND_PREFIX = '/';

  public register(cmd: Command) {
    if (!this.commands.has(cmd.name)) {
      this.commands.set(cmd.name, cmd);
    }
  }

  public async tryExecute(input: string) {
    const inputWithoutPrefix = input.slice(this.COMMAND_PREFIX.length);
    const [commandName, ...args] = inputWithoutPrefix.split(' ');
    const command = this.commands.get(commandName);
    if (command) {
      command.executor(args);
    } else {
      this.chatService.pushSystemMessage('SERVICE.COMMANDS.COMMAND_NOT_FOUND', {
        cmd: commandName,
      });
    }
  }
}
