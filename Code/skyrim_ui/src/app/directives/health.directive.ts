import { Directive, ElementRef, Input, OnChanges } from '@angular/core';


@Directive({
  selector: '[app-health]',
})
export class HealthDirective implements OnChanges {

  @Input('app-health') health: string | number;

  constructor(
    private readonly el: ElementRef,
  ) {
  }

  ngOnChanges() {
    this.el.nativeElement.style.width = `${ this.health }%`;
  }

}
