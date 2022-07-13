import { Component, SecurityContext } from '@angular/core';
import { DomSanitizer, SafeHtml } from '@angular/platform-browser';
import { Observable } from 'rxjs';
import { map } from 'rxjs/operators';
import { ErrorService } from '../../services/error.service';


@Component({
  selector: 'app-error',
  templateUrl: './error.component.html',
  styleUrls: ['./error.component.scss'],
})
export class ErrorComponent {

  error$: Observable<SafeHtml>;

  constructor(
    private readonly errorService: ErrorService,
    private readonly domSanitizer: DomSanitizer,
  ) {
    this.error$ = this.errorService.error$.pipe(
      map(error => this.domSanitizer.sanitize(SecurityContext.HTML, error)),
      map(error => error.replace(/&#10;/g, '<br>')),
      map(error => error !== '' ? this.domSanitizer.bypassSecurityTrustHtml(error) : null),
    );
  }

  removeError() {
    this.errorService.removeError();
  }

}
