import { Component, HostBinding, Input } from '@angular/core';


@Component({
  selector: 'app-window',
  templateUrl: './window.component.html',
  styleUrls: ['./window.component.scss'],
})
export class WindowComponent {

  @Input()
  @HostBinding('attr.data-app-window-border')
  public border = '';
}
