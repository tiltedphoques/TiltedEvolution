import { Component, EventEmitter, Input, Output, ViewEncapsulation } from '@angular/core';
import { faCaretDown, faCaretUp } from '@fortawesome/free-solid-svg-icons';


@Component({
  selector: 'app-order',
  templateUrl: './order.component.html',
  styleUrls: ['./order.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class OrderComponent {

  /* ### ICONS ### */
  readonly faCaretDown = faCaretDown;
  readonly faCaretUp = faCaretUp;

  @Input() public isIncreasingOrder: boolean;
  @Output() public sorted = new EventEmitter<boolean>();

  public sort() {
    this.sorted.emit(!this.isIncreasingOrder);
  }

}
