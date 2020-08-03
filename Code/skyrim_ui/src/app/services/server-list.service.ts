import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { environment } from '../../environments/environment';
import { Observable } from 'rxjs';
import { ServerList } from '../models/server-list';
import { map } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})
export class ServerListService {

  constructor(private http: HttpClient) { }

  public getServerList(): Observable<ServerList[]> {
    return this.http.get<ServerList[]>(`${environment.urlProtocol}://list.${environment.baseUrl}/list`)
      .pipe(
        map((data: any) => data.servers),
      );
  }

  public getCountryForIp(ip: string) {
    return this.http.get(`http://ip-api.com/json/${ip}?fields=country`);
  }
}
