import { Observable, from as fromPromise } from 'rxjs';
import { map } from 'rxjs/operators';

import { AssetDecoder } from '../asset-decoder';
import { AudioAsset } from '../assets/audio.asset';
import { SettingService } from '../../setting.service';

export class AudioAssetDecoder extends AssetDecoder<AudioAsset> {

  constructor(private settingService: SettingService) {
    super();
  }

  public decode(buffer: ArrayBuffer): Observable<AudioAsset> {
    return fromPromise(this.context.decodeAudioData(buffer)).pipe(
      map(audioBuffer => new AudioAsset(audioBuffer, this.settingService))
    );
  }

  private context = new AudioContext();
}
