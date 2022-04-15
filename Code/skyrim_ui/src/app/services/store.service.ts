import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class StoreService {

  constructor() {}

  public get(key: string, valueIfNull: any): any {
    const value = localStorage.getItem(key);

    if (value !== null) {
      //console.log(`Value : ${key} : ${value}`);
      //console.log(value);
      return value;
    }

    return valueIfNull;
  }

  public set(key: string, value: any): void {
    localStorage.setItem(key, value);
  }
}
