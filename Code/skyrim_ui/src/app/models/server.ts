export interface Server {
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
}
