import { Player } from "./player";

export class Group {
  isEnabled: boolean;
  owner: number;
  members: Map<number, Player>;

  constructor(options: {
    isEnabled?: boolean,
    owner?: number,
    members?: Map<number, Player>
  } = {}) {
    this.isEnabled = options.isEnabled || false;
    this.owner = options.owner || undefined;
    this.members = options.members || new Map([]);
  }
}
