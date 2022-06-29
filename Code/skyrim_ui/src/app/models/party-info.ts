export class PartyInfo {
  playerIds: Array<number>;
  leaderId: number;

  constructor(options: {
    playerIds: Array<number>;
    leaderId: number;
  }) {
    this.playerIds = options.playerIds;
    this.leaderId = options.leaderId;
  }
}