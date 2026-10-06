import { Injectable, OnDestroy } from '@angular/core';
import { BehaviorSubject, combineLatest, map, Subscription } from 'rxjs';
import { TranslocoService } from '@ngneat/transloco';
import { ClientService, TradeItemPayload, TradeStatePayload } from './client.service';
import { PlayerListService } from './player-list.service';
import { PopupNotificationService } from './popup-notification.service';
import { UiRepository } from '../store/ui.repository';
import { View } from '../models/view.enum';
import { matchesTradeCategory, normalizeTradeCategory, tradeCategories, TradeCategory, TradeItemCategory } from '../models/trade-category';

export interface TradeInventoryItemView {
  category: TradeItemCategory;
  index: number;
  name: string;
  modId: number;
  baseId: number;
  available: number;
  offered: number;
  isQuestItem: boolean;
  isEquipped: boolean;
  isGold: boolean;
  details: string[];
  key: string;
}

export interface TradeOfferItemView {
  index?: number;
  name: string;
  modId: number;
  baseId: number;
  count: number;
  isQuestItem: boolean;
  details: string[];
  key: string;
}

export interface TradeSessionView {
  active: boolean;
  partnerId: number;
  partnerName: string;
  initiatedBySelf: boolean;
  selfReady: boolean;
  partnerReady: boolean;
  selfOffer: TradeOfferItemView[];
  partnerOffer: TradeOfferItemView[];
  inventory: TradeInventoryItemView[];
  countdownMs: number;
  countdownTotalMs: number;
  countdownProgress: number;
}

@Injectable({ providedIn: 'root' })
export class TradeUiService implements OnDestroy {
  private readonly subscriptions = new Subscription();
  private readonly sessionSubject = new BehaviorSubject<TradeSessionView | undefined>(undefined);
  public readonly session$ = this.sessionSubject.asObservable();
  public readonly categories = tradeCategories;
  private readonly categorySubject = new BehaviorSubject<TradeCategory>('all');
  public readonly category$ = this.categorySubject.asObservable();
  private readonly searchSubject = new BehaviorSubject<string>('');
  public readonly search$ = this.searchSubject.asObservable();
  public readonly visibleInventory$ = combineLatest([this.session$, this.category$, this.search$]).pipe(
    map(([session, category, search]) => {
      const terms = this.normalizeSearch(search).trim().split(/\s+/u).filter(Boolean);
      return (session?.inventory ?? []).filter(item => matchesTradeCategory(item.category, category)
        && terms.every(term => this.normalizeSearch(item.name).includes(term)));
    }),
  );

  public setSearch(search: string): void { this.searchSubject.next(search); }

  private normalizeSearch(value: string): string {
    return value.normalize('NFKD').replace(/\p{M}/gu, '').toLocaleLowerCase()
      .replace(/ß/g, 'ss').replace(/ı/g, 'i').replace(/ς/g, 'σ');
  }

  public selectCategory(category: TradeCategory): void {
    if (tradeCategories.includes(category)) this.categorySubject.next(category);
  }
  private readonly outgoing = new BehaviorSubject<ReadonlySet<number>>(new Set());
  public readonly pendingOutgoing$ = this.outgoing.asObservable();
  private state?: TradeStatePayload;
  private previousView: View | null = null;
  private readonly incoming = new Set<number>();

  constructor(
    private readonly clientService: ClientService,
    private readonly playerListService: PlayerListService,
    private readonly popupNotificationService: PopupNotificationService,
    private readonly transloco: TranslocoService,
    private readonly uiRepository: UiRepository,
  ) {
    this.subscriptions.add(clientService.tradeStateChange.subscribe(state => this.applyState(state)));
    this.subscriptions.add(clientService.connectionStateChange.subscribe(connected => {
      if (!connected) this.reset();
    }));
    this.subscriptions.add(clientService.tradeInviteChange.subscribe(invite => {
      this.incoming.add(invite.inviterId);
      this.popupNotificationService.addTradeInvite(this.getDisplayName(invite.inviterId),
        () => this.respond(invite.inviterId, true), () => this.respond(invite.inviterId, false));
    }));
    this.subscriptions.add(clientService.tradeInviteExpiredChange.subscribe(id => this.incoming.delete(id)));
    this.subscriptions.add(clientService.tradeCancelledChange.subscribe(cancelled => {
      this.incoming.delete(cancelled.partnerId);
      this.removeOutgoing(cancelled.partnerId);
      const reasons = ['DECLINED', 'CANCELLED', 'PARTNER_BUSY', 'SELF_BUSY', 'PLAYER_LEFT', 'TIMEOUT', 'FAILED_VALIDATION'];
      this.popupNotificationService.addTradeCancelled(this.getDisplayName(cancelled.partnerId),
        this.transloco.translate('SERVICE.TRADE.CANCELLED_REASONS.' + (reasons[cancelled.reason] ?? 'UNKNOWN')));
      if (this.state?.partnerId === cancelled.partnerId) this.applyState(undefined);
    }));
    this.subscriptions.add(clientService.tradeCompletedChange.subscribe(id => {
      this.removeOutgoing(id);
      this.popupNotificationService.addTradeCompleted(this.getDisplayName(id));
      if (this.state?.partnerId === id) this.applyState(undefined);
    }));
    this.subscriptions.add(playerListService.playerList.subscribe(() => {
      if (this.state?.active) this.sessionSubject.next(this.toView(this.state));
    }));
  }

