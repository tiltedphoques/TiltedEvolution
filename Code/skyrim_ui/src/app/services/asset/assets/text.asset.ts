import { Observable, of } from 'rxjs';

import { Asset } from '../asset';


export class TextAsset extends Asset<string> {
  public static decode(buffer: ArrayBuffer): Observable<TextAsset> {
    return of(new TextAsset(TextAsset.decoder.decode(buffer)));
  }

  public constructor(content: string) {
    super(content);
  }

  public get text(): string {
    return this.content;
  }

  private static decoder = new TextDecoder();
}
