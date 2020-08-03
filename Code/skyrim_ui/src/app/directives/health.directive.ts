import { Directive, Input, ElementRef, OnChanges } from '@angular/core';

@Directive({
  selector: '[app-health]'
})
export class HealthDirective implements OnChanges {

  @Input('app-health')
  health: string;

  constructor(private el: ElementRef) { }

  ngOnChanges() {
    this.el.nativeElement.style.width = `${this.health}%`;
  }
}
