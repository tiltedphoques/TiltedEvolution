const { test } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const ts = require('typescript');
const { BehaviorSubject, Subject } = require('rxjs');

const source = fs.readFileSync(path.join(__dirname, '../src/app/services/trade-ui.service.ts'), 'utf8');
const output = ts.transpileModule(source, {
  compilerOptions: { module: ts.ModuleKind.CommonJS, target: ts.ScriptTarget.ES2020, experimentalDecorators: true },
}).outputText;
const exported = {};
const categoryExports = {};
const categorySource = fs.readFileSync(path.join(__dirname, '../src/app/models/trade-category.ts'), 'utf8');
vm.runInNewContext(ts.transpileModule(categorySource, {
  compilerOptions: { module: ts.ModuleKind.CommonJS, target: ts.ScriptTarget.ES2020 },
}).outputText, { exports: categoryExports });
vm.runInNewContext(output, {
  exports: exported,
  require(name) {
    if (name === '@angular/core') return { Injectable: () => value => value };
    if (name === 'rxjs') return require(name);
    if (name.endsWith('view.enum')) return { View: { TRADE: 'trade' } };
    if (name.endsWith('trade-category')) return categoryExports;
    return {};
  },
});

function setup() {
  const calls = [];
  const client = {
    localPlayerId: 1,
    tradeStateChange: new BehaviorSubject(undefined),
    connectionStateChange: new BehaviorSubject(true),
    tradeInviteChange: new Subject(), tradeInviteExpiredChange: new Subject(),
    tradeCancelledChange: new Subject(), tradeCompletedChange: new Subject(),
  };
  for (const method of ['sendTradeInvite', 'respondTradeInvite', 'cancelTrade', 'setTradeReady', 'updateTradeOffer']) {
    client[method] = (...args) => calls.push([method, ...args]);
  }
  const players = { playerList: new BehaviorSubject({ players: [{ id: 2, name: 'Partner' }] }) };
  const notifications = [];
  const popups = Object.fromEntries(['addTradeInvite', 'addTradeInviteSent', 'addTradeCancelled', 'addTradeCompleted']
    .map(method => [method, (...args) => notifications.push([method, ...args])]));
  const ui = {
    view: 'players', view$: new BehaviorSubject('players'), getView() { return this.view; },
    openView(view) { this.view = view; this.view$.next(view); },
  };
  const service = new exported.TradeUiService(client, players, popups, { translate: key => key }, ui);
  return { client, calls, service, notifications, ui };
}

function state() {
  return {
    active: true, partnerId: 2, initiatedBySelf: true, selfReady: false, partnerReady: false,
    selfItems: [], partnerItems: [], countdownMs: 0, countdownTotalMs: 0,
    inventory: [
      { modId: 0, baseId: 1, name: 'Z item', count: 10, isQuestItem: false, inventoryIndex: 7, offeredCount: 2 },
      { modId: 0, baseId: 2, name: 'A item', count: 5, isQuestItem: false, inventoryIndex: 3, offeredCount: 1 },
    ],
  };
}

test('custom names are searchable only in inventory while both offers retain base names', () => {
  const fixture = setup();
  const payload = state();
  const item = { ...payload.inventory[0], name: 'Iron Sword', customNames: ['Dragon Slayer', 'Épée du héros'] };
  payload.inventory = [item];
  payload.selfItems = [item];
  payload.partnerItems = [item];
  let visible;
  let session;
  const inventorySubscription = fixture.service.visibleInventory$.subscribe(items => visible = items);
  const sessionSubscription = fixture.service.session$.subscribe(value => session = value);
  fixture.client.tradeStateChange.next(payload);
  assert.equal(visible[0].name, 'Dragon Slayer / Épée du héros');
  assert.equal(visible[0].baseName, 'Iron Sword');
  assert.equal(session.selfOffer[0].name, 'Iron Sword');
  assert.equal(session.partnerOffer[0].name, 'Iron Sword');
  for (const search of ['dragon', 'epee heros', 'iron sword', 'slayer iron']) {
    fixture.service.setSearch(search);
    assert.equal(visible.length, 1, search);
  }
  fixture.service.updateOfferFromInput(7, 3);
  assert.deepEqual(Object.keys(fixture.calls.at(-1)[1][0]).sort(), ['count', 'index']);
  assert.equal(payload.inventory[0].name, 'Iron Sword');
  inventorySubscription.unsubscribe();
  sessionSubscription.unsubscribe();
  fixture.service.ngOnDestroy();
});

test('offers retain native indices after display sorting without mutating payloads', () => {
  const fixture = setup();
  const payload = state();
  payload.inventory.forEach(Object.freeze);
  Object.freeze(payload.inventory);
  Object.freeze(payload);
  fixture.client.tradeStateChange.next(payload);
  fixture.service.updateOfferFromInput(3, 4);
  const entries = fixture.calls.at(-1)[1];
  assert.equal(entries[0].index, 7);
  assert.equal(entries[0].count, 2);
  assert.equal(entries[1].index, 3);
  assert.equal(entries[1].count, 4);
  fixture.service.ngOnDestroy();
});

