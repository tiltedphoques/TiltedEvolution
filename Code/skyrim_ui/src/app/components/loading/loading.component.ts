import { Component, ChangeDetectionStrategy } from '@angular/core';
import { Observable } from 'rxjs';
import { LoadingService } from '../../services/loading.service';

@Component({
  selector: 'app-loading',
  templateUrl: './loading.component.html',
  styleUrls: ['./loading.component.scss'],
  changeDetection: ChangeDetectionStrategy.Eager,
  standalone: false,
})
export class LoadingComponent {
  isLoading$: Observable<boolean>;

  constructor(private readonly loadingService: LoadingService) {
    this.isLoading$ = this.loadingService.getLoading();
  }
}
