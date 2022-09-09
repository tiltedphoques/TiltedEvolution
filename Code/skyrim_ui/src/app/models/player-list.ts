import { Player } from './player';


export class PlayerList {
  players: Array<Player>;

  constructor(options: {
    players?: Array<Player>
  } = {}) {
    this.players = options.players || new Array<Player>();
  }
}
