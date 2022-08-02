import { createTest, expect } from '@ngx-playwright/test';

import { ApplicationScreen } from '../screens/main-screen.js';


const test = createTest(ApplicationScreen);

test.describe('Main Screen', () => {
  test('Title', async ({ page }) => {
    await expect(page.title()).resolves.toBe('Skyrim: Together UI');
  });

  test('Open UI Pressing F2', async ({ page }) => {
    await page.waitForSelector('.app-root-controls', { state: 'attached' });
    await page.press('body', 'F2');
    await page.waitForSelector('.app-root-controls', { state: 'visible' });
  });

  // there is no way to emulate the right control key
  // test('it should open the UI when pressing R-CTRL', async ({ page }) => {
  //   await page.waitForSelector('.app-root-controls', { state: 'attached' });
  //   await page.press('body', 'R-CTRL');
  //   await page.waitForSelector('.app-root-controls', { state: 'visible' });
  // });
});
