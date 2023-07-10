import type { PlaywrightTestConfig } from '@ngx-playwright/test';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

// @ts-ignore
const __dirname = dirname(fileURLToPath(import.meta.url));

const config: PlaywrightTestConfig = {
  use: {
    channel: 'chrome',
    headless: true,
  },

  testDir: join(__dirname, 'specs'),
  testMatch: '**/*.e2e-spec.ts',

  reporter: [
    [process.env.GITHUB_ACTION ? 'github' : 'list'],
    // ['html', { outputFolder: join(__dirname, 'results') }],
    ['junit', { outputFile: join(__dirname, 'results/junit.xml') }],
  ],
};

export default config;
