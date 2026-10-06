import { ChangeDetectionStrategy, Component } from '@angular/core';
import { Observable } from 'rxjs';
import {
  TradeInventoryItemView,
  TradeOfferItemView,
  TradeSessionView,
  TradeUiService,
} from '../../services/trade-ui.service';

@Component({
  selector: 'app-trade-popup',
  templateUrl: './trade-popup.component.html',
  styleUrls: ['./trade-popup.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class TradePopupComponent {
  public readonly categories = this.tradeUiService.categories;
  public readonly category$ = this.tradeUiService.category$;
  public readonly visibleInventory$ = this.tradeUiService.visibleInventory$;
  public readonly session$: Observable<TradeSessionView | undefined> =
    this.tradeUiService.session$;

  constructor(public readonly tradeUiService: TradeUiService) {}

  public cancelTrade(): void {
    this.tradeUiService.cancelTrade();
  }

  public toggleReady(session: TradeSessionView): void {
    this.tradeUiService.setReady(!session.selfReady);
  }

  public updateOfferFromInput(
    item: TradeInventoryItemView,
    value: string | number,
  ): void {
    this.tradeUiService.updateOfferFromInput(item.index, value);
  }

  public addToOffer(item: TradeInventoryItemView, delta: number): void {
    this.tradeUiService.addToOffer(item.index, delta);
  }

  public offerAll(item: TradeInventoryItemView): void {
    this.tradeUiService.offerAll(item.index);
  }

  public clearOffer(item: TradeInventoryItemView): void {
    this.tradeUiService.clearOffer(item.index);
  }

  public trackByInventory(_: number, item: TradeInventoryItemView): number {
    return item.index;
  }

  public trackByOffer(_: number, item: TradeOfferItemView): string {
    return item.key ?? `${item.modId}:${item.baseId}:${item.index ?? -1}`;
  }

  public formatCountdown(ms: number): string {
    if (ms <= 0) return '';
    return (ms / 1000).toFixed(1);
  }
}
