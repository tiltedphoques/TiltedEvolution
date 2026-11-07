import { environment as browser } from './environment.browser';
import { environment as fallout } from './environment.fallout';

// Browser mock with the Fallout texts, for `npm run start:fallout`.
export const environment = {
  ...browser,
  textReplacements: fallout.textReplacements,
};
