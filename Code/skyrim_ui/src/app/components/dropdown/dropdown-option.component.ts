import { Directive, Input } from '@angular/core';

@Directive({
  selector: 'app-dropdown-option',
  standalone: false,
})
export class DropdownOptionComponent {
  @Input() text: string;
  @Input() value: any;
}
