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
vm.runInNewContext(output, {
  exports: exported,
  require(name) {
    if (name === '@angular/core') return { Injectable: () => value => value };
    if (name === 'rxjs') return require(name);
    if (name.endsWith('view.enum')) return { View: { TRADE: 'trade' } };
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

test('closing an active trade cancels it and restores the previous target view', () => {
  const fixture = setup();
  fixture.client.tradeStateChange.next(state());
  assert.equal(fixture.ui.view, 'trade');
  fixture.service.closePopup();
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

test('navigation through the existing view repository also cancels a hidden trade', () => {
  const fixture = setup();
  fixture.client.tradeStateChange.next(state());
  fixture.ui.openView('party');
  assert.equal(fixture.calls.at(-1)[0], 'cancelTrade');
  assert.equal(fixture.ui.view, 'party');
  fixture.service.ngOnDestroy();
});
