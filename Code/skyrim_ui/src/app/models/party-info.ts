export class PartyInfo {
  playerIds: Array<number>;
  leaderId: number;
  allowAutoJoin: boolean;
  serverAutoJoin: boolean;
  partyCount: number;

  constructor(options: { playerIds: Array<number>; leaderId: number; allowAutoJoin?: boolean; serverAutoJoin?: boolean; partyCount?: number }) {
    this.playerIds = options.playerIds;
    this.leaderId = options.leaderId;
    this.allowAutoJoin = options.allowAutoJoin || false;
    this.serverAutoJoin = options.serverAutoJoin || false;
    this.partyCount = options.partyCount || 0;
  }
}
