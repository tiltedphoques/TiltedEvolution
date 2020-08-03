import { Component, OnInit, ViewEncapsulation, OnDestroy } from '@angular/core';
import { LoadingService } from '../../services/loading.service';
import { Subscription } from 'rxjs';

@Component({
  selector: 'app-loading',
  templateUrl: './loading.component.html',
  styleUrls: ['./loading.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class LoadingComponent implements OnInit, OnDestroy {
  private isLoading$: Subscription;
  public isLoading = false;

  constructor(private loadingService: LoadingService) { }

  ngOnInit() {
    this.isLoading$ = this.loadingService.getLoading().subscribe((isLoading: boolean) => {
      this.isLoading = isLoading;
    })
  }

  ngOnDestroy() {
    this.isLoading$.unsubscribe;
  }
}
