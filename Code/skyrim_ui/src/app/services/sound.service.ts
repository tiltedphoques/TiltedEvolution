import { Injectable } from '@angular/core';
import { AssetService, AudioAsset, Kind } from './asset.service';


export enum Sound {
  /** Cancellation. */
  Cancel = 'cancel',

  /** Checkbox activation. */
  Check = 'check',

  /** Failure. */
  Fail = 'fail',

  /** Focus change. */
  Focus = 'focus',

  /** Success. */
  Success = 'success',

  /** Message reception. */
  Message = 'message',

  /** Affirmation. */
  Ok = 'ok',

  /** Checkbox deactivation. */
  Uncheck = 'uncheck',

  /** Player joined group. */
  PlayerJoined = 'playerjoined'
}

/** Sound player service. */
@Injectable({
  providedIn: 'root',
})
export class SoundService {
  /**
   * Instantiate.
   *
   * @param asset Asset service.
   */
  public constructor(
    private readonly asset: AssetService,
  ) {
    this.sounds = new Map(Object.entries(Sound).map((entry): [string, Symbol] => {
      return [entry[1], asset.load(Kind.Audio, `assets/sounds/${ entry[1] }.wav`)];
    }));
  }

  /**
   * Play sound file.
   *
   * @param sound Audio file to play.
   */
  public play(sound: string | Sound): void {
    this.asset.get<AudioAsset>(this.sounds.get(sound)!).subscribe(asset => asset.play());
  }

  /** Stored sound assets. */
  private sounds: Map<string, Symbol>;
}
