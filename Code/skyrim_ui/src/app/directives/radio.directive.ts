import { Directive, HostListener } from '@angular/core';
import { Sound, SoundService } from '../services/sound.service';

@Directive({
  selector: 'input[type="radio"]',
  providers: [
    SoundService
  ]
})
// @ts-ignore
export class RadioDirective {

  constructor(private readonly soundService: SoundService) {}

  @HostListener('change')
  // @ts-ignore
  onChange() {
    this.soundService.play(Sound.Check);
  }
}