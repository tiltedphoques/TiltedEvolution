export class PartyInfo {
  serverIds: Array<number>;
  leaderId: number;

  constructor(options: {
    serverIds: Array<number>;
    leaderId: number;

  }) {
    this.serverIds = options.serverIds;
    this.leaderId = options.leaderId;
  }
}