  ngOnDestroy(): void {
    this.subscriptions.unsubscribe();
    this.reset();
  }

  public sendInvite(id: number): void {
    if (id === this.clientService.localPlayerId || this.state?.active || this.outgoing.value.size) return;
    this.outgoing.next(new Set([id]));
    this.clientService.sendTradeInvite(id);
    this.popupNotificationService.addTradeInviteSent(this.getDisplayName(id), () => this.cancelInvite(id));
  }

  public cancelInvite(id: number): void {
    if (!this.outgoing.value.has(id)) return;
    this.clientService.cancelTrade();
    this.removeOutgoing(id);
  }

  private respond(id: number, accept: boolean): void {
    // Expired popups may still be visible, but their actions must not send stale responses.
    if (!this.incoming.delete(id)) return;
    this.clientService.respondTradeInvite(id, accept);
  }

  public cancelTrade(): void {
    if (this.state?.active) this.clientService.cancelTrade();
    this.applyState(undefined);
  }

  public setReady(ready: boolean): void {
    if (this.state?.active) this.clientService.setTradeReady(ready);
  }

  public updateOfferFromInput(index: number, value: string | number): void {
    this.setOfferCount(index, Number(value));
  }

  public addToOffer(index: number, delta: number): void {
    const item = this.sessionSubject.value?.inventory.find(item => item.index === index);
    if (item) this.setOfferCount(index, item.offered + delta);
  }

  public offerAll(index: number): void {
    const item = this.sessionSubject.value?.inventory.find(item => item.index === index);
    if (item) this.setOfferCount(index, item.available);
  }

  public clearOffer(index: number): void { this.setOfferCount(index, 0); }

  private setOfferCount(index: number, value: number): void {
    const state = this.state;
    const item = state?.inventory.find(item => item.inventoryIndex === index);
    if (!state?.active || !item || item.isQuestItem || item.isEquipped || !Number.isFinite(value)) return;
    const count = Math.max(0, Math.min(item.count, Math.floor(value)));
    const entries = state.inventory.map(entry => ({
      index: entry.inventoryIndex!,
      count: entry.inventoryIndex === index ? count : entry.offeredCount ?? 0,
    })).filter(entry => Number.isInteger(entry.index) && entry.count > 0);
    this.clientService.updateTradeOffer(entries);
  }

  public getDisplayName(id: number): string {
    return this.playerListService.playerList.value?.players.find(player => player.id === id)?.name
      ?? this.transloco.translate('SERVICE.TRADE.UNKNOWN_PLAYER', { id });
  }

  private removeOutgoing(id: number): void {
    const next = new Set(this.outgoing.value);
    next.delete(id);
    this.outgoing.next(next);
  }

  private reset(): void {
    this.incoming.clear();
    this.outgoing.next(new Set());
    this.applyState(undefined);
  }

  private applyState(state?: TradeStatePayload): void {
    const startingSession = state?.active && (!this.state?.active || this.state.partnerId !== state.partnerId);
    this.state = state?.active ? state : undefined;
    if (!this.state) {
      this.categorySubject.next('all');
      this.searchSubject.next('');
      this.sessionSubject.next(undefined);
      if (this.uiRepository.getView() === View.TRADE) this.uiRepository.openView(this.previousView);
      this.previousView = null;
      return;
    }
    this.incoming.delete(this.state.partnerId);
    this.removeOutgoing(this.state.partnerId);
    if (startingSession && this.uiRepository.getView() !== View.TRADE) {
      this.previousView = this.uiRepository.getView();
      this.uiRepository.openView(View.TRADE);
    }
    this.sessionSubject.next(this.toView(this.state));
  }

  private toView(state: TradeStatePayload): TradeSessionView {
    const offer = (item: TradeItemPayload, position: number): TradeOfferItemView => ({
      index: item.inventoryIndex, name: item.name, modId: item.modId, baseId: item.baseId,
      count: item.count, isQuestItem: item.isQuestItem, details: item.details ?? [],
      key: `${item.modId}:${item.baseId}:${position}`,
    });
    return {
      active: true, partnerId: state.partnerId, partnerName: this.getDisplayName(state.partnerId),
      initiatedBySelf: state.initiatedBySelf, selfReady: state.selfReady, partnerReady: state.partnerReady,
      selfOffer: state.selfItems.map(offer), partnerOffer: state.partnerItems.map(offer),
      inventory: state.inventory.filter(item => Number.isInteger(item.inventoryIndex)).map((item, position) => ({
        ...offer(item, position), index: item.inventoryIndex!, available: item.count,
        category: normalizeTradeCategory(item.category),
        isEquipped: item.isEquipped ?? false,
        offered: item.offeredCount ?? 0, isGold: item.isGold ?? false,
      })).sort((a, b) => a.name.localeCompare(b.name)),
      countdownMs: state.countdownMs, countdownTotalMs: state.countdownTotalMs,
      countdownProgress: state.countdownTotalMs > 0
        ? Math.max(0, Math.min(1, 1 - state.countdownMs / state.countdownTotalMs)) : 0,
    };
  }
}
