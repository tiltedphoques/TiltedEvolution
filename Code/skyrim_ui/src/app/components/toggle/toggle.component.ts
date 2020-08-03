import { Component, OnInit, ViewEncapsulation, ChangeDetectionStrategy, Input, EventEmitter, Output } from '@angular/core';

@Component({
  selector: 'app-toggle',
  templateUrl: './toggle.component.html',
  styleUrls: ['./toggle.component.scss'],
  encapsulation: ViewEncapsulation.None,
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class ToggleComponent implements OnInit {
  @Input('isShow')
  public isShow = false;
  @Input('title')
  public title: String;

  @Output('toggleChange')
  public toggleChange = new EventEmitter();

  constructor() { }

  ngOnInit() {
  }

  public clickToggle() {
    this.isShow = !this.isShow;
    this.toggleChange.emit(this.isShow);
  }

}
