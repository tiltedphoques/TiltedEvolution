import { Injectable } from '@angular/core';
import { environment } from '../../environments/environment';


@Injectable({
  providedIn: 'root',
})
export class StoreService {

  public get(key: string, valueIfNull: any): any {
    const value = localStorage.getItem(key);

    if (value !== null) {
      if (!environment.game || environment.nightlyBuild) {
        console.log(`Value : ${ key } : ${ value }`);
      }
      return value;
    }

    return valueIfNull;
  }

  public set(key: string, value: any): void {
    localStorage.setItem(key, value);
  }

  public remove(key: string): void {
    localStorage.removeItem(key);
  }

}
