import { InjectionToken } from '@angular/core';

/**
 * Lets player privacy settings decide who may invite you to trade and which
 * name trade popups and the trade window show. Without a provider every
 * invite is allowed and real names are shown.
 */
export interface TradeInvitePolicy {
  acceptsTradeInvite(playerId: number): boolean;
  displayName(playerId: number, name: string): string;
}

export const TRADE_INVITE_POLICY = new InjectionToken<TradeInvitePolicy>(
  'TradeInvitePolicy',
);
