export class Server {
  name: string;
  desc: string;
  icon_url: string;
  version: string;
  ip: string;
  port: number;
  tick: number;
  player_count: number;
  max_player_count: number;
  tags: string;
  ping: number;
  continent: string;
  countryCode: string;
  country: string;
  isFavorite: boolean;

  constructor(options: {
    name: string;
    desc: string;
    icon_url: string;
    version: string;
    ip: string;
    port: number;
    tick: number;
    player_count: number;
    max_player_count: number;
    tags: string;
    ping: number;
    continent: string;
    countryCode: string;
    country: string;
    isFavorite: boolean;
  }) {

    this.name = options.name;
    this.desc = options.desc;
    this.icon_url = options.icon_url;
    this.version = options.version;
    this.ip = options.ip;
    this.port = options.port;
    this.tick = options.tick;
    this.player_count = options.player_count;
    this.max_player_count = options.max_player_count;
    this.tags = options.tags;
    this.ping = options.ping;
    this.continent = options.continent;
    this.countryCode = options.countryCode;
    this.country = options.country;
    this.isFavorite = options.isFavorite;
  }
}
