import { ChangeDetectionStrategy, Component, EventEmitter, Input, Output } from '@angular/core';


@Component({
  selector: 'app-toggle',
  templateUrl: './toggle.component.html',
  styleUrls: ['./toggle.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class ToggleComponent {

  @Input('isShow') public isShow = false;
  @Input('title') public title: String;

  @Output('toggleChange') public toggleChange = new EventEmitter();

  public clickToggle() {
    this.isShow = !this.isShow;
    this.toggleChange.emit(this.isShow);
  }

}
