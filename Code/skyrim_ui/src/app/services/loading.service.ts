import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';
import { Sound, SoundService } from './sound.service';


@Injectable({
  providedIn: 'root',
})
export class LoadingService {

  private isLoading = new BehaviorSubject<boolean>(false);

  constructor(private soundService: SoundService) {
  }

  public setLoading(isLoading: boolean): void {
    this.isLoading.next(isLoading);

    if (isLoading) {
      this.soundService.play(Sound.Ok);
    }
  }

  public getLoading(): Observable<boolean> {
    return this.isLoading.asObservable();
  }

}
