import { Directive, HostListener } from '@angular/core';
import { Sound, SoundService } from '../services/sound.service';

@Directive({
  selector: 'input[type="checkbox"]',
})
// @ts-ignore
export class CheckboxDirective {

  constructor(private readonly soundService: SoundService) {}

  @HostListener('change', ['$event'])
  onChange(event: Event) {
    const target = event.target as HTMLInputElement;
    this.soundService.play(target.checked ? Sound.Check : Sound.Uncheck);
  }
}