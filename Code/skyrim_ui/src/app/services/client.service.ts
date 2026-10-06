import { Injectable, NgZone, OnDestroy } from '@angular/core';
import { TranslocoService } from '@ngneat/transloco';
import { AsyncSubject, BehaviorSubject, ReplaySubject, Subject } from 'rxjs';
import { environment } from '../../environments/environment';
import { Debug } from '../models/debug';
import { PartyInfo } from '../models/party-info';
import { Player } from '../models/player';
import { ChatService } from './chat.service';
import { ErrorEvents, ErrorService } from './error.service';
import { LoadingService } from './loading.service';

export interface TradeItemPayload {
  modId: number;
  baseId: number;
  count: number;
  isQuestItem: boolean;
  isEquipped?: boolean;
  name: string;
  inventoryIndex?: number;
  offeredCount?: number;
  isGold?: boolean;
  details?: string[];
}

export interface TradeStatePayload {
  active: boolean;
  partnerId: number;
  initiatedBySelf: boolean;
  selfReady: boolean;
  partnerReady: boolean;
  selfItems: TradeItemPayload[];
  partnerItems: TradeItemPayload[];
  inventory: TradeItemPayload[];
  countdownMs: number;
  countdownTotalMs: number;
}

export interface TradeInvitePayload {
  inviterId: number;
  expiryTick: number;
}

export interface TradeCancellationPayload {
  partnerId: number;
  reason: number;
  wasInitiator: boolean;
}

/** Client game service. */
@Injectable({
  providedIn: 'root',
})
export class ClientService implements OnDestroy {
  /** Initialization done. */
  public initDone = new AsyncSubject<undefined>();

  /** Activation state change. */
  public activationStateChange = new BehaviorSubject(false);

  /** InGame state change. */
  public inGameStateChange = new BehaviorSubject(!environment.game);

  /** Opening/close menu change. */
  public openingMenuChange = new BehaviorSubject(false);

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

  /** Connect party info change. */
  public partyLeftChange = new Subject<void>();

  /** Connect party invite received. */
  public partyInviteReceivedChange = new Subject<number>();

  /** Local quest started or updated while not in a party. */
  public questUpdatedChange = new Subject<void>();

  /** Trade invite received. */
  public tradeInviteChange = new Subject<TradeInvitePayload>();

  /** Trade invite expired. */
  public tradeInviteExpiredChange = new Subject<number>();

  /** Active trade state change. */
  public tradeStateChange = new BehaviorSubject<TradeStatePayload | undefined>(
    undefined,
  );

  /** Trade cancellation notifications. */
  public tradeCancelledChange = new Subject<TradeCancellationPayload>();

  /** Trade completion notifications. */
  public tradeCompletedChange = new Subject<number>();

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
  public triggerError = new ReplaySubject<ErrorEvents>(1);

  /** Used purely for debugging. */
  public debugChange = new Subject<void>();

  // The below emitters are used in the mocking service

  /** Used for when a party leader changed. */
  public partyLaunchedChange = new Subject<void>();

  /** Used for when a party invite is sent. */
  public partyInviteChange = new Subject<number>();

  /** Used for when a party invite was accepted by the local player. */
  public partyJoinedChange = new Subject<number>();

  /** Used for when a party member was kicked. */
  public memberKickedChange = new Subject<number>();

  /** Used for when a party leader changed. */
  public partyLeaderChange = new Subject<number>();

  public localPlayerId = undefined;

  private _host: string;

  private _port: number;

  private _password: string;

  private _remainingReconnectionAttempt = environment.nbReconnectionAttempts;

