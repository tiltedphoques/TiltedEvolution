declare const module: NodeModule;

interface NodeModule {
  id: string;
}

/** Skyim: Together type definitions */
declare namespace SkyrimTogetherTypes {
  /** Client initialization callback */
  type InitCallback = () => void;

  /** UI activation callback */
  type ActivateCallback = () => void;

  /** UI deactivation callback */
  type DeactivateCallback = () => void;

  /** Player game entry callback */
  type EnterGameCallback = () => void;

  /** Player game exit callback */
  type ExitGameCallback = () => void;

  /** Player open/close game menu callback */
  type OpeningMenuCallback = (openingMenu: boolean) => void;

  /** Player message reception callback */
  type MessageCallback = (name: string, message: string) => void;

  /** System message reception callback */
  type SystemMessageCallback = (message: string) => void;

  /** Connection callback */
  type ConnectCallback = () => void;

  /** Disconnection callback */
  type DisconnectCallback = (isError: boolean) => void;

  /** Name change callback */
  type NameChangeCallback = (name: string) => void;

  /** Version set callback */
  type VersionSetCallback = (version: string) => void;

  /**  */
  type OnDebugCallback = (isDebug: boolean) => void;

  type UpdateDebugCallback = (
    numPacketsSent: number,
    numPacketsReceived: number,
    RTT: number,
    packetLoss: number,
    sentBandwidth: number,
    receivedBandwidth: number
  ) => void;

  type UserDataSetCallback = (token: string, username: string) => void;

  type PlayerConnectedCallback = (serverId: number, username: string, level: number, cellName: string) => void;

  type PlayerDisconnectedCallback = (serverId: number, username: string) => void;

  type HealthCallback = (serverId: number, health: number) => void;

  type SetLevelCallback = (serverId: number, level: number) => void;

  type SetCellCallback = (serverId: number, cellName: string) => void;

  type SetPlayer3DLoadedCallback = (serverId: number, isLoaded: boolean) => void;

  type SetServerIdCallback = (serverId: number) => void;

  type ProtocolMismatch = () => void;

  type TriggerError = () => void;
}

/** Global Skyrim: Together object. */
declare const skyrimtogether: SkyrimTogether;

/** Global Skyrim: Together object type. */
interface SkyrimTogether {
  /** Add listener to when the UI is first initialized. */
  on(event: 'init', callback: SkyrimTogetherTypes.InitCallback): void;

  /** Add listener to when the UI is activated. */
  on(event: 'activate', callback: SkyrimTogetherTypes.ActivateCallback): void;

  /** Add listener to when the UI is deactivated. */
  on(event: 'deactivate', callback: SkyrimTogetherTypes.DeactivateCallback): void;

  /** Add listener to when the player enters a game. */
  on(event: 'entergame', callback: SkyrimTogetherTypes.EnterGameCallback): void;

  /** Add listener to when the player exits a game. */
  on(event: 'exitgame', callback: SkyrimTogetherTypes.ExitGameCallback): void;

  /** Add listener to when the player open/close a game menu. */
  on(event: 'openingmenu', callback: SkyrimTogetherTypes.OpeningMenuCallback): void;

  /** Add listener to when a player message is received. */
  on(event: 'message', callback: SkyrimTogetherTypes.MessageCallback): void;

  /** Add listener to when a system message is received. */
  on(event: 'systemmessage', callback: SkyrimTogetherTypes.SystemMessageCallback): void;

  /** Add listener to when the player connects to a server. */
  on(event: 'connect', callback: SkyrimTogetherTypes.ConnectCallback): void;

  /** Add listener to when the player disconnects from a server. */
  on(event: 'disconnect', callback: SkyrimTogetherTypes.DisconnectCallback): void;

  /** Add listener to when the player's name changes. */
  on(event: 'namechange', callback: SkyrimTogetherTypes.NameChangeCallback): void;

  /** Add listener to when the client's version is set. */
  on(event: 'versionset', callback: SkyrimTogetherTypes.VersionSetCallback): void;

  /** Add listener to when the player's press the F3 key */
  on(event: 'debug', callback: SkyrimTogetherTypes.OnDebugCallback): void;

  on(event: 'debugdata', callback: SkyrimTogetherTypes.UpdateDebugCallback): void;

  /** Add listener to when the player's is connected with the launcher */
  on(event: 'userdataset', callback: SkyrimTogetherTypes.UserDataSetCallback): void;

  /** Add listener to when one player connect in server. */
  on(event: 'playerconnected', callback: SkyrimTogetherTypes.PlayerConnectedCallback): void;

  /** Add listener to when one player disconnect in server. */
  on(event: 'playerdisconnected', callback: SkyrimTogetherTypes.PlayerDisconnectedCallback): void;

