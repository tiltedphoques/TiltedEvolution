import { Observable } from 'rxjs';

import { Asset } from './asset';


export abstract class AssetDecoder<T extends Asset<any>> {
  public abstract decode(buffer: ArrayBuffer): Observable<T>;
}
