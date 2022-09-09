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
    this.owner = options.owner;
    this.members = options.members || new Array<number>();
  }
}
