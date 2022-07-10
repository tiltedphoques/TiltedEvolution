import { Component, ViewEncapsulation } from '@angular/core';
import { animation } from './popup.animation';


@Component({
  selector: 'app-popup',
  templateUrl: './popup.component.html',
  styleUrls: ['./popup.component.scss'],
  encapsulation: ViewEncapsulation.None,
  animations: [animation],
  host: { '[@popup]': 'true' },
})
export class PopupComponent {
}