  /**
   * Instantiate.
   */
  public constructor(
    private readonly zone: NgZone,
    private readonly errorService: ErrorService,
    private readonly loadingService: LoadingService,
    private readonly translocoService: TranslocoService,
    private readonly chatService: ChatService,
  ) {
    skyrimtogether.on('init', this.onInit.bind(this));
    skyrimtogether.on('activate', this.onActivate.bind(this));
    skyrimtogether.on('deactivate', this.onDeactivate.bind(this));
    skyrimtogether.on('enterGame', this.onEnterGame.bind(this));
    skyrimtogether.on('exitGame', this.onExitGame.bind(this));
    skyrimtogether.on('openingMenu', this.onOpeningMenu.bind(this));
    skyrimtogether.on('connect', this.onConnect.bind(this));
    skyrimtogether.on('disconnect', this.onDisconnect.bind(this));
    skyrimtogether.on('setName', this.onSetName.bind(this)); //not wanted, we dont sync name changes
    skyrimtogether.on('setVersion', this.onSetVersion.bind(this));
    skyrimtogether.on('debug', this.onDebug.bind(this)); //not needed anymore
    skyrimtogether.on('debugData', this.onUpdateDebug.bind(this));
    skyrimtogether.on('playerConnected', this.onPlayerConnected.bind(this));
    skyrimtogether.on(
      'playerDisconnected',
      this.onPlayerDisconnected.bind(this),
    );
    skyrimtogether.on('setHealth', this.onSetHealth.bind(this));
    skyrimtogether.on('setLevel', this.onSetLevel.bind(this));
    skyrimtogether.on('setCell', this.onSetCell.bind(this));
    skyrimtogether.on('setPlayer3dLoaded', this.onSetPlayer3dLoaded.bind(this));
    skyrimtogether.on(
      'setPlayer3dUnloaded',
      this.onSetPlayer3dUnloaded.bind(this),
    );
    skyrimtogether.on('setLocalPlayerId', this.onSetLocalPlayerId.bind(this));
    skyrimtogether.on('protocolMismatch', this.onProtocolMismatch.bind(this));
    skyrimtogether.on('triggerError', this.onTriggerError.bind(this));
    skyrimtogether.on('dummyData', this.onDummyData.bind(this));
    skyrimtogether.on('partyInfo', this.onPartyInfo.bind(this));
    skyrimtogether.on('partyCreated', this.onPartyCreated.bind(this));
    skyrimtogether.on('partyLeft', this.onPartyLeft.bind(this));
    skyrimtogether.on(
      'partyInviteReceived',
      this.onPartyInviteReceived.bind(this),
    );
    skyrimtogether.on(
      'tradeInviteReceived',
      this.onTradeInviteReceived.bind(this),
    );
    skyrimtogether.on(
      'tradeInviteExpired',
      this.onTradeInviteExpired.bind(this),
    );
    skyrimtogether.on('tradeStateUpdated', this.onTradeStateUpdated.bind(this));
    skyrimtogether.on('tradeCancelled', this.onTradeCancelled.bind(this));
    skyrimtogether.on('tradeCompleted', this.onTradeCompleted.bind(this));
    skyrimtogether.on('questUpdated', this.onQuestUpdated.bind(this));
  }

  /**
   * Dispose.
   */
  public ngOnDestroy(): void {
    skyrimtogether.off('init');
    skyrimtogether.off('activate');
    skyrimtogether.off('deactivate');
    skyrimtogether.off('enterGame');
    skyrimtogether.off('exitGame');
    skyrimtogether.off('openingMenu');
    skyrimtogether.off('connect');
    skyrimtogether.off('disconnect');
    skyrimtogether.off('setName');
    skyrimtogether.off('setVersion');
    skyrimtogether.off('debug');
    skyrimtogether.off('debugData');
    skyrimtogether.off('playerConnected');
    skyrimtogether.off('playerDisconnected');
    skyrimtogether.off('setHealth');
    skyrimtogether.off('setLevel');
    skyrimtogether.off('setCell');
    skyrimtogether.off('setPlayer3dLoaded');
    skyrimtogether.off('setPlayer3dUnloaded');
    skyrimtogether.off('setLocalPlayerId');
    skyrimtogether.off('protocolMismatch');
    skyrimtogether.off('triggerError');
    skyrimtogether.off('dummyData');
    skyrimtogether.off('partyInfo');
    skyrimtogether.off('partyCreated');
    skyrimtogether.off('partyLeft');
    skyrimtogether.off('partyInviteReceived');
    skyrimtogether.off('tradeInviteReceived');
    skyrimtogether.off('tradeInviteExpired');
    skyrimtogether.off('tradeStateUpdated');
    skyrimtogether.off('tradeCancelled');
    skyrimtogether.off('tradeCompleted');
    skyrimtogether.off('questUpdated');
  }

  /**
   * Connect to a server at the given address and port.
   *
   * @param host IP address or hostname.
   * @param port Port.
   * @param password Password or admin password
   */
  public connect(host: string, port: number, password = ''): void {
    skyrimtogether.connect(host, port, password);
    this.isConnectionInProgressChange.next(true);
    this._host = host;
    this._port = port;
    this._password = password;
  }

