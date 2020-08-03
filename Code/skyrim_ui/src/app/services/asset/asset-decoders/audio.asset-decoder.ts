import { Observable, from as fromPromise } from 'rxjs';
import { map } from 'rxjs/operators';

import { AssetDecoder } from '../asset-decoder';
import { AudioAsset } from '../assets/audio.asset';

export class AudioAssetDecoder extends AssetDecoder<AudioAsset> {
  public decode(buffer: ArrayBuffer): Observable<AudioAsset> {
    return fromPromise(this.context.decodeAudioData(buffer)).pipe(
      map(audioBuffer => new AudioAsset(audioBuffer))
    );
  }

  private context = new AudioContext();
}
