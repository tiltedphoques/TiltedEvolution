import { Directive, ElementRef, HostListener } from '@angular/core';
import { Sound, SoundService } from '../services/sound.service';

@Directive({
  selector: 'input[type="range"]',
  providers: [
    SoundService
  ]
})
// @ts-ignore
export class SliderDirective {

  constructor(private readonly soundService: SoundService) {}

  @HostListener('change') onChange() {
    this.soundService.play(Sound.Check);
  }
}