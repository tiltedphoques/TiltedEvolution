export interface MockPlayer {
  id: number;
  name: string;
  level: number;
  health: number;
  cellName: string;
  hasOwnParty: boolean;
  isInGroup: boolean;
  invited: boolean;
  invitedLocalPlayer: boolean;
}
