import { Injectable } from '@angular/core';
import { BehaviorSubject } from 'rxjs';
import { SoundService, Sound } from './sound.service';

@Injectable({
  providedIn: 'root'
})
export class ErrorService {

  error$ = new BehaviorSubject<string>("");

  constructor(private sound: SoundService) { }

  error(message: string) {
    this.sound.play(Sound.Fail);
    this.error$.next(message);
  }

  removeError() {
    this.sound.play(Sound.Ok);
    this.error$.next("");
  }
}
