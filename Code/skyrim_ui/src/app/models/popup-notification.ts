import { Player } from './player';


export enum NotificationType {
  Connection = 1,
  Invitation
}

export interface PopupNotification {
  message: string,
  type?: NotificationType,
  player?: Player,
}
