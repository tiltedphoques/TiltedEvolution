import { Component, EventEmitter, HostListener, OnDestroy, OnInit, Output, ViewEncapsulation } from '@angular/core';
import { FormControl } from '@angular/forms';
import { faStar as farStar } from '@fortawesome/free-regular-svg-icons';
import { faStar as fasStar } from '@fortawesome/free-solid-svg-icons';
import { forkJoin, interval, Observable, Subscription } from 'rxjs';
import { debounceTime, distinctUntilChanged, map, switchMap } from 'rxjs/operators';
import { Server } from '../../models/server';
import { ClientService } from '../../services/client.service';
import { ErrorService } from '../../services/error.service';
import { ServerListService } from '../../services/server-list.service';
import { Sound, SoundService } from '../../services/sound.service';
import { StoreService } from '../../services/store.service';


@Component({
  selector: 'app-server-list',
  templateUrl: './server-list.component.html',
  styleUrls: ['./server-list.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class ServerListComponent implements OnInit, OnDestroy {

  @Output()
  public done = new EventEmitter<void>();
  @Output()
  public setView = new EventEmitter<string>();

  // Server list with search / filter
  public serverList: Server[] = [];
  public formSearch = new FormControl('');
  public isIncreasingPlayerOrder = true;
  public isIncreasingCountryOrder = true;
  public isIncreasingNameOrder = true;
  public isIncreasingFavoriteOrder = true;
  public farStar = farStar;
  public fasStar = fasStar;

  public isLoading = true;

  // All serverList data
  private _serverList: Server[] = [];
  private searchSubscription: Subscription;
  private autoRefreshSubscription: Subscription;

  constructor(
    private errorService: ErrorService,
    private serverListService: ServerListService,
    private clientService: ClientService,
    private soundService: SoundService,
    private storeService: StoreService,
  ) {
  }

  ngOnInit() {
    this.updateServerList();
    this.onSearchSubscription();
    this.onAutoRefreshSubcription();
  }

  ngOnDestroy() {
    this.searchSubscription.unsubscribe();
    this.autoRefreshSubscription.unsubscribe();
    this.saveFavoriteServerList();
  }

  public cancel(): void {
    this.setView.next('connect');
  }

  public updateServerList() {

    if (!this.isLoading) {
      this.saveFavoriteServerList();
    }

    this.serverListService.getServerList().subscribe((list) => {
      if (list.length === 0) {
        this.isLoading = false;
        return;
      }
    });

    this.serverListService.getServerList()
      .pipe(
        /**
         * Removed due performance and ratelimit issue
         * 
         * Temp fix
         * https://github.com/tiltedphoques/TiltedEvolution/pull/226
         * 
         * Long term fix
         * https://github.com/tiltedphoques/TiltedEvolution/issues/247
         */
        //switchMap((list) => forkJoin(this.getLocationDataByIp(list))),
        map((list) => this.markFavoritedServers(list)),
      )
      .subscribe((list) => {
        this._serverList = list;
        this.filterServerList();
        this.sortPlayers(false);
        this.sortFavorite(false);
        this.sortCountry(false);
        this.isLoading = false;
      });
  }

  private getLocationDataByIp(servers: Server[]): Array<Observable<Server>> {
    return servers.map((server) => {
      return this.serverListService.getInformationForIp(server.ip).pipe(
        map((data) => ({ ...server, countryCode: data.countryCode.toLowerCase(), continent: data.continent, country: data.country })),
      );
    });
  }

  public saveFavoriteServerList() {
    this.storeService.set('favoriteServerList', JSON.stringify(this.getFavoriteServers()));
  }

  private markFavoritedServers(servers: Server[]): Server[] {
    const favoriteServerList = JSON.parse(this.storeService.get('favoriteServerList', '[]'));

    if (favoriteServerList.length > 0) {
      servers.map((server: Server) => {
        favoriteServerList.forEach(favServer => {
          if (server.ip === favServer.ip && server.port === favServer.port) {
            server.isFavorite = true;
          } else if (server.name === favServer.name) {
            server.isFavorite = true;
          }
        });
      });
    }

    return servers;
  }

  public toggleServerFavorite(server: Server) {
    server.isFavorite = !server.isFavorite;
    this.saveFavoriteServerList();
    this.filterServerList();
  }

  private onAutoRefreshSubcription() {
    let minutes = 5;
    this.autoRefreshSubscription = interval(60000 * minutes)
      .subscribe((value) => {
        this.updateServerList();
      });
  }


  private filterServerList() {
    const search = this.formSearch.value;

    const maybeSearch = search !== '' ? search.toLowerCase() : undefined;

    this.serverList = this._serverList.filter((server: Server) => {
      if (maybeSearch && !server.name.toLowerCase().includes(maybeSearch) && !server.desc.toLowerCase().includes(maybeSearch)) {
        return false;
      }
      return true;
    });

    this.sortPlayers(false);
    this.sortFavorite(false);
    this.sortCountry(false);
  }


  public onSearchSubscription() {
    this.searchSubscription = this.formSearch.valueChanges
      .pipe(
        debounceTime(400),
        distinctUntilChanged(),
      )
      .subscribe((value) => {
        this.filterNameServer(value);
        this.filterCountryServer(value);
      });
  }

  public sortPlayers(isIncreasingOrder: boolean) {
    let sort = ServerListComponent.sortDescendingPlayerCount;
    if (isIncreasingOrder) {
      sort = ServerListComponent.sortIncreasingPlayerCount;
    }

    this.serverList.sort(sort);
    this.isIncreasingPlayerOrder = isIncreasingOrder;
  }

  public sortCountry(isIncreasingCountryOrder: boolean) {
    let sort = ServerListComponent.sortDescendingCountryCount;
    if (isIncreasingCountryOrder) {
      sort = ServerListComponent.sortIncreasingCountryCount;
    }

    this.serverList.sort(sort);
    this.isIncreasingCountryOrder = isIncreasingCountryOrder;
  }

  public sortName(isIncreasingNameOrder: boolean) {
    let sort = ServerListComponent.sortDescendingNameCount;
    if (isIncreasingNameOrder) {
      sort = ServerListComponent.sortIncreasingNameCount;
    }

    this.serverList.sort(sort);
    this.isIncreasingNameOrder = isIncreasingNameOrder;
  }

  public sortFavorite(isIncreasingFavoriteOrder: boolean) {
    let sort = ServerListComponent.sortDescendingFavoriteCount;
    if (isIncreasingFavoriteOrder) {
      sort = ServerListComponent.sortIncreasingFavoriteCount;
    }

    this.serverList.sort(sort);
    this.isIncreasingFavoriteOrder = isIncreasingFavoriteOrder;
  }

  private filterNameServer(search: string): void {
    if (search || search === '') {
      search = search.toLowerCase();

      this.serverList = this._serverList.filter((server: Server) => {
        return server.name.toLowerCase().includes(search);
      });
    }
  }

  private filterCountryServer(search: string): void {
    if (search) {
      search = search.toLowerCase();

      this.serverList = this.serverList.concat(this._serverList.filter((server: Server) => {
        return server.country.toLowerCase().includes(search);
      }));
    }
  }

  public connect(server: Server) {
    this.clientService.connect(server.ip, server.port ? server.port : 10578);
    this.soundService.play(Sound.Ok);
    this.close();
  }

  public getFavoriteServers(): Server[] {
    return this._serverList.filter(value => {
      return value.isFavorite;
    });
  }

  public getFavoriteIcon(server: Server) {
    return server.isFavorite ? fasStar : farStar;
  }

  public getClientVersion() {
    return this.clientService.versionSet.value.split('-')[0];
  }

  public getServerVersion(server: Server) {
    return server.version.split('-')[0];
  }

  public isCompatibleToClient(server: Server) {
    return this.getServerVersion(server) === this.getClientVersion();
  }

  private close() {
    if (this.errorService.error$.value) {
      this.errorService.removeError();
    } else {
      this.done.next();
    }
  }

  static sortDescendingPlayerCount(a: Server, b: Server) {
    return b.player_count - a.player_count;
  }

  static sortIncreasingPlayerCount(a: Server, b: Server) {
    return a.player_count - b.player_count;
  }

  static sortDescendingCountryCount(a: Server, b: Server) {
    if (a.country > b.country) {
      return -1;
    } else if (a.country < b.country) {
      return 1;
    }
    return 0;
  }

  static sortIncreasingCountryCount(a: Server, b: Server) {
    if (a.country > b.country) {
      return 1;
    } else if (a.country < b.country) {
      return -1;
    }
    return 0;
  }

  static sortDescendingNameCount(a: Server, b: Server) {
    if (a.name > b.name) {
      return -1;
    } else if (a.name < b.name) {
      return 1;
    }
    return 0;
  }

  static sortIncreasingNameCount(a: Server, b: Server) {
    if (a.name > b.name) {
      return 1;
    } else if (a.name < b.name) {
      return -1;
    }
    return 0;
  }

  static sortDescendingFavoriteCount(a: Server, b: Server) {
    return (a.isFavorite === b.isFavorite) ? 0 : a.isFavorite ? -1 : 1;
  }

  static sortIncreasingFavoriteCount(a: Server, b: Server) {
    return (b.isFavorite === a.isFavorite) ? 0 : b.isFavorite ? -1 : 1;
  }

  @HostListener('window:keydown.escape', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    this.close();
    event.stopPropagation();
    event.preventDefault();
  }
}
