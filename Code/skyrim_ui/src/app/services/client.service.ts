import { Injectable, NgZone, OnDestroy } from '@angular/core';

import { BehaviorSubject, ReplaySubject, AsyncSubject, Subject } from 'rxjs';

import { environment } from '../../environments/environment';
import { Debug } from '../models/debug';
import { Player } from '../models/player';
import { PartyInfo } from '../models/party-info';
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

  /** Connect party info change. */
  public partyInfoChange = new Subject<PartyInfo>();

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

  public localServerId = -1;

  private _host: string;

  private _port: number;

  private _token: string;

  private _remainingReconnectionAttempt = environment.nbReconnectionAttempts;

  /**
   * Instantiate.
   *
   * @param zone Angular Zone.
   */
  public constructor(private zone: NgZone, private errorService: ErrorService) {
    if (environment.game) {
      skyrimtogether.on('init', this.onInit.bind(this));
      skyrimtogether.on('activate', this.onActivate.bind(this));
      skyrimtogether.on('deactivate', this.onDeactivate.bind(this));
      skyrimtogether.on('enterGame', this.onEnterGame.bind(this));
      skyrimtogether.on('exitGame', this.onExitGame.bind(this));
      skyrimtogether.on('openingMenu', this.onOpeningMenu.bind(this));
      skyrimtogether.on('message', this.onMessage.bind(this));
      skyrimtogether.on('systemMessage', this.onSystemMessage.bind(this));
      skyrimtogether.on('whisperMessage', this.onWhisperMessage.bind(this));
      skyrimtogether.on('connect', this.onConnect.bind(this));
      skyrimtogether.on('disconnect', this.onDisconnect.bind(this));
      skyrimtogether.on('setName', this.onSetName.bind(this)); //not wanted, we dont sync name changes
      skyrimtogether.on('setVersion', this.onSetVersion.bind(this));
      skyrimtogether.on('debug', this.onDebug.bind(this)); //not needed anymore
      skyrimtogether.on('debugData', this.onUpdateDebug.bind(this));
      skyrimtogether.on('playerConnected', this.onPlayerConnected.bind(this));
      skyrimtogether.on('playerDisconnected', this.onPlayerDisconnected.bind(this));
      skyrimtogether.on('setHealth', this.onSetHealth.bind(this));
      skyrimtogether.on('setLevel', this.onSetLevel.bind(this));
      skyrimtogether.on('setCell', this.onSetCell.bind(this));
      skyrimtogether.on('setPlayer3dLoaded', this.onSetPlayer3dLoaded.bind(this));
      skyrimtogether.on('setPlayer3dUnloaded', this.onSetPlayer3dUnloaded.bind(this));
      skyrimtogether.on('setServerId', this.onSetServerId.bind(this));
      skyrimtogether.on('protocolMismatch', this.onProtocolMismatch.bind(this));
      skyrimtogether.on('triggerError', this.onTriggerError.bind(this));
      skyrimtogether.on('dummyData', this.onDummyData.bind(this));
      skyrimtogether.on('partyInfo', this.onPartyInfo.bind(this));
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
      skyrimtogether.off('enterGame');
      skyrimtogether.off('exitGame');
      skyrimtogether.off('openingMenu');
      skyrimtogether.off('message');
      skyrimtogether.off('systemMessage');
      skyrimtogether.off('whisperMessage');
      skyrimtogether.off('connect');
      skyrimtogether.off('disconnect');
      skyrimtogether.off('setName');
      skyrimtogether.off('setVersion');
      skyrimtogether.off('debug');
      skyrimtogether.off('debugData');
      skyrimtogether.off('userDataSet');
      skyrimtogether.off('playerConnected');
      skyrimtogether.off('playerDisconnected');
      skyrimtogether.off('setHealth');
      skyrimtogether.off('setLevel');
      skyrimtogether.off('setCell');
      skyrimtogether.off('setPlayer3dLoaded');
      skyrimtogether.off('setPlayer3dUnloaded');
      skyrimtogether.off('setServerId');
      skyrimtogether.off('protocolMismatch');
      skyrimtogether.off('triggerError');
      skyrimtogether.off('dummyData');
      skyrimtogether.off('partyInfo');
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
   * Launch a party.
   */
   public launchParty(): void {
    if (environment.game) {
      skyrimtogether.launchParty();
    }
    else {
      this.messageReception.next({ content: "You're not ingame, duh" });
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

  public teleportToPlayer(playerId: number): void {
    if (environment.game) {
      skyrimtogether.teleportToPlayer(playerId);
    }
    else {
      this.messageReception.next({ content: "Youre not ingame, duh" });
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
      console.log("ENTER GAME: " + this.inGameStateChange.value);
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
  private onSetName(name: string): void {
    this.zone.run(() => {
      this.nameChange.next(name);
    });
  }

  /**
   * Called when the client's version is set.
   *
   * @param version Game's version.
   */
  private onSetVersion(version: string): void {
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

  private onSetHealth(serverId: number, health: number) {
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

  private onSetPlayer3dLoaded(serverId: number, health: number) {
    this.zone.run(() => {
      this.isLoadedChange.next(new Player({serverId: serverId, isLoaded: true, health: health}));
    })
  }

  private onSetPlayer3dUnloaded(serverId: number) {
    this.zone.run(() => {
      this.isLoadedChange.next(new Player({serverId: serverId, isLoaded: false}));
    })
  }

  private onSetServerId(serverId: number) {
    this.zone.run(() => {
      this.localServerId = serverId;
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

  private onDummyData(data: Array<number>) {
    this.zone.run(() => {
      for (const numb of data) {
        console.log(numb);
      }
      console.log(data);
    })
  }

  private onPartyInfo(serverIds: Array<number>, leaderId: number) {
    this.zone.run(() => {
      this.partyInfoChange.next(new PartyInfo(
        {
          serverIds: serverIds,
          leaderId: leaderId
        }
      ));
    })
  }
}
