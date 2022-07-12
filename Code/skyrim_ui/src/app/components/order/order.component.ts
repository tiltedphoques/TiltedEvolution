import { Component, EventEmitter, Input, Output, ViewEncapsulation } from '@angular/core';
import { faCaretDown, faCaretUp, faSort } from '@fortawesome/free-solid-svg-icons';
import { BehaviorSubject } from 'rxjs';


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
  readonly faSort = faSort;

  order = new BehaviorSubject<boolean | null>(null);

  @Input('isIncreasingOrder')
  public set isIncreasingOrderInput(value: boolean | null) {
    this.order.next(value);
  }

  @Output() public sorted = new EventEmitter<boolean>();

  public sort() {
    let newValue: boolean | null;
    switch (this.order.getValue()) {
      case true:
        newValue = false;
        break;
      case false:
        newValue = null;
        break;
      default:
        newValue = true;
        break;
    }
    this.sorted.emit(newValue);
  }

}
