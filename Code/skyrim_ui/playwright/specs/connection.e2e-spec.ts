import { createTest, expect } from '@ngx-playwright/test';
import { createClassXPathSelector } from '../helpers/selector.js';

import { ApplicationScreen } from '../screens/main-screen.js';


const test = createTest(ApplicationScreen);

test.describe('Connection', () => {

  test.beforeEach(async ({ page }) => {
    await page.waitForSelector('.app-root-controls', { state: 'attached' });
    await page.press('body', 'F2');
    await page.waitForSelector('.app-root-controls', { state: 'visible' });

    await page.click(`//app-window${ createClassXPathSelector('app-root-menu') }/button[1]`);
    await page.waitForSelector('//app-connect');
  });

  test('Check Invalid Inputs', async ({ page }) => {
    await page.locator('//app-connect/div[1]/input[1]').fill('t-port');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window[1]/div[1]/p[1]'))
      .toHaveText(/Could not connect to the specified server/);
    await page.click('//app-error[1]/app-popup[1]/app-window[1]/div[1]/app-action-buttons[1]/button[1]');

    await page.locator('//app-connect/div[1]/input[1]').fill('t-host');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window/div[1]/p[1]'))
      .toHaveText(/Could not connect to the specified server/);
    await page.click('//app-error[1]/app-popup[1]/app-window[1]/div[1]/app-action-buttons[1]/button[1]');

    await page.locator('//app-connect/div[1]/input[1]').fill('t-full');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window/div[1]/p[1]'))
      .toHaveText(/This server is full/);
    await page.click('//app-error[1]/app-popup[1]/app-window[1]/div[1]/app-action-buttons[1]/button[1]');

    await page.locator('//app-connect/div[1]/input[1]').fill('t-password');
    await page.locator('//app-connect/div[1]/input[2]').fill('wrong');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window/div[1]/p[1]'))
      .toHaveText(/The password you entered is incorrect/);
    await page.click('//app-error[1]/app-popup[1]/app-window[1]/div[1]/app-action-buttons[1]/button[1]');
    await page.locator('//app-connect/div[1]/input[2]').fill('');

    await page.locator('//app-connect/div[1]/input[1]').fill('t-version');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window/div[1]/p[1]'))
      .toHaveText(/\[current-version]/);
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window/div[1]/p[1]'))
      .toHaveText(/\[expected-version]/);
    await page.click('//app-error[1]/app-popup[1]/app-window[1]/div[1]/app-action-buttons[1]/button[1]');

    await page.locator('//app-connect/div[1]/input[1]').fill('t-client-mods');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window/div[1]/p[1]'))
      .toHaveText(/SKSE/);
    await page.click('//app-error[1]/app-popup[1]/app-window[1]/div[1]/app-action-buttons[1]/button[1]');

    await page.locator('//app-connect/div[1]/input[1]').fill('t-mods');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window/div[1]/p[1]'))
      .toHaveText(/remove\.esp, remove_2\.esp/);
    await expect(page.locator('//app-error[1]/app-popup[1]/app-window/div[1]/p[1]'))
      .toHaveText(/missing\.esp, missing_2\.esp/);
    await page.click('//app-error[1]/app-popup[1]/app-window[1]/div[1]/app-action-buttons[1]/button[1]');
  });

  test('Save & Load -> Address & Password', async ({ page }) => {
    const password = 'PW' + Math.floor(Math.random() * 100);
    const address = '192.168.178.' + Math.floor(Math.random() * 100);
    await page.locator('//app-connect/div[1]/input[1]').fill(address);
    await page.locator('//app-connect/div[1]/input[2]').fill(password);
    await page.click('.save-password app-checkbox');
    await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');

    const storageState = await page.context().storageState();
    const localStorage = storageState.origins[0].localStorage;
    await expect(localStorage.find(e => e.name === 'last_connected_address')?.value).toBe(address);
    await expect(localStorage.find(e => e.name === 'last_connected_password')?.value).toBe(password);

    await page.reload();
    await page.waitForSelector('.app-root-controls', { state: 'attached' });
    await page.press('body', 'F2');
    await page.waitForSelector('.app-root-controls', { state: 'visible' });

    await page.click(`//app-window${ createClassXPathSelector('app-root-menu') }/button[1]`);
    await page.waitForSelector('//app-connect');
    await expect(page.locator('//app-connect/div[1]/input[1]')).toHaveValue(address);
    await expect(page.locator('//app-connect/div[1]/input[2]')).toHaveValue(password);
  });

  test('Connect & Disconnect', async ({ page }) => {
    await test.step('connect', async () => {
      await page.locator('//app-connect/div[1]/input[1]').fill('t-password');
      await page.locator('//app-connect/div[1]/input[2]').fill('test');
      await page.click('//app-connect/div[1]/app-action-buttons[1]/button[1]');
      await expect(page.locator(`//app-window${ createClassXPathSelector('app-root-menu') }/button[1]`)).toHaveText(/Disconnect/);
    });

    await expect(page.evaluate('skyrimtogether.connected')).resolves.toBeTruthy();

    await test.step('disconnect', async () => {
      await page.click(`//app-window${ createClassXPathSelector('app-root-menu') }/button[1]`);
      await page.click(`//app-disconnect[1]/app-action-buttons[1]/button[1]`);
      await expect(page.locator(`//app-window${ createClassXPathSelector('app-root-menu') }/button[1]`)).toHaveText(/Connect/);
    });

    await expect(page.evaluate('skyrimtogether.connected')).resolves.toBeFalsy();
  });
});