  /**
   * Disconnect from the server or cancel connection.
   */
  public disconnect(): void {
    skyrimtogether.disconnect();
    this._remainingReconnectionAttempt = 0;
  }

  /**
   * Reveal players in the immediate area (lighting them up with a visual effect).
   */
  public revealPlayers(): void {
    skyrimtogether.revealPlayers();
  }

  /**
   * Launch a party.
   */
  public launchParty(): void {
    skyrimtogether.launchParty();
  }

  /**
   * Create a party invite.
   */
  public createPartyInvite(playerId: number): void {
    skyrimtogether.createPartyInvite(playerId);
  }

  /**
   * Accept a party invite.
   */
  public acceptPartyInvite(inviterId: number): void {
    skyrimtogether.acceptPartyInvite(inviterId);
  }

  /**
   * As a party leader, kick a player from the party.
   */
  public kickPartyMember(playerId: number): void {
    skyrimtogether.kickPartyMember(playerId);
  }

  /**
   * Leave a party.
   */
  public leaveParty(): void {
    skyrimtogether.leaveParty();
  }

  /**
   * As a party leader, make someone else the leader.
   */
  public changePartyLeader(playerId: number): void {
    skyrimtogether.changePartyLeader(playerId);
  }

  /**
   * Open the play guide on the wiki in the default browser.
   */
  /** Send a trade invite to another player. */
  public sendTradeInvite(playerId: number): void {
    skyrimtogether.sendTradeInvite(playerId);
  }

  /** Respond to an incoming trade invite. */
  public respondTradeInvite(playerId: number, accept: boolean): void {
    skyrimtogether.respondTradeInvite(playerId, accept);
  }

  /** Cancel the current trade session or outstanding invite. */
  public cancelTrade(): void {
    skyrimtogether.cancelTrade();
  }

  /** Toggle readiness state in the current trade session. */
  public setTradeReady(ready: boolean): void {
    skyrimtogether.setTradeReady(ready);
  }

  /** Update the offered items for the current trade session. */
  public updateTradeOffer(entries: { index: number; count: number }[]): void {
    skyrimtogether.updateTradeOffer(entries);
  }

  public openPlayGuide(): void {
    skyrimtogether.openPlayGuide();
  }

  /**
   * Deactivate UI and release control.
   */
  public deactivate(): void {
    skyrimtogether.deactivate();
  }

  /**
   * Reconnect
   */
  public reconnect(): void {
    skyrimtogether.reconnect();
    this._remainingReconnectionAttempt = 0;
  }

  public teleportToPlayer(playerId: number): void {
    skyrimtogether.teleportToPlayer(playerId);
  }

  /**
   * Called when the UI is first initialized.
   */
  // TODO: is this still used?
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
   * Called when a connection is made.
   */
  private onConnect(): void {
    this.zone.run(async () => {
      this._remainingReconnectionAttempt = environment.nbReconnectionAttempts;
      this.isConnectionInProgressChange.next(false);
      this.connectionStateChange.next(true);

      this.chatService.pushSystemMessage('SERVICE.CLIENT.CONNECTED');
    });
  }

