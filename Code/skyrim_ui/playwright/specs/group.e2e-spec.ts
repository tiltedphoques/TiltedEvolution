import { createTest, expect } from '@ngx-playwright/test';
import type { Page } from '@ngx-playwright/test';
import type { MockPlayer } from '../../src/app/mock/mock-player.js';
import { ApplicationScreen } from '../screens/main-screen.js';

const test = createTest(ApplicationScreen);

async function connect(page: Page) {
  await page
    .locator('.app-root-menu')
    .getByRole('button', { name: 'Connect', exact: true })
    .click();
  await page.locator('app-connect input').nth(0).fill('test');
  await page.locator('app-connect input').nth(1).fill('test');
  await page.locator('app-connect app-action-buttons button').first().click();
  await expect(
    page
      .locator('.app-root-menu')
      .getByRole('button', { name: 'Disconnect', exact: true }),
  ).toBeVisible();
}

async function openPartyMenu(page: Page) {
  if (!(await page.locator('app-player-manager').count())) {
    await page
      .locator('.app-root-menu')
      .getByRole('button', { name: 'Player Manager', exact: true })
      .click();
  }
  await page
    .locator('app-player-manager')
    .getByRole('button', { name: 'Party Menu', exact: true })
    .click();
}

async function openPlayerList(page: Page) {
  await page
    .locator('app-player-manager')
    .getByRole('button', { name: 'Player List', exact: true })
    .click();
}

async function closePlayerManager(page: Page) {
  await page
    .locator('app-player-manager')
    .getByRole('button', { name: 'Back', exact: true })
    .click();
  await expect(page.locator('app-player-manager')).toHaveCount(0);
}

async function addPlayer(page: Page): Promise<MockPlayer> {
  return page.evaluate('skyrimtogether.addMockPlayer()');
}

function inviteButton(page: Page, player: MockPlayer) {
  return page
    .locator('.player-list tbody tr')
    .filter({ hasText: player.name })
    .getByRole('button', { name: 'Invite', exact: true });
}

function receivedInvite(page: Page, player: MockPlayer) {
  return page.locator('app-party-menu').getByRole('button', {
    name: `Accept invite from ${player.name}`,
    exact: true,
  });
}

async function receiveInvite(
  page: Page,
  player: MockPlayer,
  expiresInMs: number,
) {
  await page.evaluate(
    ({ id, expiresInMs }) => {
      const mock = (window as any).skyrimtogether;
      mock.startPlayerMockParty(id);
      mock.emit('partyInviteReceived', id, expiresInMs);
    },
    { id: player.id, expiresInMs },
  );
}

