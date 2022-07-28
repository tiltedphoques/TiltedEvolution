import { Observable, of } from 'rxjs';

import { AssetDecoder } from '../asset-decoder';
import { TextAsset } from '../assets/text.asset';


export class TextAssetDecoder extends AssetDecoder<TextAsset> {
  public decode(buffer: ArrayBuffer): Observable<TextAsset> {
    return of(new TextAsset(this.decoder.decode(buffer)));
  }

  private decoder = new TextDecoder();
}
