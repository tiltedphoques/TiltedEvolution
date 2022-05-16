import { Player } from "./player";


export class PlayerList {
  players: Map<number, Player>;

  constructor(options: {
    players?: Map<number, Player>
  } = {}) {
    this.players = options.players || new Map([]);
  }
}
