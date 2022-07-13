import { Component } from '@angular/core';
import { animation } from './popup.animation';


@Component({
  selector: 'app-popup',
  templateUrl: './popup.component.html',
  styleUrls: ['./popup.component.scss'],
  animations: [animation],
  host: { '[@popup]': 'true' },
})
export class PopupComponent {
}