test('disconnect clears outgoing invitations so reconnection can trade', () => {
  const fixture = setup();
  fixture.service.sendInvite(2);
  fixture.client.connectionStateChange.next(false);
  fixture.client.connectionStateChange.next(true);
  fixture.service.sendInvite(2);
  assert.equal(fixture.calls.filter(call => call[0] === 'sendTradeInvite').length, 2);
  fixture.service.ngOnDestroy();
});

test('expired notification actions cannot respond to an obsolete invitation', () => {
  const fixture = setup();
  fixture.client.tradeInviteChange.next({ inviterId: 2, expiryTick: 1000 });
  const accept = fixture.notifications.at(-1)[2];
  fixture.client.tradeInviteExpiredChange.next(2);
  accept();
  assert.equal(fixture.calls.length, 0);
  fixture.service.ngOnDestroy();
});

test('Cancel Trade explicitly cancels the exchange and restores the previous target view', () => {
  const fixture = setup();
  fixture.client.tradeStateChange.next(state());
  assert.equal(fixture.ui.view, 'trade');
  fixture.service.cancelTrade();
  assert.equal(fixture.calls.at(-1)[0], 'cancelTrade');
  assert.equal(fixture.ui.view, 'players');
  fixture.service.ngOnDestroy();
});

test('quest items, equipped items and nonfinite counts never enter an offer', () => {
  const fixture = setup();
  const payload = state();
  payload.inventory[0].isQuestItem = true;
  payload.inventory[1].isEquipped = true;
  fixture.client.tradeStateChange.next(payload);
  fixture.service.updateOfferFromInput(7, 2);
  fixture.service.updateOfferFromInput(3, 2);
  fixture.service.updateOfferFromInput(7, NaN);
  assert.equal(fixture.calls.length, 0);
  fixture.service.ngOnDestroy();
});

test('navigation and subsequent state updates preserve an exchange without reopening it', () => {
  const fixture = setup();
  fixture.client.tradeStateChange.next(state());
  fixture.ui.openView('party');
  fixture.client.tradeStateChange.next({ ...state(), selfReady: true, countdownMs: 2000 });
  assert.equal(fixture.calls.length, 0);
  assert.equal(fixture.ui.view, 'party');
  fixture.service.ngOnDestroy();
});

test('category filtering preserves offers and native indices for hidden items', () => {
  const fixture = setup();
  const payload = state();
  payload.inventory[0].category = 'weapons';
  payload.inventory[1].category = 'food';
  payload.selfItems = [{ ...payload.inventory[0], count: 2 }];
  let visible;
  let session;
  const inventorySubscription = fixture.service.visibleInventory$.subscribe(items => visible = items);
  const sessionSubscription = fixture.service.session$.subscribe(value => session = value);
  fixture.client.tradeStateChange.next(payload);
  fixture.service.selectCategory('food');
  assert.equal(visible.length, 1);
  assert.equal(visible[0].index, 3);
  assert.equal(session.selfOffer.length, 1);
  assert.equal(session.selfOffer[0].count, 2);
  fixture.service.updateOfferFromInput(3, 4);
  const selections = fixture.calls.at(-1)[1];
  assert.equal(selections.find(item => item.index === 7).count, 2);
  assert.equal(selections.find(item => item.index === 3).count, 4);
  fixture.service.selectCategory('all');
  assert.equal(visible.length, 2);
  inventorySubscription.unsubscribe();
  sessionSubscription.unsubscribe();
  fixture.service.ngOnDestroy();
});

test('category selection survives inventory updates and resets for the next session', () => {
  const fixture = setup();
  const payload = state();
  payload.inventory[0].category = 'weapons';
  payload.inventory[1].category = 'food';
  let visible;
  const subscription = fixture.service.visibleInventory$.subscribe(items => visible = items);
  fixture.client.tradeStateChange.next(payload);
  fixture.service.selectCategory('weapons');
  fixture.client.tradeStateChange.next({ ...payload, inventory: [payload.inventory[1]] });
  assert.equal(visible.length, 0);
  fixture.client.tradeStateChange.next(undefined);
  fixture.client.tradeStateChange.next(payload);
  assert.equal(visible.length, 2);
  subscription.unsubscribe();
  fixture.service.ngOnDestroy();
});

test('broad categories include ammo, poisons and soul gems; unknown types remain accessible', () => {
  const fixture = setup();
  const payload = state();
  payload.inventory = ['weapons', 'ammunition', 'potions', 'poisons', 'soul_gems', 'unrecognized'].map((category, index) => ({
    ...payload.inventory[0], category, inventoryIndex: index,
  }));
  let visible;
  const subscription = fixture.service.visibleInventory$.subscribe(items => visible = items);
  fixture.client.tradeStateChange.next(payload);
  fixture.service.selectCategory('weapons');
  assert.equal(visible.length, 2);
  fixture.service.selectCategory('potions');
  assert.equal(visible.length, 2);
  fixture.service.selectCategory('misc');
  assert.equal(visible.length, 2);
  assert.equal(fixture.service.categories.includes('soul_gems'), false);
  assert.equal(fixture.service.categories.includes('ammunition'), false);
  subscription.unsubscribe();
  fixture.service.ngOnDestroy();
});

