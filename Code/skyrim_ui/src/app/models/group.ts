import { Player } from "./player";


export enum State {
  None = "none",
  Requested = "requested",
  Running = "running"
}

export class Group {
  id: string;
  state: State;
  owner: Player | undefined;
  members: Map<number, Player>;

  constructor(options: {
    id?: string,
    state?: State,
    owner?: Player,
    members?: Map<number, Player>
  } = {}) {
    this.id = options.id || '';
    this.state = options.state || State.None;
    this.owner = options.owner || undefined;
    this.members = options.members || new Map([]);
  }
}
