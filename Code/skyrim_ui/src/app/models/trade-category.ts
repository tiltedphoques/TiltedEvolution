export const tradeCategories = [
  'all', 'weapons', 'armor', 'potions', 'poisons', 'scrolls',
  'food', 'ingredients', 'books', 'keys', 'misc',
] as const;

export type TradeCategory = typeof tradeCategories[number];
const tradeItemCategories = [...tradeCategories.filter(category => category !== 'all'), 'ammunition', 'soul_gems'] as const;
export type TradeItemCategory = Exclude<TradeCategory, 'all'> | 'ammunition' | 'soul_gems';

export function normalizeTradeCategory(value: unknown): TradeItemCategory {
  return tradeItemCategories.includes(value as TradeItemCategory)
    ? value as TradeItemCategory : 'misc';
}

export function matchesTradeCategory(item: TradeItemCategory, filter: TradeCategory): boolean {
  if (filter === 'all' || filter === item) return true;
  // The broad Skyrim categories retain their normal inventory grouping.
  return (filter === 'weapons' && item === 'ammunition')
    || (filter === 'potions' && item === 'poisons')
    || (filter === 'misc' && item === 'soul_gems');
}