test('F2 activation changes retain the trade view, ready status and countdown', () => {
  const rootExports = {};
  const rootSource = fs.readFileSync(path.join(__dirname, '../src/app/components/root/root.component.ts'), 'utf8');
  vm.runInNewContext(ts.transpileModule(rootSource, {
    compilerOptions: { module: ts.ModuleKind.CommonJS, target: ts.ScriptTarget.ES2020, experimentalDecorators: true },
  }).outputText, {
    exports: rootExports, setTimeout,
    require(name) {
      if (name === '@angular/core') return { Component: () => value => value, ViewChild: () => () => {} };
      if (name.startsWith('rxjs')) return require(name);
      if (name.endsWith('view.enum')) return { View: { TRADE: 'trade' } };
      if (name.endsWith('/environment')) return { environment: { game: false } };
      return {};
    },
  });
  const fixture = setup();
  fixture.ui.isViewOpen = () => fixture.ui.view !== null;
  fixture.client.activationStateChange = new Subject();
  fixture.client.inGameStateChange = new BehaviorSubject(true);
  const root = Object.create(rootExports.RootComponent.prototype);
  root.client = fixture.client;
  root.uiRepository = fixture.ui;
  root.destroy$ = new Subject();
  root.onActivationStateSubscription();
  let session;
  const subscription = fixture.service.session$.subscribe(value => session = value);
  fixture.client.tradeStateChange.next({ ...state(), selfReady: true, countdownMs: 3000 });
  fixture.client.activationStateChange.next(false);
  fixture.client.tradeStateChange.next({ ...state(), selfReady: true, countdownMs: 2000 });
  fixture.client.activationStateChange.next(true);
  assert.equal(fixture.ui.view, 'trade');
  assert.equal(session.selfReady, true);
  assert.equal(session.countdownMs, 2000);
  assert.equal(fixture.calls.length, 0);
  root.destroy$.next();
  subscription.unsubscribe();
  fixture.service.ngOnDestroy();
});

test('partial text search handles localized names and combines with categories without changing offers', () => {
  const fixture = setup();
  const payload = state();
  payload.inventory = ['Épée enchantée', 'Двемерская стрела', '鋼鉄の剣', 'Straße', 'Ilık çorba'].map((name, index) => ({
    ...payload.inventory[0], name, category: index === 4 ? 'food' : 'weapons', inventoryIndex: index,
  }));
  let visible;
  const subscription = fixture.service.visibleInventory$.subscribe(items => visible = items);
  fixture.client.tradeStateChange.next(payload);
  for (const [query, expected] of [['EPE ENCH', 0], ['МЕРСК', 1], ['鉄の', 2], ['STRASS', 3], ['ılık', 4]]) {
    fixture.service.setSearch(query);
    assert.equal(visible.length, 1);
    assert.equal(visible[0].index, expected);
  }
  fixture.service.selectCategory('weapons');
  assert.equal(visible.length, 0);
  assert.equal(fixture.calls.length, 0);
  fixture.service.setSearch('');
  assert.equal(visible.length, 4);
  fixture.client.tradeStateChange.next(undefined);
  fixture.client.tradeStateChange.next(payload);
  assert.equal(visible.length, 5);
  subscription.unsubscribe();
  fixture.service.ngOnDestroy();
});

test('row Max chooses the maximum and Clear chooses zero while preserving other offers', () => {
  const fixture = setup();
  fixture.client.tradeStateChange.next(state());
  fixture.service.offerAll(3);
  assert.equal(fixture.calls.at(-1)[1].find(item => item.index === 3).count, 5);
  assert.equal(fixture.calls.at(-1)[1].find(item => item.index === 7).count, 2);
  fixture.service.clearOffer(3);
  assert.equal(fixture.calls.at(-1)[1].some(item => item.index === 3), false);
  assert.equal(fixture.calls.at(-1)[1].find(item => item.index === 7).count, 2);
  fixture.service.ngOnDestroy();
});

test('quest items and internal fists are hidden while native indices and other offers remain intact', () => {
  const fixture = setup();
  const payload = state();
  payload.inventory = [
    { ...payload.inventory[0], inventoryIndex: 0, isQuestItem: true },
    { ...payload.inventory[0], inventoryIndex: 1, baseId: 0x1f4, offeredCount: 0 },
    { ...payload.inventory[1], inventoryIndex: 9 },
  ];
  let visible;
  const subscription = fixture.service.visibleInventory$.subscribe(items => visible = items);
  fixture.client.tradeStateChange.next(payload);
  assert.equal(visible.length, 1);
  assert.equal(visible[0].index, 9);
  fixture.service.offerAll(0);
  fixture.service.offerAll(1);
  assert.equal(fixture.calls.length, 0);
  fixture.service.offerAll(9);
  assert.equal(fixture.calls.at(-1)[1].length, 1);
  assert.equal(fixture.calls.at(-1)[1][0].index, 9);
  subscription.unsubscribe();
  fixture.service.ngOnDestroy();
});
