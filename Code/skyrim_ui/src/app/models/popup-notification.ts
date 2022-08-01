import { Subject } from 'rxjs';
import { Player } from './player';


export enum NotificationType {
  Connection = 1,
  Invitation
}

export interface PopupNotification {
  message: string;
  type?: NotificationType;
  player?: Player;
  duration?: number;
  onClose: Subject<boolean>;
}

export interface PopupNotifactionOptions {
  type?: NotificationType;
  player?: Player;
  duration?: number;
}
