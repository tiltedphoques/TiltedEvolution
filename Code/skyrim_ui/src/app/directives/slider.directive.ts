import { Directive, HostListener } from '@angular/core';
import { Sound, SoundService } from '../services/sound.service';

@Directive({
  selector: 'input[type="range"]',
})
// @ts-ignore
export class SliderDirective {

  constructor(private readonly soundService: SoundService) {}

  @HostListener('change') onChange() {
    this.soundService.play(Sound.Check);
  }
}