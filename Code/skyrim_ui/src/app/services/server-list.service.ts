import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable, of } from 'rxjs';
import { map, tap } from 'rxjs/operators';
import { environment } from '../../environments/environment';
import { GeoLocation } from '../models/geo-location';
import { Server } from '../models/server';

const MAX_SERVERNAME_LENGTH = 100;

function truncateServerName(server: Server): Server {
  const name = server.name.substring(0, MAX_SERVERNAME_LENGTH);
  return {...server, name};
};

@Injectable({
  providedIn: 'root',
})
export class ServerListService {

  constructor(
    private readonly http: HttpClient,
  ) {
  }

  public getServerList(): Observable<Server[]> {
    return this.http.get<Server[]>(`${ environment.urlProtocol }://${ environment.url }/list`)
      .pipe(
        map<any, Server[]>((data: any) => data.servers),
        map(servers => servers.map(truncateServerName))
      );
  }

  public getInformationForIp(ip: string): Observable<GeoLocation> {
    const cached = localStorage.getItem(`ip-cache:${ ip }`);

    if (cached) {
      return of(JSON.parse(cached));
    }

    return this.http.get<GeoLocation>(`http://ip-api.com/json/${ ip }?fields=continent,countryCode,country`).pipe(
      tap((response) => localStorage.setItem(`ip-cache:${ ip }`, JSON.stringify(response))),
    );
  }


}
