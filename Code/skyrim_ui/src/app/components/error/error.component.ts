import { Component, ViewEncapsulation } from '@angular/core';
import { Observable } from 'rxjs';
import { ErrorService } from '../../services/error.service';


@Component({
  selector: 'app-error',
  templateUrl: './error.component.html',
  styleUrls: ['./error.component.scss'],
  encapsulation: ViewEncapsulation.None,
})
export class ErrorComponent {

  error$: Observable<string>;

  constructor(
    private readonly errorService: ErrorService,
  ) {
    this.error$ = this.errorService.error$.asObservable();
  }

  removeError() {
    this.errorService.removeError();
  }

}
