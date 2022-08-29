import { createStore } from '@ngneat/elf';
import { addEntities, deleteEntities, getAllEntities, getEntity, selectAllEntities, updateEntities, withEntities } from '@ngneat/elf-entities';
import { EventEmitter } from 'events';
import { fromEvent } from 'rxjs';
import { MessageTypes } from '../services/chat.service';
import { ErrorEvents } from '../services/error.service';
import { MockPlayer } from './mock-player';


let nextPlayerId = 1;

const playerStore = createStore({ name: 'players' }, withEntities<MockPlayer>());

export class SkyrimtogetherMock extends EventEmitter implements SkyrimTogether {

  private connected = false;
  private active = false;
  private version = 'browser';
  private playerName = 'Local Player';
  private showEvents = true;
  private localPlayerId: number;
  public readonly players$ = playerStore.pipe(selectAllEntities());

  connect(host: string, port: number, password: string): void {
    if (!this.connected) {
      let error: ErrorEvents | boolean;
      switch (host) {
        case 't-port':
        case 't-host':
          error = true;
          break;
        case 't-password':
          if (password !== 'test') {
            error = { error: 'wrong_password' };
          }
          break;
        case 't-version':
          error = { error: 'wrong_version', data: { version: '[current-version]', expectedVersion: '[expected-version]' } };
          break;
        case 't-full':
          error = { error: 'server_full' };
          break;
        case 't-client-mods':
          error = { error: 'client_mods_disallowed', data: { mods: ['SKSE'] } };
          break;
        case 't-mods':
          error = {
            error: 'mods_mismatch',
            data: { mods: [['missing.esp', '0'], ['remove.esp', '12'], ['missing_2.esp', '0'], ['remove_2.esp', '12']] },
          };
          break;
      }
      setTimeout(() => {
        this.emit(!!error ? 'disconnect' : 'connect');
        this.connected = !error;
        if (error && typeof error !== 'boolean') {
          this.emit('triggerError', JSON.stringify(error));
        } else {
          this.emit('setLocalPlayerId', this.localPlayerId = nextPlayerId++);
        }
        for (const player of playerStore.query(getAllEntities())) {
          this.emit('playerConnected', player.id, player.name, player.level, player.cellName);
          this.emit('setPlayer3dLoaded', player.id, player.health);
        }
      }, 250);
    }
  }

  disconnect(): void {
    if (this.connected) {
      this.emit('disconnect');
      this.connected = false;
    }
  }

  reconnect(): void {
    throw new Error('NOT YET IMPLEMENTED');
  }

  sendMessage(type: MessageTypes, message: string): void {
    if (this.connected) {
      this.emit('message', type, message, this.playerName);
    }
  }

  deactivate(): void {
    throw new Error('NOT YET IMPLEMENTED');
  }

  teleportToPlayer(playerId: number): void {
    if (this.connected) {
      const player = playerStore.query(getEntity(playerId));
      if (player) {
        console.log(
          `%cTELEPORT`, 'background: #F09688; color: #fff; padding: 3px; font-size: 9px;',
          'Teleport to player', JSON.stringify(player.name), `(${ player.id })`, 'in', JSON.stringify(player.cellName),
        );
      }
    }
  }

  launchParty(): void {
    if (this.connected) {
      this.emit('partyCreated');
    }
  }

  createPartyInvite(playerId: number): void {
    playerStore.update(updateEntities(playerId, { invited: true }));
  }

  acceptPartyInvite(inviterId: number): void {
    this.emit('partyCreated');
    this.emit(
      'partyInfo',
      [...playerStore.query(getAllEntities()).filter(p => p.isInGroup).map(p => p.id)],
      inviterId,
    );
  }

  kickPartyMember(playerId: number): void {
    playerStore.update(updateEntities(playerId, { isInGroup: false }));
    this.emit(
      'partyInfo',
      playerStore.query(getAllEntities()).filter(p => p.isInGroup).map(p => p.id),
      this.localPlayerId,
    );
  }

  leaveParty(): void {
    if (this.connected) {
      this.emit('partyInfo', [], -1);
      this.emit('partyLeft');
    }
  }

  changePartyLeader(playerId: number): void {
    playerStore.update(updateEntities(playerId, { hasOwnParty: true }));
    this.emit(
      'partyInfo',
      [...playerStore.query(getAllEntities()).filter(p => p.isInGroup).map(p => p.id)],
      playerId,
    );
  }

