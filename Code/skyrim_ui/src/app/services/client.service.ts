import { Injectable, NgZone, OnDestroy } from '@angular/core';

import { BehaviorSubject, ReplaySubject, AsyncSubject, Subject } from 'rxjs';

import { environment } from '../../environments/environment';
import { Debug } from '../models/debug';
import { UserService } from './user.service';
import { Player } from '../models/player';
import { take } from 'rxjs/operators';
import { ErrorService } from './error.service';

/** Message. */
export interface Message {
  /** Player name. Unset if it's a system message. */
  name?: string;

  /** Message content. */
  content: string;

  whisper?: boolean;
}

/** Client game service. */
@Injectable()
export class ClientService implements OnDestroy {
  /** Initialization done. */
  public initDone = new AsyncSubject<undefined>();

  /** Activation state change. */
  public activationStateChange = new BehaviorSubject(false);

  /** InGame state change. */
  public inGameStateChange = new BehaviorSubject(!environment.game);

  /** Opening/close menu change. */
  public openingMenuChange = new BehaviorSubject(false);

  /** Message reception. */
  public messageReception = new ReplaySubject<Message>();

  /** Connection state change. */
  public connectionStateChange = new BehaviorSubject(false);

  public isConnectionInProgressChange = new BehaviorSubject(false);

  /** Player name change. */
  public nameChange = new BehaviorSubject(environment.game ? '' : 'test');

  /** Client version setting. */
  public versionSet = new BehaviorSubject(environment.game ? '' : 'browser');

  /** Debug state change. */
  public debugStateChange = new Subject<boolean>();

  /** Debug data change. */
  public debugDataChange = new BehaviorSubject(new Debug());

  /** Connect player to server change. */
  public playerConnectedChange = new Subject<Player>();

  /** Disconnect player to server change. */
  public playerDisconnectedChange = new Subject<Player>();

  /** Player health change. */
  public healthChange = new Subject<Player>();

  /** Player level change. */
  public levelChange = new Subject<Player>();

  /** Player cell change. */
  public cellChange = new Subject<Player>();

  /** Player isLoaded change. */
  public isLoadedChange = new Subject<Player>();

  public protocolMismatchChange = new BehaviorSubject(false);

  /** Receive error from core */
  public triggerError = new BehaviorSubject('');

  private _host: string;

  private _port: number;

  private _token: string;

  private _remainingReconnectionAttempt = environment.nbReconnectionAttempts;

  /**
   * Instantiate.
   *
   * @param zone Angular Zone.
   */
  public constructor(private zone: NgZone, private userService: UserService, private errorService: ErrorService) {
    if (environment.game) {
      skyrimtogether.on('init', this.onInit.bind(this));
      skyrimtogether.on('activate', this.onActivate.bind(this));
      skyrimtogether.on('deactivate', this.onDeactivate.bind(this));
      skyrimtogether.on('entergame', this.onEnterGame.bind(this));
      skyrimtogether.on('exitgame', this.onExitGame.bind(this));
      skyrimtogether.on('openingmenu', this.onOpeningMenu.bind(this));
      skyrimtogether.on('message', this.onMessage.bind(this));
      skyrimtogether.on('systemmessage', this.onSystemMessage.bind(this));
      skyrimtogether.on('whispermessage', this.onWhisperMessage.bind(this));
      skyrimtogether.on('connect', this.onConnect.bind(this));
      skyrimtogether.on('disconnect', this.onDisconnect.bind(this));
      skyrimtogether.on('namechange', this.onNameChange.bind(this));
      skyrimtogether.on('versionset', this.onVersionSet.bind(this));
      skyrimtogether.on('debug', this.onDebug.bind(this));
      skyrimtogether.on('debugdata', this.onUpdateDebug.bind(this));
      skyrimtogether.on('userdataset', this.onUserDataSet.bind(this));
      skyrimtogether.on('playerconnected', this.onPlayerConnected.bind(this));
      skyrimtogether.on('playerdisconnected', this.onPlayerDisconnected.bind(this));
      skyrimtogether.on('healthset', this.onHealthSet.bind(this));
      skyrimtogether.on('setlevel', this.onSetLevel.bind(this));
      skyrimtogether.on('setcell', this.onSetCell.bind(this));
      skyrimtogether.on('setplayer3Dloaded', this.onSetPlayer3DLoaded.bind(this));
      skyrimtogether.on('serverid', this.onServerId.bind(this));
      skyrimtogether.on('protocolmismatch', this.onProtocolMismatch.bind(this));
      skyrimtogether.on('triggererror', this.onTriggerError.bind(this));
    }
  }

