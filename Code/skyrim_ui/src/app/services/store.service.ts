import { Injectable } from '@angular/core';
import { environment } from '../../environments/environment';

@Injectable({
  providedIn: 'root',
})
export class StoreService {
  public get(key: string, valueIfNull: any): any {
    const value = localStorage.getItem(key);

    if (value !== null) {
      if (!environment.game || !environment.production) {
        console.log(
          `%cSTORAGE`,
          'background: #400088; color: #fff; padding: 3px; font-size: 9px;',
          'Value:',
          key,
          ':',
          value,
        );
      }
      return value;
    }

    return valueIfNull;
  }

  public getBool(key: string, valueIfNull: boolean): boolean {
    const value = this.get(key, null);
    if (value === 'true') {
      return true;
    } else if (value === 'false') {
      return false;
    } else {
      return valueIfNull;
    }
  }

  public getFloat(key: string, valueIfNull: number): number {
    const value = parseFloat(this.get(key, null));
    if (isFinite(value)) {
      return value;
    } else {
      return valueIfNull;
    }
  }

  public set(key: string, value: any): void {
    localStorage.setItem(key, value);
  }

  public remove(key: string): void {
    localStorage.removeItem(key);
  }
}
