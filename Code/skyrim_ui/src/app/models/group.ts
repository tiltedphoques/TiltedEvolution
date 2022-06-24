import { Player } from "./player";

export class Group {
  isEnabled: boolean;
  owner: number;
  members: Array<number>;

  constructor(options: {
    isEnabled?: boolean,
    owner?: number,
    members?: Array<number>
  } = {}) {
    this.isEnabled = options.isEnabled || false;
    // TODO: this may be problematic, since the owner id can be 0
    this.owner = options.owner || undefined;
    this.members = options.members || new Array<number>();
  }
}