  /**
   * Dispose.
   */
  public ngOnDestroy(): void {
    if (environment.game) {
      skyrimtogether.off('init');
      skyrimtogether.off('activate');
      skyrimtogether.off('deactivate');
      skyrimtogether.off('entergame');
      skyrimtogether.off('exitgame');
      skyrimtogether.off('openingmenu');
      skyrimtogether.off('message');
      skyrimtogether.off('systemmessage');
      skyrimtogether.off('whispermessage');
      skyrimtogether.off('connect');
      skyrimtogether.off('disconnect');
      skyrimtogether.off('namechange');
      skyrimtogether.off('versionset');
      skyrimtogether.off('debug');
      skyrimtogether.off('debugdata');
      skyrimtogether.off('userdataset');
      skyrimtogether.off('playerconnected');
      skyrimtogether.off('playerdisconnected');
      skyrimtogether.off('healthset');
      skyrimtogether.off('setlevel');
      skyrimtogether.off('setcell');
      skyrimtogether.off('setplayer3Dloaded');
      skyrimtogether.off('serverid');
      skyrimtogether.off('protocolmismatch');
      skyrimtogether.off('triggererror');
    }
  }

  /**
   * Connect to a server at the given address and port.
   *
   * @param host IP address or hostname.
   * @param port Port.
   */
  public connect(host: string, port: number, token = ""): void {
    if (environment.game) {
      skyrimtogether.connect(host, port, token);
      this.isConnectionInProgressChange.next(true);
      this._host = host;
      this._port = port;
      this._token = token;
    }
    else {
      this.connectionStateChange.next(true);
      this.isConnectionInProgressChange.next(false);
    }
  }

  /**
   * Disconnect from the server or cancel connection.
   */
  public disconnect(): void {
    if (environment.game) {
      skyrimtogether.disconnect();
      this._remainingReconnectionAttempt = 0;
    }
    else {
      this.connectionStateChange.next(false);
    }
  }

  /**
   * Broadcast message to server.
   *
   * @param message Message to send.
   */
  public sendMessage(message: string): void {
    if (environment.game) {
      skyrimtogether.sendMessage(message);
    }
    else {
      this.messageReception.next({ name: this.nameChange.value, content: message });
    }
  }

  /**
   * Deactivate UI and release control.
   */
  public deactivate(): void {
    if (environment.game) {
      skyrimtogether.deactivate();
    }
    else {
      this.activationStateChange.next(this.activationStateChange.value);
    }
  }

  /**
   * Reconnect
   */
  public reconnect(): void {
    if (environment.game) {
      skyrimtogether.reconnect();
      this._remainingReconnectionAttempt = 0;
    }
  }

  /**
   * Called when the UI is first initialized.
   */
  private onInit(): void {
    this.zone.run(() => {
      this.initDone.next(undefined);
      this.initDone.complete();
    });
  }

  /**
   * Called when the UI is activated.
   */
  private onActivate(): void {
    this.zone.run(() => {
      this.activationStateChange.next(true);
    });
  }

  /**
   * Called when the UI is deactivated.
   */
  private onDeactivate(): void {
    this.zone.run(() => {
      this.activationStateChange.next(false);
    });
  }

  /**
   * Called when the player enters a game.
   */
  private onEnterGame(): void {
    this.zone.run(() => {
      this.inGameStateChange.next(true);
    });
  }

  /**
   * Called when the player exits a game.
   */
  private onExitGame(): void {
    this.zone.run(() => {
      this.inGameStateChange.next(false);
    });
  }

  /**
   * Called when the player open/close a game menu
   * @param openingMenu
   */
  private onOpeningMenu(openingMenu: boolean): void {
    this.zone.run(() => {
      this.openingMenuChange.next(openingMenu);
    });
  }

  /**
   * Called when a message is received.
   *
   * @param name Sender's name.
   * @param message Message content.
   */
  private onMessage(name: string, message: string): void {
    this.zone.run(() => {
      this.messageReception.next({ name, content: message });
    });
  }

