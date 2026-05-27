export class Group {
  isEnabled: boolean;
  owner: number;
  members: Array<number>;
  allowAutoJoin: boolean;
  serverAutoJoin: boolean;
  partyCount: number;

  constructor(
    options: {
      isEnabled?: boolean;
      owner?: number;
      members?: Array<number>;
      allowAutoJoin?: boolean;
      serverAutoJoin?: boolean;
      partyCount?: number;
    } = {},
  ) {
    this.isEnabled = options.isEnabled || false;
    this.owner = options.owner;
    this.members = options.members || new Array<number>();
    this.allowAutoJoin = options.allowAutoJoin || false;
    this.serverAutoJoin = options.serverAutoJoin || false;
    this.partyCount = options.partyCount || 0;
  }
}
