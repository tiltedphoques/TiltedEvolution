import { Component, EventEmitter, OnDestroy, OnInit, Output, ViewEncapsulation } from "@angular/core";

@Component({
  selector: 'app-player-manager',
  templateUrl: './player-manager.component.html',
  styleUrls: ['./player-manager.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class PlayerManagerComponent implements OnInit, OnDestroy {

  @Output()
  public done = new EventEmitter();

  constructor() { }

  ngOnInit(): void {
  }

  ngOnDestroy(): void {
  }

  public cancel(): void {
    this.done.next();
  }
}