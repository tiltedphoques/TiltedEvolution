import { Component, OnInit, ViewEncapsulation, Input, Output, EventEmitter } from '@angular/core';
import { faCaretUp, faCaretDown} from "@fortawesome/free-solid-svg-icons";

@Component({
  selector: 'app-order',
  templateUrl: './order.component.html',
  styleUrls: ['./order.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class OrderComponent implements OnInit {

  @Input()
  public isIncreasingOrder: boolean;

  @Output()
  public sorted = new EventEmitter<boolean>();
  public faCaretDown = faCaretDown;
  public faCaretUp = faCaretUp;

  constructor() { }

  ngOnInit() {
  }

  public sort() {
    this.sorted.emit(!this.isIncreasingOrder);
  }

}