  on(event: 'healthset', callback: SkyrimTogetherTypes.HealthCallback): void;

  /** Add listener to when one player change level in server. */
  on(event: 'setlevel', callback: SkyrimTogetherTypes.SetLevelCallback): void;

  /** Add listener to when one player change cell in server. */
  on(event: 'setcell', callback: SkyrimTogetherTypes.SetCellCallback): void;

  /** Add listener to when a player is loaded or unloaded in 3D.  */
  on(event: 'setplayer3Dloaded', callback: SkyrimTogetherTypes.SetPlayer3DLoadedCallback): void;

  on(event: 'serverid', callback: SkyrimTogetherTypes.SetServerIdCallback): void;

  on(event: 'protocolmismatch', callback: SkyrimTogetherTypes.ProtocolMismatch): void;

  on(event: 'triggererror', callback: SkyrimTogetherTypes.TriggerError): void;

  /** Remove listener from when the application is first initialized. */
  off(event: 'init', callback?: SkyrimTogetherTypes.InitCallback): void;

  /** Remove listener from when the UI is activated. */
  off(event: 'activate', callback?: SkyrimTogetherTypes.ActivateCallback): void;

  /** Remove listener from when the UI is deactivated. */
  off(event: 'deactivate', callback?: SkyrimTogetherTypes.DeactivateCallback): void;

  /** Remove listener from when the player enters a game. */
  off(event: 'entergame', callback?: SkyrimTogetherTypes.EnterGameCallback): void;

  /** Remove listener from when the player exits a game. */
  off(event: 'exitgame', callback?: SkyrimTogetherTypes.ExitGameCallback): void;

  /** Add listener to when the player open/close a game menu. */
  off(event: 'openingmenu', callback?: SkyrimTogetherTypes.OpeningMenuCallback): void;

  /** Remove listener from when a player message is received. */
  off(event: 'message', callback?: SkyrimTogetherTypes.MessageCallback): void;

  /** Remove listener from when a system message is received. */
  off(event: 'systemmessage', callback?: SkyrimTogetherTypes.SystemMessageCallback): void;

  /** Remove listener from when the player connects to a server. */
  off(event: 'connect', callback?: SkyrimTogetherTypes.ConnectCallback): void;

  /** Remove listener from when the player disconnects from a server. */
  off(event: 'disconnect', callback?: SkyrimTogetherTypes.DisconnectCallback): void;

  /** Remove listener from when the player's name changes. */
  off(event: 'namechange', callback?: SkyrimTogetherTypes.NameChangeCallback): void;

  /** Remove listener from when the client's version is set. */
  off(event: 'versionset', callback?: SkyrimTogetherTypes.VersionSetCallback): void;

  /** Remove listener to when the player's press the F3 key */
  off(event: 'debug', callback?: SkyrimTogetherTypes.OnDebugCallback): void;

  off(event: 'debugdata', callback?: SkyrimTogetherTypes.UpdateDebugCallback): void;

  /** Add listener to when one player connect in server. */
  off(event: 'playerconnected', callback?: SkyrimTogetherTypes.PlayerConnectedCallback): void;

  /** Add listener to when one player disconnect in server. */
  off(event: 'playerdisconnected', callback?: SkyrimTogetherTypes.PlayerDisconnectedCallback): void;

  off(event: 'userdataset', callback?: SkyrimTogetherTypes.UserDataSetCallback): void;

  off(event: 'healthset', callback?: SkyrimTogetherTypes.HealthCallback): void;

  off(event: 'setlevel', callback?: SkyrimTogetherTypes.SetLevelCallback): void;

  off(event: 'setcell', callback?: SkyrimTogetherTypes.SetCellCallback): void;

  /** Add listener to when a player is loaded or unloaded in 3D.  */
  off(event: 'setplayer3Dloaded', callback?: SkyrimTogetherTypes.SetPlayer3DLoadedCallback): void;

  off(event: 'serverid', callback?: SkyrimTogetherTypes.SetServerIdCallback): void;

  off(event: 'protocolmismatch', callback?: SkyrimTogetherTypes.ProtocolMismatch): void;

  off(event: 'triggererror', callback?: SkyrimTogetherTypes.TriggerError): void;

  /**
   * Connect to server at given address and port.
   *
   * @param host IP address or hostname.
   * @param port Port.
   */
  connect(host: string, port: number, token: string): void;

  /**
   * Disconnect from server or cancel connection.
   */
  disconnect(): void;

  /**
   * Send message to server.
   *
   * @param message Message to send.
   */
  sendMessage(message: string): void;

  /**
   * Deactivate UI and release control.
   */
  deactivate(): void;

  /**
   * Reconnect the client.
   */
  reconnect(): void;
}
