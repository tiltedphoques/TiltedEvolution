import { createTest, expect } from '@ngx-playwright/test';
import type { MockPlayer } from '../../src/app/mock/mock-player.js';
import { createClassXPathSelector } from '../helpers/selector.js';

import { ApplicationScreen } from '../screens/main-screen.js';


const test = createTest(ApplicationScreen);

test.describe('Group', () => {

  test.beforeEach(async ({ page }) => {
    await page.waitForSelector('.app-root-controls', { state: 'attached' });
    await page.press('body', 'F2');
    await page.waitForSelector('.app-root-controls', { state: 'visible' });

    // connect
    await page.click(`//app-window${ createClassXPathSelector('app-root-menu') }/button[1]`);
    await page.waitForSelector('//app-connect');
    await page.locator('//app-connect/div[1]/input[1]').fill('test');
    await page.locator('//app-connect/div[1]/input[2]').fill('test');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
    await expect(page.locator(`//app-window${ createClassXPathSelector('app-root-menu') }/button[1]`)).toHaveText(/Disconnect/);
  });

  test('Invite & Kick', async ({ page }) => {
    const player = await page.evaluate('skyrimtogether.addMockPlayer()') as MockPlayer;

    // open player manager
    await page.click(`//app-window${ createClassXPathSelector('app-root-menu') }/button[2]`);
    await page.click(`//app-player-manager/div[1]/button[2]`);

    // launch party
    await expect(await page.locator('.player-list-table').count()).toBe(0);
    await page.click(`//app-party-menu[1]/div[1]/button[1]`);
    await expect(await page.locator('.player-list-table').count()).toBe(0);
    await expect(page.locator(`//app-party-menu/div[1]/span[1]`)).toHaveText(/There is nobody in your party yet/);

    // open playerlist and invite player
    await page.click(`//app-player-manager/div[1]/button[1]`);
    await page.click(`//div${ createClassXPathSelector('player-list') }/table[1]/tbody[1]/tr[1]/td[4]/button[1]`);
    await expect(page.locator(`//div${ createClassXPathSelector('player-list') }/table[1]/tbody[1]/tr[1]/td[4]/button[1]`)).toBeDisabled();

    // accept invite
    await page.evaluate(`skyrimtogether.accteptMockPlayerInvite(${ player.id })`);
    await expect(page.locator(`//div${ createClassXPathSelector('player-list') }/table[1]/tbody[1]/tr[1]/td[4]/button[1]`)).toBeDisabled();

    // check member list
    await page.click(`//app-player-manager/div[1]/button[2]`);
    await expect(await page.locator(`//div${ createClassXPathSelector('member-list') }/table[1]/tbody[1]/tr`).count()).toBe(1);
    await expect(page.locator(`//div${ createClassXPathSelector('member-list') }/table[1]/tbody[1]/tr[1]/td[1]`)).toHaveText(`${ player.level }`);
    await expect(page.locator(`//div${ createClassXPathSelector('member-list') }/table[1]/tbody[1]/tr[1]/td[2]`)).toHaveText(player.name);
    await expect(page.locator(`//div${ createClassXPathSelector('member-list') }/table[1]/tbody[1]/tr[1]/td[3]`)).toHaveText(player.cellName);

    // kick member
    await page.click(`//div${ createClassXPathSelector('member-list') }/table[1]/tbody[1]/tr[1]/td[4]/button[2]`);
    await expect(await page.locator('.player-list-table').count()).toBe(0);

    // check invite button
    await page.click(`//app-player-manager/div[1]/button[1]`);
    await expect(page.locator(`//div${ createClassXPathSelector('player-list') }/table[1]/tbody[1]/tr[1]/td[4]/button[1]`)).toBeEnabled();
  });

  test('Launch & Leave Party', async ({ page }) => {
    // open player manager
    await page.click(`//app-window${ createClassXPathSelector('app-root-menu') }/button[2]`);
    await page.click(`//app-player-manager/div[1]/button[2]`);

    // launch party
    await expect(page.locator(`//app-party-menu/div[1]/button[1]`)).toHaveText(/Launch party/);
    await page.click(`//app-party-menu/div[1]/button[1]`);

    // leave party
    await expect(page.locator(`//app-party-menu/button[1]`)).toHaveText(/Leave party/);
    await page.click(`//app-party-menu/button[1]`);

    // check if the launch button reappeared
    await expect(page.locator(`//app-party-menu/div[1]/button[1]`)).toHaveText(/Launch party/);

    // close the player manager
    await page.click(`//app-player-manager/div${ createClassXPathSelector('actions') }[1]//button[1]`);
    await page.waitForSelector('//app-player-manager', { state: 'detached' });
  });
});