  /**
   * Called when a system message is received.
   *
   * @param message Message content.
   */
  private onSystemMessage(message: string): void {
    this.zone.run(() => {
      this.messageReception.next({ content: message });
    });
  }

  /**
   * Called when a whisper message is received.
   *
   * @param name Sender's name.
   * @param message Message content.
   */
   private onWhisperMessage(name: string, message: string): void {
    let whisper = true;
    this.zone.run(() => {
      this.messageReception.next({ name, content: message, whisper});
    });
  }

  /**
   * Called when a connection is made.
   */
  private onConnect(): void {
    this.zone.run(() => {
      this._remainingReconnectionAttempt = environment.nbReconnectionAttempts;
      this.isConnectionInProgressChange.next(false);
      this.connectionStateChange.next(true);
    });
  }

  /**
   * Called when a connection is terminated.
   */
  private onDisconnect(isError: boolean): void {
    this.zone.run(() => {
      this.connectionStateChange.next(false);
      this.isConnectionInProgressChange.next(false);

      if (isError && this._remainingReconnectionAttempt > 0) {
        this._remainingReconnectionAttempt--;
        this.messageReception.next({ content: `Connection lost, trying to reconnect. ${this._remainingReconnectionAttempt} attempts left.` })
        this.connect(this._host, this._port, this._token);
      }
    });
  }

  /**
   * Called when the player's name changes.
   *
   * @param name Player's name.
   */
  private onNameChange(name: string): void {
    this.zone.run(() => {
      this.nameChange.next(name);
    });
  }

  /**
   * Called when the client's version is set.
   *
   * @param version Game's version.
   */
  private onVersionSet(version: string): void {
    this.zone.run(() => {
      this.versionSet.next(version);
    });
  }

  /**
   * Add listener to when the player's press the F3 key
   */
  private onDebug(isDebug: boolean): void {
    this.zone.run(() => {
      this.debugStateChange.next(isDebug);
    });
  }

  private onUpdateDebug(
    numPacketsSent: number,
    numPacketsReceived: number,
    RTT: number,
    packetLoss: number,
    sentBandwidth: number,
    receivedBandwidth: number
  ): void {
    this.zone.run(() => {
      this.debugDataChange.next(new Debug(
        numPacketsSent, numPacketsReceived, RTT, packetLoss, sentBandwidth,
        receivedBandwidth
      ));
    });
  }

  private onUserDataSet(token: string, username: string) {
    this.userService.login(token, username);
  }

  private onPlayerConnected(serverId: number, username: string, level: number, cellName: string) {
    this.zone.run(() => {
      this.playerConnectedChange.next(new Player(
        {
          name: username,
          serverId: serverId,
          connected: true,
          level: level,
          cellName: cellName
        }
      ));
    });
  }

  private onPlayerDisconnected(serverId: number, username: string) {
    this.zone.run(() => {
      this.playerDisconnectedChange.next(new Player(
        {
          name: username,
          serverId: serverId,
          connected: false
        }
      ));
    });
  }

  private onHealthSet(serverId: number, health: number) {
    this.zone.run(() => {
      this.healthChange.next(new Player({ serverId: serverId, health: health }));
    });
  }

  private onSetLevel(serverId: number, level: number) {
    this.zone.run(() => {
      this.levelChange.next(new Player({ serverId: serverId, level: level }));
    })
  }

  private onSetCell(serverId: number, cellName: string) {
    this.zone.run(() => {
      this.cellChange.next(new Player({ serverId: serverId, cellName: cellName }));
    })
  }

  private onSetPlayer3DLoaded(serverId: number, isLoaded: boolean) {
    this.zone.run(() => {
      this.isLoadedChange.next(new Player({ serverId: serverId, isLoaded: isLoaded }));
    })
  }

  private onServerId(serverId: number) {
    this.zone.run(() => {
      this.userService.player.value.serverId = serverId;
    })
  }

  private onProtocolMismatch() {
    this.zone.run(() => {
      this.protocolMismatchChange.next(true);
    })
  }

  private onTriggerError(error: string) {
    this.zone.run(() => {
      this.triggerError.next(error);
      this.errorService.error(error);
    })
  }
}
