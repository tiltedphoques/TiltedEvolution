import { ChangeDetectionStrategy, Component, EventEmitter, HostListener, Output } from '@angular/core';
import { faStar as farStar } from '@fortawesome/free-regular-svg-icons';
import { faStar as fasStar } from '@fortawesome/free-solid-svg-icons';
import { loadingFor } from '@ngneat/loadoff';
import { FormControl } from '@ngneat/reactive-forms';
import { BehaviorSubject, combineLatestWith, Observable, ReplaySubject, share, startWith, throttleTime } from 'rxjs';
import { distinctUntilChanged, map, switchMap } from 'rxjs/operators';
import { Server } from '../../models/server';
import { View } from '../../models/view.enum';
import { ClientService } from '../../services/client.service';
import { ErrorService } from '../../services/error.service';
import { ServerListService } from '../../services/server-list.service';
import { Sound, SoundService } from '../../services/sound.service';
import { StoreService } from '../../services/store.service';
import { UiRepository } from '../../store/ui.repository';
import { SortOrder } from '../order/order.component';


interface SortFunction {
  fn: ((a: Server, b: Server) => number);
  priority: number;
}

@Component({
  selector: 'app-server-list',
  templateUrl: './server-list.component.html',
  styleUrls: ['./server-list.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class ServerListComponent {

  /* ### ICONS ### */
  readonly fasStar = fasStar;
  readonly farStar = farStar;

  loader = loadingFor('serverlist');
  refreshServerlist = new BehaviorSubject<void>(undefined);
  sortFunctions = new BehaviorSubject<SortFunction[]>([]);
  favoriteServers = new BehaviorSubject<Record<string, Server>>({});
  hideVersionMismatchedServers = new BehaviorSubject(true);
  hideFullServers = new BehaviorSubject(true);
  playerCountOrdering = new BehaviorSubject(SortOrder.NONE);
  countryOrdering = new BehaviorSubject(SortOrder.NONE);
  serverNameOrdering = new BehaviorSubject(SortOrder.NONE);
  favoriteOrdering = new BehaviorSubject(SortOrder.NONE);
  serverlist$: Observable<(Server & { isCompatible: boolean; shortVersion: string; isFull: boolean })[]>;
  filteredServerlist$: typeof this.serverlist$;
  clientVersion$: Observable<string>;

  formSearch = new FormControl<string>('');
  rowHeight = 16 * document.documentElement.clientWidth / 1920 * 2;

  @Output() public done = new EventEmitter<void>();

  constructor(
    private readonly errorService: ErrorService,
    private readonly serverListService: ServerListService,
    private readonly clientService: ClientService,
    private readonly soundService: SoundService,
    private readonly storeService: StoreService,
    private readonly uiRepository: UiRepository,
  ) {
    this.sortFavorite(SortOrder.DESC);
    this.sortPlayerCount(SortOrder.DESC);
    this.serverlist$ = this.refreshServerlist
      .pipe(
        switchMap(() => this.serverListService
          .getServerList()
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
            this.loader.serverlist.track(),
          ),
        ),
        combineLatestWith(this.favoriteServers, this.clientService.versionSet),
        map(([servers, favorites, clientVersion]) => {
          return servers.map(server => {
            const shortVersion = this.getServerVersion(server);
            return {
              ...server,
              isFavorite: !!favorites[`${ server.ip }:${ server.port }`],
              isFull: server.player_count >= server.max_player_count,
              shortVersion,
              isCompatible: shortVersion === clientVersion,
            };
          });
        }),
        share({ connector: () => new ReplaySubject(1), resetOnRefCountZero: true }),
      );

    this.filteredServerlist$ = this.serverlist$
      .pipe(
        combineLatestWith(
          this.formSearch.value$.pipe(
            map(searchPhrase => searchPhrase?.toLowerCase()),
            distinctUntilChanged(),
            throttleTime(300),
          ),
          this.hideVersionMismatchedServers,
          this.hideFullServers,
          this.sortFunctions,
        ),
        map(([servers, searchPhrase, hideVersionMismatchedServers, hideFullServers, sortFunction]) => {
          if (hideVersionMismatchedServers) {
            servers = servers.filter(server => server.isCompatible);
          }
          if (hideFullServers) {
            servers = servers.filter(server => !server.isFull);
          }
          if (searchPhrase) {
            servers = servers.filter((server: Server) => {
              return server.name.toLowerCase().includes(searchPhrase) || server.desc.toLowerCase().includes(searchPhrase);
            });
          }
          if (sortFunction.length > 0) {
            servers = [...servers].sort(this.sortElementsFn());
          }
          return servers;
        }),
        startWith([]),
        share({ connector: () => new ReplaySubject(1), resetOnRefCountZero: true }),
      );

    this.clientVersion$ = this.clientService.versionSet.pipe(map(version => version.split('-')[0]));

    // load favorite servers
    const favoriteServerList = JSON.parse(this.storeService.get('favoriteServerList', '[]'));
    const favoriteServers: Record<string, Server> = {};
    for (const favoriteServer of favoriteServerList) {
      favoriteServers[`${ favoriteServer.ip }:${ favoriteServer.port }`] = favoriteServer;
    }
    this.favoriteServers.next(favoriteServers);
  }

  public cancel(): void {
    this.uiRepository.openView(View.CONNECT);
  }

  async updateServerList() {
    this.refreshServerlist.next();
  }

  private getLocationDataByIp(servers: Server[]): Array<Observable<Server>> {
    return servers.map((server) => {
      return this.serverListService.getInformationForIp(server.ip).pipe(
        map((data) => ({ ...server, countryCode: data.countryCode.toLowerCase(), continent: data.continent, country: data.country })),
      );
    });
  }

  async saveFavoriteServerList() {
    const favorites = Object.values(this.favoriteServers.getValue());
    this.storeService.set('favoriteServerList', JSON.stringify(favorites));
  }

  async toggleServerFavorite(server: Server) {
    let favorites = this.favoriteServers.getValue();
    favorites = { ...favorites };
    if (favorites[`${ server.ip }:${ server.port }`]) {
      delete favorites[`${ server.ip }:${ server.port }`];
    } else {
      favorites[`${ server.ip }:${ server.port }`] = { ...server };
    }
    this.favoriteServers.next(favorites);

    server.isFavorite = !server.isFavorite;
    await this.saveFavoriteServerList();
  }

  public sortPlayerCount(sortOrder: SortOrder) {
    this.setSortingFn(3, sortOrder, ServerListComponent.sortPlayerCountAsc, ServerListComponent.sortPlayerCountDesc);
    this.playerCountOrdering.next(sortOrder);
  }

  public sortCountry(sortOrder: SortOrder) {
    this.setSortingFn(2, sortOrder, ServerListComponent.sortCountryAsc, ServerListComponent.sortCountryDesc);
    this.countryOrdering.next(sortOrder);
  }

  public sortServerName(sortOrder: SortOrder) {
    this.setSortingFn(1, sortOrder, ServerListComponent.sortNameAsc, ServerListComponent.sortNameDesc);
    this.serverNameOrdering.next(sortOrder);
  }

  public sortFavorite(sortOrder: SortOrder) {
    this.setSortingFn(0, sortOrder, ServerListComponent.sortFavoriteAsc, ServerListComponent.sortFavoriteDesc);
    this.favoriteOrdering.next(sortOrder);
  }

  // private filterCountryServer(search: string): void {
  //   if (search) {
  //     search = search.toLowerCase();
  //
  //     this.serverList = this.serverList.concat(this._serverList.filter((server: Server) => {
  //       return server.country.toLowerCase().includes(search);
  //     }));
  //   }
  // }

  public connect(server: Server) {
    this.clientService.connect(server.ip, server.port ? server.port : 10578);
    this.soundService.play(Sound.Ok);
    this.close();
  }

  public getServerVersion(server: Server) {
    return server.version.split('-')[0];
  }

  private close() {
    if (this.errorService.getError()) {
      this.errorService.removeError();
    } else {
      this.done.next();
    }
  }

  private setSortingFn(priority: number, ordering: SortOrder, asc: (a: Server, b: Server) => number, desc: (a: Server, b: Server) => number) {
    const sortFns = [...this.sortFunctions.getValue()];
    let index = sortFns.findIndex(sortFn => sortFn.fn === asc);
    if (index !== -1) {
      sortFns.splice(index, 1);
    }
    index = sortFns.findIndex(sortFn => sortFn.fn === desc);
    if (index !== -1) {
      sortFns.splice(index, 1);
    }
    if (ordering === SortOrder.ASC) {
      sortFns.push({ fn: asc, priority });
    } else if (ordering === SortOrder.DESC) {
      sortFns.push({ fn: desc, priority });
    }
    this.sortFunctions.next(sortFns);
  }

  sortElementsFn() {
    const fns = [...this.sortFunctions.getValue()].sort((a, b) => a.priority - b.priority);
    return (a: Server, b: Server) => {
      let result = 0;
      for (const fn of fns) {
        result ||= fn.fn(a, b);
      }
      return result;
    };
  }

  static sortPlayerCountDesc(a: Server, b: Server) {
    return b.player_count - a.player_count;
  }

  static sortPlayerCountAsc(a: Server, b: Server) {
    return a.player_count - b.player_count;
  }

  static sortCountryDesc(a: Server, b: Server) {
    return ServerListComponent.sortCountryAsc(a, b) * -1;
  }

  static sortCountryAsc(a: Server, b: Server) {
    if (a.country > b.country) {
      return 1;
    } else if (a.country < b.country) {
      return -1;
    }
    return 0;
  }

  static sortNameDesc(a: Server, b: Server) {
    return ServerListComponent.sortNameAsc(a, b) * -1;
  }

  static sortNameAsc(a: Server, b: Server) {
    return a.name.localeCompare(b.name);
  }

  static sortFavoriteDesc(a: Server, b: Server) {
    return ServerListComponent.sortFavoriteAsc(a, b) * -1;
  }

  static sortFavoriteAsc(a: Server, b: Server) {
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