test.describe('Group', () => {
  test.beforeEach(async ({ page }) => {
    await page.waitForSelector('.app-root-controls', { state: 'attached' });
    await page.press('body', 'F2');
    await page.waitForSelector('.app-root-controls', { state: 'visible' });
    await connect(page);

    // Observe calls at the native bridge, without granting the UI an invitation.
    await page.evaluate(() => {
      const mock = (window as any).skyrimtogether;
      for (const method of ['createPartyInvite', 'acceptPartyInvite']) {
        const original = mock[method].bind(mock);
        mock[`${method}Calls`] = [];
        mock[method] = (id: number) => {
          mock[`${method}Calls`].push(id);
          original(id);
        };
      }
    });
  });

  test('Invite & Kick', async ({ page }) => {
    const player = await addPlayer(page);
    await openPartyMenu(page);
    await page
      .getByRole('button', { name: 'Launch party', exact: true })
      .click();
    await expect(page.locator('app-party-menu .no-players')).toContainText(
      'There is nobody in your party yet',
    );

    await openPlayerList(page);
    await inviteButton(page, player).click();
    await expect(inviteButton(page, player)).toBeDisabled();
    await page.evaluate(`skyrimtogether.accteptMockPlayerInvite(${player.id})`);
    await expect(inviteButton(page, player)).toBeDisabled();

    await openPartyMenu(page);
    const member = page.locator('.member-list tbody tr');
    await expect(member).toHaveCount(1);
    await expect(member.locator('td').nth(0)).toHaveText(`${player.level}`);
    await expect(member.locator('td').nth(1)).toHaveText(player.name);
    await expect(member.locator('td').nth(2)).toHaveText(player.cellName);
    await member.getByRole('button', { name: 'Kick', exact: true }).click();
    await expect(member).toHaveCount(0);

    await openPlayerList(page);
    await expect(inviteButton(page, player)).toBeEnabled();
    await inviteButton(page, player).click();
    await expect(
      page.evaluate('skyrimtogether.createPartyInviteCalls'),
    ).resolves.toEqual([player.id, player.id]);
  });

  test('Ignored invites can be retried after a short per-player cooldown', async ({
    page,
  }) => {
    const player = await addPlayer(page);
    const otherPlayer = await addPlayer(page);
    await openPartyMenu(page);
    await page
      .getByRole('button', { name: 'Launch party', exact: true })
      .click();
    await openPlayerList(page);

    await inviteButton(page, player).click();
    await expect(inviteButton(page, player)).toBeDisabled();
    await expect(inviteButton(page, otherPlayer)).toBeEnabled();
    await expect(inviteButton(page, player)).toBeEnabled({ timeout: 4500 });
    await inviteButton(page, player).click();
    await expect(inviteButton(page, player)).toBeDisabled();
    await expect(
      page.evaluate('skyrimtogether.createPartyInviteCalls'),
    ).resolves.toEqual([player.id, player.id]);
  });

  test('Received invitation expires and its stale popup cannot accept it', async ({
    page,
  }) => {
    const player = await addPlayer(page);
    await openPartyMenu(page);
    await receiveInvite(page, player, 1500);
    await expect(receivedInvite(page, player)).toBeVisible();
    const popup = page
      .locator('app-notification-popup')
      .filter({ hasText: player.name });
    await closePlayerManager(page);
    // Hover pauses the presentation timer, but must not extend the invitation.
    await popup.hover();
    await page.waitForTimeout(1600);
    await popup.getByRole('button', { name: 'Accept', exact: true }).click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([]);
    await openPartyMenu(page);
    await expect(receivedInvite(page, player)).toHaveCount(0);
    await expect(
      page.getByRole('button', { name: 'Launch party', exact: true }),
    ).toBeVisible();
  });

  test('Renewing an invite cancels its old deadline and invalidates its old popup', async ({
    page,
  }) => {
    const player = await addPlayer(page);
    await openPartyMenu(page);
    await receiveInvite(page, player, 1800);
    const popups = page
      .locator('app-notification-popup')
      .filter({ hasText: player.name });
    await closePlayerManager(page);
    await popups.first().hover();
    await receiveInvite(page, player, 6000);
    await expect(popups).toHaveCount(2);
    await popups
      .first()
      .getByRole('button', { name: 'Accept', exact: true })
      .click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([]);
    await openPartyMenu(page);
    await page.waitForTimeout(1900);
    await expect(receivedInvite(page, player)).toBeVisible();
    await receivedInvite(page, player).click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([player.id]);
    await expect(
      page.getByRole('button', { name: 'Leave party', exact: true }),
    ).toBeVisible();
  });

  test('Joining and leaving clears every old received invite', async ({
    page,
  }) => {
    const player = await addPlayer(page);
    const otherPlayer = await addPlayer(page);
    await openPartyMenu(page);
    await receiveInvite(page, player, 60000);
    await receiveInvite(page, otherPlayer, 60000);
    await receivedInvite(page, player).click();
    await page
      .getByRole('button', { name: 'Leave party', exact: true })
      .click();
    await expect(receivedInvite(page, player)).toHaveCount(0);
    await expect(receivedInvite(page, otherPlayer)).toHaveCount(0);
    const stalePopup = page
      .locator('app-notification-popup')
      .filter({ hasText: otherPlayer.name });
    await closePlayerManager(page);
    await stalePopup
      .getByRole('button', { name: 'Accept', exact: true })
      .click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([player.id]);

    await receiveInvite(page, otherPlayer, 60000);
    await openPartyMenu(page);
    await receivedInvite(page, otherPlayer).click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([player.id, otherPlayer.id]);
  });

  test('A rejected acceptance preserves another valid invitation', async ({
    page,
  }) => {
    const player = await addPlayer(page);
    const otherPlayer = await addPlayer(page);
    await openPartyMenu(page);
    await receiveInvite(page, player, 60000);
    await receiveInvite(page, otherPlayer, 60000);
    await page.evaluate(rejectedId => {
      const mock = (window as any).skyrimtogether;
      const accept = mock.acceptPartyInvite.bind(mock);
      mock.acceptPartyInvite = (id: number) => {
        if (id === rejectedId) {
          // A stale server invite produces no successful partyInfo response.
          mock.acceptPartyInviteCalls.push(id);
          return;
        }
        accept(id);
      };
    }, player.id);

    await receivedInvite(page, player).click();
    await expect(receivedInvite(page, player)).toHaveCount(0);
    await expect(receivedInvite(page, otherPlayer)).toBeVisible();
    await receivedInvite(page, otherPlayer).click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([player.id, otherPlayer.id]);
    await expect(
      page.getByRole('button', { name: 'Leave party', exact: true }),
    ).toBeVisible();
  });

  test('Leaving and relaunching permits an immediate fresh invite', async ({
    page,
  }) => {
    const player = await addPlayer(page);
    await openPartyMenu(page);
    await page
      .getByRole('button', { name: 'Launch party', exact: true })
      .click();
    await openPlayerList(page);
    await inviteButton(page, player).click();
    await openPartyMenu(page);
    await page
      .getByRole('button', { name: 'Leave party', exact: true })
      .click();
    await page
      .getByRole('button', { name: 'Launch party', exact: true })
      .click();
    await openPlayerList(page);
    await expect(inviteButton(page, player)).toBeEnabled();
    await inviteButton(page, player).click();
    await expect(inviteButton(page, player)).toBeDisabled();
    await expect(
      page.evaluate('skyrimtogether.createPartyInviteCalls'),
    ).resolves.toEqual([player.id, player.id]);
  });

  test('Disconnecting clears invites and rejects callbacks from the previous connection', async ({
    page,
  }) => {
    const player = await addPlayer(page);
    await openPartyMenu(page);
    await receiveInvite(page, player, 60000);
    await page
      .locator('app-player-manager')
      .getByRole('button', { name: 'Back', exact: true })
      .click();
    await page
      .locator('.app-root-menu')
      .getByRole('button', { name: 'Disconnect', exact: true })
      .click();
    await page
      .locator('app-disconnect')
      .getByRole('button', { name: 'Proceed', exact: true })
      .click();
    await connect(page);
    await openPartyMenu(page);
    await expect(receivedInvite(page, player)).toHaveCount(0);
    await receiveInvite(page, player, 60000);
    const popups = page
      .locator('app-notification-popup')
      .filter({ hasText: player.name });
    await closePlayerManager(page);
    await popups
      .first()
      .getByRole('button', { name: 'Accept', exact: true })
      .click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([]);
    await openPartyMenu(page);
    await receivedInvite(page, player).click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([player.id]);
  });

  test('A disconnected inviter cannot be accepted from its popup', async ({
    page,
  }) => {
    const player = await addPlayer(page);
    await openPartyMenu(page);
    await receiveInvite(page, player, 60000);
    await page.evaluate(`skyrimtogether.disconnectMockPlayer(${player.id})`);
    await expect(receivedInvite(page, player)).toHaveCount(0);
    await closePlayerManager(page);
    await page
      .locator('app-notification-popup')
      .filter({ hasText: player.name })
      .getByRole('button', { name: 'Accept', exact: true })
      .click();
    await expect(
      page.evaluate('skyrimtogether.acceptPartyInviteCalls'),
    ).resolves.toEqual([]);
  });

  test('Launch & Leave Party', async ({ page }) => {
    await openPartyMenu(page);
    await page
      .getByRole('button', { name: 'Launch party', exact: true })
      .click();
    await page
      .getByRole('button', { name: 'Leave party', exact: true })
      .click();
    await expect(
      page.getByRole('button', { name: 'Launch party', exact: true }),
    ).toBeVisible();
    await page
      .locator('app-player-manager')
      .getByRole('button', { name: 'Back', exact: true })
      .click();
    await expect(page.locator('app-player-manager')).toHaveCount(0);
  });
});
