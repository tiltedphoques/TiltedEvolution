import { IconDefinition } from '@fortawesome/fontawesome-svg-core';

export interface PopupNotification {
  /** Translation Key. Plaintext should **NOT** be used */
  messageKey: string;
  messageParams?: Record<string, any>;
  imageUrl?: string;
  icon?: IconDefinition;
  /** milliseconds */
  duration?: number;
  actions?: PopupNotifactionAction[];
}

export interface PopupNotifactionAction {
  /** Translation Key. Plaintext should **NOT** be used */
  nameKey: string;
  callback: () => void;
}