  initMock() {
    Object.keys((this as any)._events).forEach(e => {
      this.on(e, (...params) => {
        if (this.showEvents) {
          const eventName = e.replace(/(\G(?!^)|\b[a-zA-Z][a-z]*)([A-Z][a-z]*|\d+)/gm, `$1_$2`).toUpperCase();
          console.log(`%cEVENT`, 'background: #009688; color: #fff; padding: 3px; font-size: 9px;', `[${ eventName }]`, ...params.map(v => JSON.stringify(v)));
        }
      });
    });
    this.emit('init');
    this.emit('enterGame');
    this.emit('setVersion', this.version);
    this.emit('setName', this.playerName);

    fromEvent(window, 'keydown')
      .subscribe((event: KeyboardEvent) => {
        if (event.ctrlKey && event.location === 2) {
          this.active = !this.active;
          this.emit(this.active ? 'activate' : 'deactivate');
          event.preventDefault();
          return;
        }
        switch (event.key) {
          case 'F2': {
            this.active = !this.active;
            this.emit(this.active ? 'activate' : 'deactivate');
            event.preventDefault();
            break;
          }
        }
      });
  }

  setMockVersion(version: string): void {
    this.version = version;
  }

  setMockPlayerName(name: string): void {
    this.playerName = name;
  }

  setShowEvents(show: boolean): void {
    this.showEvents = show;
  }

  addMockPlayer() {
    const newPlayerId = nextPlayerId++;
    const cities = ['Whiterun', 'Dawnstar', 'Falkreath', 'Markarth', 'Morthal', 'Riften', 'Solitude', 'Windhelm'];
    const newPlayer: MockPlayer = {
      name: 'Player ' + newPlayerId,
      id: newPlayerId,
      level: Math.floor(Math.random() * 100),
      health: Math.floor(Math.random() * 100),
      cellName: cities[Math.floor(Math.random() * cities.length)],
      hasOwnParty: false,
      isInGroup: false,
      invited: false,
      invitedLocalPlayer: false,
    };
    playerStore.update(addEntities(newPlayer));
    if (this.connected) {
      this.emit('playerConnected', newPlayer.id, newPlayer.name, newPlayer.level, newPlayer.cellName);
      this.emit('setPlayer3dLoaded', newPlayer.id, newPlayer.health);
    }
    // this.emit('setHealth', newPlayer.id, newPlayer.health);
    return newPlayer;
  }

  disconnectMockPlayer(playerId: number) {
    const mockPlayer = playerStore.query(getEntity(playerId));
    if (mockPlayer) {
      if (this.connected) {
        this.emit('playerDisconnected', mockPlayer.id, mockPlayer.name);
      }
      playerStore.update(deleteEntities(mockPlayer.id));
    }
  }

  accteptMockPlayerInvite(playerId: number) {
    playerStore.update(updateEntities(playerId, { invited: false, isInGroup: true }));
    this.emit(
      'partyInfo',
      playerStore.query(getAllEntities()).filter(p => p.isInGroup).map(p => p.id),
      this.localPlayerId,
    );
  }

  inviteToPlayerMockParty(playerId: number) {
    playerStore.update(updateEntities(playerId, { invitedLocalPlayer: true }));
    this.emit('partyInviteReceived', playerId);
  }

  startPlayerMockParty(playerId: number) {
    playerStore.update(updateEntities(playerId, { hasOwnParty: true, isInGroup: true }));
  }

  mockPlayerLeaveParty(playerId: number) {
    const player = playerStore.query(getEntity(playerId));

    playerStore.update(updateEntities(playerId, { hasOwnParty: false, invitedLocalPlayer: false, isInGroup: false }));
    if (this.connected) {
      if (player.isInGroup) {
        this.emit(
          'partyInfo',
          playerStore.query(getAllEntities()).filter(p => p.isInGroup).map(p => p.id),
          this.localPlayerId,
        );
      } else if (player.hasOwnParty) {
        this.emit('partyInfo', [], -1);
      }
    }
  }

  updateMockDebugData() {
    const debugData = [
      Math.floor(Math.random() * 100),
      Math.floor(Math.random() * 100),
      Math.floor(Math.random() * 100),
      Math.floor((Math.random() / 4) * 100),
      Math.random() * 100,
      Math.random() * 100,
    ];
    this.emit(
      'debugData',
      ...debugData,
    );
    return debugData;
  }
}

export function mockSkyrimTogether() {
  (globalThis as any).skyrimtogether = new SkyrimtogetherMock();
}
