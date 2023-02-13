export class LanServer {
  ip: string;
  port: number;

  constructor(options: { ip: string; port: number }) {
    this.ip = options.ip;
    this.port = options.port;
  }
}
