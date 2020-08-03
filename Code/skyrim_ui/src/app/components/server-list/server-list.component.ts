import { Component, OnInit, ViewEncapsulation, Output, EventEmitter, HostListener, OnDestroy } from '@angular/core';
import { ErrorService } from '../../services/error.service';
import { ServerListService } from '../../services/server-list.service';
import { ServerList } from '../../models/server-list';
import { ClientService } from '../../services/client.service';
import { SoundService, Sound } from '../../services/sound.service';
import { FormControl } from '@angular/forms';
import { Subscription } from 'rxjs';
import { debounceTime, distinctUntilChanged } from 'rxjs/operators';
import { StoreService } from '../../services/store.service';
import { faStar as farStar} from "@fortawesome/free-regular-svg-icons";
import { faStar as fasStar} from "@fortawesome/free-solid-svg-icons";

@Component({
  selector: 'app-server-list',
  templateUrl: './server-list.component.html',
  styleUrls: ['./server-list.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class ServerListComponent implements OnInit, OnDestroy {

  @Output()
  public done = new EventEmitter();

  // Server list with search / filter
  public serverList: ServerList[] = [];
  public searchServerList = new FormControl();
  public isIncreasingPlayerOrder = true;
  public isIncreasingCountryOrder = true;
  public isIncreasingNameOrder = true;
  public isIncreasingFavoriteOrder = true;
  public farStar = farStar;
  public fasStar = fasStar;

  public isLoading = true;

  // All serverList data
  private _serverList: ServerList[] = [];
  private searchSubscription: Subscription;

  constructor(private errorService: ErrorService, private serverListService: ServerListService,
    private clientService: ClientService,
    private soundService: SoundService,
    private storeService: StoreService) { }

  ngOnInit() {
    this.updateServerList();
    this.onSearchSubscription()
  }

  ngOnDestroy() {
    this.searchSubscription.unsubscribe();
    this.storeService.set('favoriteServerList', JSON.stringify(this.getFavoriteServers()));
  }

  public updateServerList() {
    const favoriteServerList = JSON.parse(this.storeService.get('favoriteServerList', "[]"));

    this.serverListService.getServerList().subscribe((serverList: any[]) => {
      if (serverList && serverList.length > 0) {
        this._serverList = serverList.map((value: any) => {
          value.isOnline = true;
          return new ServerList(value);
        });

        // Favorite server
        favoriteServerList.map((s: ServerList) => {
          s.isFavorite = true;
          s.isOnline = false;
          this._serverList = this._serverList.filter((value: ServerList) => {
            if (value.name === s.name) {
              s.isOnline = true;
              s.playerCount = value.playerCount;
              return false;
            }
            else {
              return true;
            }
          });
        });

        this._serverList = this._serverList.concat(favoriteServerList);
        this.serverList = this._serverList;

        this.filterNameServer(this.searchServerList.value);

        this.sortPlayers(false);
        this.sortFavorite(false);

        // Get country info
        this.serverList.map(value => {
          this.serverListService.getCountryForIp(value.ip).subscribe((country: any) => {
            if (country && country.country) {
              value.country = country.country;
            }
          })
        });
        this.filterCountryServer(this.searchServerList.value);
      }
      this.isLoading = false;
    });
  }

  public onSearchSubscription() {
    this.searchSubscription = this.searchServerList.valueChanges
      .pipe(
        debounceTime(400),
        distinctUntilChanged()
      )
      .subscribe((value) => {
        this.filterNameServer(value);
        this.filterCountryServer(value);
      })
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
    if (search || search === "") {
      search = search.toLowerCase();

      this.serverList = this._serverList.filter((server: ServerList) => {
        return server.name.toLowerCase().includes(search);
      });
    }
  }

  private filterCountryServer(search: string): void {
    if (search) {
      search = search.toLowerCase();

      this.serverList = this.serverList.concat(this._serverList.filter((server: ServerList) => {
        return server.country.toLowerCase().includes(search);
      }));
    }
  }

  public connect(server: ServerList) {
    this.clientService.connect(server.ip, server.port ? Number.parseInt(server.port) : 10578);
    this.soundService.play(Sound.Ok);
    this.close();
  }

  public getFavoriteServers(): ServerList[] {
    return this._serverList.filter(value => {
      return value.isFavorite;
    })
  }

  public getFavoriteIcon(server: ServerList) {
    return server.isFavorite ? fasStar : farStar;
  }

  private close() {
    if (this.errorService.error$.value) {
      this.errorService.removeError();
    }
    else {
      this.done.next();
    }
  }

  static sortDescendingPlayerCount(a: ServerList, b: ServerList) {
    return b.playerCount - a.playerCount;
  }

  static sortIncreasingPlayerCount(a: ServerList, b: ServerList) {
    return a.playerCount - b.playerCount;
  }

  static sortDescendingCountryCount(a: ServerList, b: ServerList) {
    if (a.country > b.country) {
      return -1;
    }
    else if (a.country < b.country) {
      return 1;
    }
    return 0;
  }

  static sortIncreasingCountryCount(a: ServerList, b: ServerList) {
    if (a.country > b.country) {
      return 1;
    }
    else if (a.country < b.country) {
      return -1;
    }
    return 0;
  }

  static sortDescendingNameCount(a: ServerList, b: ServerList) {
    if (a.name > b.name) {
      return -1;
    }
    else if (a.name < b.name) {
      return 1;
    }
    return 0;
  }

  static sortIncreasingNameCount(a: ServerList, b: ServerList) {
    if (a.name > b.name) {
      return 1;
    }
    else if (a.name < b.name) {
      return -1;
    }
    return 0;
  }

  static sortDescendingFavoriteCount(a: ServerList, b: ServerList) {
    return (a.isFavorite === b.isFavorite)? 0 : a.isFavorite? -1 : 1;
  }

  static sortIncreasingFavoriteCount(a: ServerList, b: ServerList) {
    return (b.isFavorite === a.isFavorite)? 0 : b.isFavorite? -1 : 1;
  }

  @HostListener('window:keydown.escape', [ '$event' ])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    this.close();
    event.stopPropagation();
    event.preventDefault();
  }
}
