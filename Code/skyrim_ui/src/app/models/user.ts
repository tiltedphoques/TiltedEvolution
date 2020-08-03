import { Player } from "./player";

/** User data. */
export class User {
  /** Session token. */
  token: string;

  /** Friend list. */
  friends: Player[];

  constructor(options: {
    token: string,
    friends?: Player[]
  }) {
    this.token = options.token;
    this.friends = options.friends || [];
  }
}
