import { Component, ChangeDetectionStrategy } from '@angular/core';
import { animation } from './popup.animation';

@Component({
  selector: 'app-popup',
  templateUrl: './popup.component.html',
  styleUrls: ['./popup.component.scss'],
  animations: [animation],
  host: { '[@popup]': 'true' },
  changeDetection: ChangeDetectionStrategy.Eager,
  standalone: false,
})
export class PopupComponent {}
