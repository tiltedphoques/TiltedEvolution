import { Component, EventEmitter, HostListener, OnDestroy, OnInit, Output, ViewEncapsulation } from '@angular/core';


@Component({
  selector: 'app-player-manager',
  templateUrl: './player-manager.component.html',
  styleUrls: ['./player-manager.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class PlayerManagerComponent implements OnInit, OnDestroy {

  @Output()
  public done = new EventEmitter<void>();

  constructor() {
  }

  ngOnInit(): void {
  }

  ngOnDestroy(): void {
  }

  public cancel(): void {
    this.done.next();
  }

  private close() {
    this.done.next();
  }

  @HostListener('window:keydown.escape', ['$event'])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    this.close();
    event.stopPropagation();
    event.preventDefault();
  }
}
