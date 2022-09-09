import { Directive, Input } from '@angular/core';


@Directive({
  selector: 'app-dropdown-option',
})
export class DropdownOptionComponent {

  @Input() text: string;
  @Input() value: any;

}
