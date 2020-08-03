import { Component, ViewEncapsulation } from '@angular/core';
import { ErrorService } from '../../services/error.service';

@Component({
  selector: 'app-error',
  templateUrl: './error.component.html',
  styleUrls: ['./error.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class ErrorComponent {

  constructor(private errorService: ErrorService)
  {
  }

  public get error(): string {
    return this.errorService.error$.value;
  }

  removeError() {
    this.errorService.removeError();
  }

}
