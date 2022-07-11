import { HttpClient, HttpEventType, HttpRequest } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { AsyncSubject, Observable, of, Subscription } from 'rxjs';
import { delay, filter, map } from 'rxjs/operators';
import { Asset } from './asset/asset';
import { AssetDecoder } from './asset/asset-decoder';
import { AudioAssetDecoder } from './asset/asset-decoders/audio.asset-decoder';
import { TextAssetDecoder } from './asset/asset-decoders/text.asset-decoder';
import { AudioAsset } from './asset/assets/audio.asset';
import { TextAsset } from './asset/assets/text.asset';
import { SettingService } from './setting.service';


export { TextAsset, AudioAsset };

/** Current state of the asset. */
export enum State {
  /** A request for the asset is being sent. */
  Requesting,

  /** A request for the asset has been sent. */
  Requested,

  /** The asset has started downloading. */
  Loading,

  /** The asset is being decoded. */
  Decoding,

  /** The asset has finished downloading and decoding. */
  Done,

  /** An error has occured while loading the asset. */
  Error
}

/** Progress of asset loading. */
export interface Progress {
  /** Current state of asset. */
  state: State;

  /** Progress of download. Ranges from `0` to `1`. Only set if `state` equals `State.Loading`. */
  progress?: number;

  /** Asset identifier. */
  symbol: Symbol;
}

/** Asset kinds. */
export enum Kind {
  /** Unicode text asset. */
  Text,

  /** Audio asset. */
  Audio
}

/** Stored asset. */
interface StoredAsset<T extends Asset<any>> {
  /** URL at which the asset is available. */
  href: string;

  /** Asset. Only set if fully loaded and decoded. */
  asset?: T;

  /** Reference counter. */
  count: number;

  /** HTTP request for asset. */
  request: Subscription;
}

/** Asset manager service. */
@Injectable({
  providedIn: 'root',
})
export class AssetService {

  public progress = new AsyncSubject<Progress>();

  /** Cached assets. */
  private cache = new Map<string, Symbol>();

  /** Stored assets. */
  private assets = new Map<Symbol, StoredAsset<any>>();

  /** Asset decoders. */
  private decoders = new Map<Kind, AssetDecoder<any>>([
    [Kind.Text, new TextAssetDecoder()],
    [Kind.Audio, new AudioAssetDecoder(this.settingService)],
  ]);

  public constructor(
    private readonly http: HttpClient,
    private readonly settingService: SettingService,
  ) {
  }

  /**
   * Load and store a text file.
   *
   * @param kind Kind of asset to load.
   * @param path Absolute or relative path to the asset file.
   *
   * @return Asset identifier.
   */
  public load(kind: Kind.Text, path: string): Symbol;

  /**
   * Load and store a audio file.
   *
   * @param kind Kind of asset to load.
   * @param path Absolute or relative path to the asset file.
   *
   * @return Asset identifier.
   */
  public load(kind: Kind.Audio, path: string): Symbol;

  /**
   * Load and store an asset file.
   *
   * @param kind Kind of asset to load.
   * @param path Absolute or relative path to the asset file.
   *
   * @return Asset identifier.
   */
  public load(kind: Kind, path: string): Symbol {
    const href = new URL(path, location.href).href;

    if (this.cache.has(href)) {
      return this.cache.get(href)!;
    }

    const get = new HttpRequest('GET', href, {
      reportProgress: true,
      responseType: 'arraybuffer',
    });

    const request = this.http.request<ArrayBuffer>(get).pipe(delay(0)).subscribe(event => {
      if (event.type === HttpEventType.Sent) {
        this.progress.next({ state: State.Requested, symbol });
      } else if (event.type === HttpEventType.DownloadProgress) {
        this.progress.next({
          state: State.Loading,
          progress: event.total ? event.loaded / event.total : 0,
          symbol,
        });
      } else if (event.type === HttpEventType.Response) {
        this.decoders.get(kind)!.decode(event.body!).subscribe(asset => {
          stored.asset = asset;

          this.progress.next({ state: State.Done, symbol });
          this.progress.complete();
        });

        this.progress.next({ state: State.Decoding, symbol });
      }
    }, () => this.progress.next({ state: State.Error, symbol }));

    const symbol = Symbol();
    const stored: StoredAsset<any> = { href, count: 1, request };

    this.assets.set(symbol, stored);
    this.cache.set(href, symbol);

    this.progress.next({ state: State.Requesting, symbol });

    return symbol;
  }

  /**
   * Unload an asset.
   *
   * If there are multiple references to this asset, the internal counter is decreased.
   * Otherwise, the asset is unloaded and dropped from cache entirely.
   *
   * @param symbol Asset identifier.
   */
  public unload(symbol: Symbol): void {
    const asset = this.assets.get(symbol);

    if (asset && !--asset.count) {
      asset.request.unsubscribe();

      this.assets.delete(symbol);
      this.cache.delete(asset.href);
    }
  }

  /**
   * Retrieve a loaded text asset.
   *
   * Returns `undefined` if the given asset isn't loaded.
   *
   * @param symbol Asset identifier.
   *
   * @return Text asset.
   */
  public get<TextAsset>(symbol: Symbol): Observable<TextAsset>;

  /**
   * Retrieve a loaded audio asset.
   *
   * Returns `undefined` if the given asset isn't loaded.
   *
   * @param symbol Asset identifier.
   *
   * @return Audio asset.
   */
  public get<AudioAsset>(symbol: Symbol): Observable<AudioAsset>;

  /**
   * Retrieve a loaded asset.
   *
   * Returns `undefined` if the given asset isn't loaded.
   *
   * @param symbol Asset identifier.
   *
   * @return Asset.
   */
  public get<T extends Asset<any>>(symbol: Symbol): Observable<T> {
    return of(this.assets.get(symbol)).pipe(
      filter(stored => !!stored),
      map(stored => stored!.asset!),
      filter<T>(asset => !!asset),
    );
  }

}
