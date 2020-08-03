export class ServerList {
  ip: string;
  port: string;
  playerCount: number;
  lastUpdate: number;
  name: string;
  country: string;
  isOnline: boolean;
  isFavorite: boolean;

  constructor(options: {
    ip: string,
    port: string,
    player_count: number,
    last_update: number,
    name: string,
    country: string,
    isOnline: boolean,
    isFavorite: boolean
  }) {
    this.ip = options.ip;
    this.port = options.port;
    this.playerCount = options.player_count;
    this.lastUpdate = options.last_update;
    this.name = options.name;
    this.country = options.country ? options.country : "";
    this.isOnline = options.isOnline;
    this.isFavorite = options.isFavorite;
  }
}
