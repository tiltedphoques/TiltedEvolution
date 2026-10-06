export const tradeCategories = [
  'all', 'weapons', 'armor', 'ammunition', 'potions', 'poisons', 'scrolls',
  'food', 'ingredients', 'books', 'keys', 'soul_gems', 'misc',
] as const;

export type TradeCategory = typeof tradeCategories[number];
export type TradeItemCategory = Exclude<TradeCategory, 'all'>;

export function normalizeTradeCategory(value: unknown): TradeItemCategory {
  return value !== 'all' && tradeCategories.includes(value as TradeCategory)
    ? value as TradeItemCategory : 'misc';
}

export function matchesTradeCategory(item: TradeItemCategory, filter: TradeCategory): boolean {
  if (filter === 'all' || filter === item) return true;
  // The broad Skyrim categories retain their normal inventory grouping.
  return (filter === 'weapons' && item === 'ammunition')
    || (filter === 'potions' && item === 'poisons')
    || (filter === 'misc' && item === 'soul_gems');
}