  /**
   * Called when a connection is terminated.
   */
  private onDisconnect(isError: boolean): void {
    void this.zone.run(async () => {
      this.localPlayerId = undefined;
      this.connectionStateChange.next(false);
      this.isConnectionInProgressChange.next(false);

      if (isError && this._remainingReconnectionAttempt > 0) {
        this._remainingReconnectionAttempt--;
        this.chatService.pushSystemMessage('SERVICE.CLIENT.CONNECTION_LOST');
        this.connect(this._host, this._port, this._password);
      } else {
        this.chatService.pushSystemMessage('SERVICE.CLIENT.DISCONNECTED');
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
    version = environment.overwriteVersion || version;

    this.zone.run(() => {
      this.versionSet.next(version);
    });
  }

  getVersion(): string {
    return this.versionSet.value;
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
    receivedBandwidth: number,
  ): void {
    this.zone.run(() => {
      this.debugDataChange.next(
        new Debug(
          numPacketsSent,
          numPacketsReceived,
          RTT,
          packetLoss,
          sentBandwidth,
          receivedBandwidth,
        ),
      );
    });
  }

  private onPlayerConnected(
    playerId: number,
    username: string,
    level: number,
    cellName: string,
  ) {
    if (environment.game) {
      console.log(
        `%conPlayerConnected`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.playerConnectedChange.next(
        new Player({
          name: username,
          id: playerId,
          connected: true,
          level: level,
          cellName: cellName,
        }),
      );
    });
  }

  private onPlayerDisconnected(playerId: number, username: string) {
    if (environment.game) {
      console.log(
        `%conPlayerDisconnected`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.playerDisconnectedChange.next(
        new Player({
          name: username,
          id: playerId,
          connected: false,
        }),
      );
    });
  }

  private onSetHealth(playerId: number, health: number) {
    this.zone.run(() => {
      this.healthChange.next(new Player({ id: playerId, health: health }));
    });
  }

  private onSetLevel(playerId: number, level: number) {
    if (environment.game) {
      console.log(
        `%conSetLevel`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.levelChange.next(new Player({ id: playerId, level: level }));
    });
  }

  private onSetCell(playerId: number, cellName: string) {
    if (environment.game) {
      console.log(
        `%conSetCell`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.cellChange.next(new Player({ id: playerId, cellName: cellName }));
    });
  }

  private onSetPlayer3dLoaded(playerId: number, health: number) {
    if (environment.game) {
      console.log(
        `%conSetPlayer3dLoaded`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.isLoadedChange.next(
        new Player({ id: playerId, isLoaded: true, health: health }),
      );
    });
  }

  private onSetPlayer3dUnloaded(playerId: number) {
    if (environment.game) {
      console.log(
        `%conSetPlayer3dUnloaded`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.isLoadedChange.next(new Player({ id: playerId, isLoaded: false }));
    });
  }

  private onSetLocalPlayerId(playerId: number) {
    if (environment.game) {
      console.log(
        `%conSetLocalPlayerId`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.localPlayerId = playerId;
    });
  }

  private onProtocolMismatch() {
    this.zone.run(() => {
      this.protocolMismatchChange.next(true);
    });
  }

  private onTriggerError(rawError: string) {
    this.zone.run(() => {
      const error = JSON.parse(rawError) as ErrorEvents;
      this.triggerError.next(error);
      void this.errorService.setError(error);
    });
  }

  private onDummyData(data: Array<number>) {
    this.zone.run(() => {
      this.debugChange.next();
      /*
      for (const numb of data) {
        console.log(numb);
      }
      console.log(data);
      */
    });
  }

  public onPartyInfo(playerIds: Array<number>, leaderId: number) {
    if (environment.game) {
      console.log(
        `%conPartyInfo`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.partyInfoChange.next(
        new PartyInfo({
          playerIds: playerIds,
          leaderId: leaderId,
        }),
      );
    });
  }

  private onPartyCreated() {
    if (environment.game) {
      console.log(
        `%conPartyCreated`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.loadingService.setLoading(false);
      this.partyInfoChange.next(
        new PartyInfo({
          playerIds: [this.localPlayerId],
          leaderId: this.localPlayerId,
        }),
      );
    });
  }

  private onPartyLeft() {
    if (environment.game) {
      console.log(
        `%conPartyLeft`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.partyLeftChange.next();
    });
  }

  private onPartyInviteReceived(inviterId: number) {
    if (environment.game) {
      console.log(
        `%conPartyInviteReceived`,
        'background: #009688; color: #fff; padding: 3px; font-size: 9px;',
        ...Array.from(arguments).map(v => JSON.stringify(v)),
      );
    }
    this.zone.run(() => {
      this.partyInviteReceivedChange.next(inviterId);
    });
  }

  private onTradeInviteReceived(inviterId: number, expiryTick: number) {
    if (environment.game) {
      console.log(
        `%conTradeInviteReceived`,
        'background: #FFC107; color: #000; padding: 3px; font-size: 9px;',
        inviterId,
        expiryTick,
      );
    }
    this.zone.run(() => {
      this.tradeInviteChange.next({ inviterId, expiryTick });
    });
  }

  private onTradeInviteExpired(inviterId: number) {
    if (environment.game) {
      console.log(
        `%conTradeInviteExpired`,
        'background: #FFC107; color: #000; padding: 3px; font-size: 9px;',
        inviterId,
      );
    }
    this.zone.run(() => {
      this.tradeInviteExpiredChange.next(inviterId);
    });
  }

  private onTradeStateUpdated(
    active: boolean,
    partnerId: number,
    initiatedBySelf: boolean,
    selfReady: boolean,
    partnerReady: boolean,
    selfItems: unknown[],
    partnerItems: unknown[],
    inventory: unknown[],
    countdownMs: number,
    countdownTotalMs: number,
  ) {
    if (environment.game) {
      console.log(
        `%conTradeStateUpdated`,
        'background: #FFC107; color: #000; padding: 3px; font-size: 9px;',
        active,
        partnerId,
        initiatedBySelf,
        selfReady,
        partnerReady,
        selfItems,
        partnerItems,
        inventory,
        countdownMs,
        countdownTotalMs,
      );
    }

    const normalized: TradeStatePayload = {
      active: Boolean(active),
      partnerId: Number.isFinite(partnerId) ? partnerId : 0,
      initiatedBySelf: Boolean(initiatedBySelf),
      selfReady: Boolean(selfReady),
      partnerReady: Boolean(partnerReady),
      selfItems: Array.isArray(selfItems)
        ? selfItems.map(item => this.normalizeTradeItem(item))
        : [],
      partnerItems: Array.isArray(partnerItems)
        ? partnerItems.map(item => this.normalizeTradeItem(item))
        : [],
      inventory: Array.isArray(inventory)
        ? inventory.map(item => this.normalizeTradeItem(item))
        : [],
      countdownMs: Number.isFinite(countdownMs) ? Number(countdownMs) : 0,
      countdownTotalMs: Number.isFinite(countdownTotalMs)
        ? Number(countdownTotalMs)
        : 0,
    };

    this.zone.run(() => {
      this.tradeStateChange.next(normalized);
    });
  }

  private onTradeCancelled(
    partnerId: number,
    reason: number,
    wasInitiator: boolean,
  ) {
    if (environment.game) {
      console.log(
        `%conTradeCancelled`,
        'background: #FFC107; color: #000; padding: 3px; font-size: 9px;',
        partnerId,
        reason,
        wasInitiator,
      );
    }
    this.zone.run(() => {
      this.tradeCancelledChange.next({ partnerId, reason, wasInitiator });
    });
  }

  private onTradeCompleted(partnerId: number) {
    if (environment.game) {
      console.log(
        `%conTradeCompleted`,
        'background: #FFC107; color: #000; padding: 3px; font-size: 9px;',
        partnerId,
      );
    }
    this.zone.run(() => {
      this.tradeCompletedChange.next(partnerId);
    });
  }

  private normalizeTradeItem(payload: any): TradeItemPayload {
    const modId = Number.isFinite(payload?.modId) ? Number(payload.modId) : 0;
    const baseId = Number.isFinite(payload?.baseId)
      ? Number(payload.baseId)
      : 0;
    const count = Math.max(
      0,
      Number.isFinite(payload?.count) ? Number(payload.count) : 0,
    );
    const isQuestItem = Boolean(payload?.isQuestItem);
    const fallbackName = `0x${modId.toString(16).padStart(8, '0')}:0x${baseId
      .toString(16)
      .padStart(8, '0')}`;
    const name =
      typeof payload?.name === 'string' && payload.name.length > 0
        ? payload.name
        : fallbackName;

    const item: TradeItemPayload = {
      modId,
      baseId,
      count,
      isQuestItem,
      isEquipped: Boolean(payload?.isEquipped),
      name,
    };

    if (payload?.inventoryIndex !== undefined) {
      item.inventoryIndex = Number(payload.inventoryIndex);
    }
    if (payload?.offeredCount !== undefined) {
      item.offeredCount = Math.max(
        0,
        Number.isFinite(payload.offeredCount)
          ? Number(payload.offeredCount)
          : 0,
      );
    }
    if (payload?.isGold !== undefined) {
      item.isGold = Boolean(payload.isGold);
    }
    if (Array.isArray(payload?.details)) {
      item.details = payload.details.map((entry: any) => String(entry));
    }
    if (item.isGold === undefined) {
      item.isGold = modId === 0 && baseId === 0x0000000f;
    }

    return item;
  }

  private onQuestUpdated() {
    this.zone.run(() => {
      this.questUpdatedChange.next();
    });
  }
}
