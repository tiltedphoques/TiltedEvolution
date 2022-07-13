import { ChangeDetectionStrategy, Component, EventEmitter, Input, Output } from '@angular/core';
import { faCaretDown, faCaretUp, faSort } from '@fortawesome/free-solid-svg-icons';
import { BehaviorSubject } from 'rxjs';


export enum SortOrder {
  NONE,
  ASC,
  DESC
}

@Component({
  selector: 'app-order',
  templateUrl: './order.component.html',
  styleUrls: ['./order.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class OrderComponent {

  /* ### ENUMS ### */
  readonly SortOrder = SortOrder;

  /* ### ICONS ### */
  readonly faCaretDown = faCaretDown;
  readonly faCaretUp = faCaretUp;
  readonly faSort = faSort;

  order = new BehaviorSubject<SortOrder>(SortOrder.NONE);

  @Input()
  public set sortingOrder(value: SortOrder) {
    this.order.next(value);
  }

  @Output() public sorted = new EventEmitter<SortOrder>();

  public sort() {
    let newValue: SortOrder;
    switch (this.order.getValue()) {
      case SortOrder.ASC:
        newValue = SortOrder.DESC;
        break;
      case SortOrder.DESC:
        newValue = SortOrder.NONE;
        break;
      default:
        newValue = SortOrder.ASC;
        break;
    }
    this.sorted.emit(newValue);
  }

}
