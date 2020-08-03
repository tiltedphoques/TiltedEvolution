import {
  Component, ViewEncapsulation, Input, HostBinding
} from '@angular/core';

@Component({
  selector: 'app-window',
  templateUrl: './window.component.html',
  styleUrls: [ './window.component.scss' ],
  encapsulation: ViewEncapsulation.None
})
export class WindowComponent {
  @Input()
  @HostBinding('attr.data-app-window-border')
  public border = '';
}
