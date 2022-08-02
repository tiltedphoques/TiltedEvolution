import { Directive, HostListener } from '@angular/core';
import { Sound, SoundService } from '../services/sound.service';

@Directive({
  selector: 'input[type="checkbox"]',
  providers: [
    SoundService
  ]
})
// @ts-ignore
export class CheckboxDirective {

  constructor(private readonly soundService: SoundService) {}

  @HostListener('change', ['$event'])
  // @ts-ignore
  onChange(event: ChangeEvent<HTMLInputElement>) {
    const target: HTMLInputElement = event.target
    this.soundService.play(target.checked ? Sound.Check : Sound.Uncheck);
  }
}