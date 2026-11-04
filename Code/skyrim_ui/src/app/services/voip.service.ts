import { Injectable, NgZone } from '@angular/core';
import { BehaviorSubject } from 'rxjs';
import { PlayerListService } from './player-list.service';
import { StoreService } from './store.service';

export enum VoipChannel {
  EVERYONE = 'everyone',
  PARTY = 'party',
  STRANGERS = 'strangers',
}

/** Voice chat settings, saved by the game in the player's STServer.ini. */
export interface VoipSettings {
  /** Empty sends nothing, 'default' follows the Windows default device. */
  inputDevice: string;
  connect: boolean;
  /** Empty plays on the game's (the Windows default) device. */
  outputDevice: string;
  channel: VoipChannel;
  /** Full volume up to this distance in game units, then quieter like real sound, silent at four times it. */
  fadeStart: number;
  /** 0 to 100 */
  micVolume: number;
  /** 0 to 100, like the Windows "Test your microphone" bar. */
  noiseGate: number;
  /** Playback gain, 0 to 500 percent. */
  masterVolume: number;
}

export const defaultVoipSettings: VoipSettings = {
  inputDevice: '',
  connect: false,
  outputDevice: '',
  channel: VoipChannel.EVERYONE,
  fadeStart: 2000,
  micVolume: 100,
  noiseGate: 5,
  masterVolume: 100,
};

/** Per player voice volumes by username, 0 to 1, kept across servers and sessions like the name overrides. */
const volumesStoreKey = 'playerUi_voiceVolumes';
const defaultPlayerVolume = 100;

@Injectable({
  providedIn: 'root',
})
export class VoipService {
  public readonly settings = new BehaviorSubject<VoipSettings>(
    defaultVoipSettings,
  );
  public readonly inputDevices = new BehaviorSubject<string[]>([]);
  public readonly outputDevices = new BehaviorSubject<string[]>([]);
  /** Players whose voice is getting through their noise gate right now. */
  public readonly talking = new BehaviorSubject<Set<number>>(new Set());
  /** Local microphone level, 0 to 100, while the meter is on. */
  public readonly micLevel = new BehaviorSubject<number>(0);
  /** Per player volumes by username, 0 to 100. */
  public readonly volumes: BehaviorSubject<Record<string, number>>;

  constructor(
    private readonly zone: NgZone,
    private readonly storeService: StoreService,
    private readonly playerListService: PlayerListService,
  ) {
    this.volumes = new BehaviorSubject(this.loadVolumes());

    skyrimtogether.on('voiceSettings', this.onSettings.bind(this));
    skyrimtogether.on('voiceDevices', this.onDevices.bind(this));
    skyrimtogether.on('voiceTalking', this.onTalking.bind(this));
    skyrimtogether.on('voiceMicLevel', this.onMicLevel.bind(this));

    // The game forgets volumes when it disconnects and only knows players by id, so they're sent again whenever
    // the player list changes
    this.playerListService.playerList.subscribe(() => this.sendVolumes());

    this.refresh();
  }

  /** Asks the game for the saved settings and the device lists. */
  public refresh(): void {
    skyrimtogether.voiceRefresh();
  }

  public update(changes: Partial<VoipSettings>): void {
    const settings = { ...this.settings.value, ...changes };
    settings.fadeStart = Math.max(
      0,
      Math.min(100000, Math.round(settings.fadeStart || 0)),
    );
    settings.micVolume = this.clampPercent(settings.micVolume);
    settings.noiseGate = this.clampPercent(settings.noiseGate);
    settings.masterVolume = Number.isFinite(settings.masterVolume)
      ? Math.max(0, Math.min(500, Math.round(settings.masterVolume)))
      : 100;
    this.settings.next(settings);

    skyrimtogether.setVoiceSettings(
      settings.inputDevice,
      settings.connect ? 1 : 0,
      settings.outputDevice,
      settings.channel,
      settings.fadeStart,
      settings.micVolume,
      settings.noiseGate,
      settings.masterVolume,
    );
  }

  public getVolume(name: string): number {
    return this.volumes.value[name] ?? defaultPlayerVolume;
  }

  public setVolume(playerId: number, name: string, volume: number): void {
    const clamped = this.clampPercent(volume);
    const volumes = { ...this.volumes.value };
    if (clamped === defaultPlayerVolume) {
      delete volumes[name];
    } else {
      volumes[name] = clamped;
    }

    this.volumes.next(volumes);
    this.saveVolumes(volumes);
    skyrimtogether.setVoicePlayerVolume(playerId, clamped);
  }

  public setMeter(enabled: boolean): void {
    if (!enabled) {
      this.micLevel.next(0);
    }
    skyrimtogether.setVoiceMeter(enabled ? 1 : 0);
  }

  private sendVolumes(): void {
    const players = this.playerListService.playerList.value?.players ?? [];
    for (const player of players) {
      skyrimtogether.setVoicePlayerVolume(
        player.id,
        this.getVolume(player.name),
      );
    }
  }

  private onSettings(
    inputDevice: string,
    connect: number,
    outputDevice: string,
    channel: string,
    fadeStart: number,
    micVolume: number,
    noiseGate: number,
    masterVolume: number = 100,
  ): void {
    const knownChannel = Object.values(VoipChannel).includes(
      channel as VoipChannel,
    )
      ? (channel as VoipChannel)
      : VoipChannel.EVERYONE;

    this.zone.run(() => {
      this.settings.next({
        inputDevice,
        connect: !!connect,
        outputDevice,
        channel: knownChannel,
        fadeStart,
        micVolume,
        noiseGate,
        masterVolume,
      });
    });
  }

  private onDevices(inputs: string[], outputs: string[]): void {
    this.zone.run(() => {
      this.inputDevices.next(inputs ?? []);
      this.outputDevices.next(outputs ?? []);
    });
  }

  private onTalking(playerIds: number[]): void {
    this.zone.run(() => {
      this.talking.next(new Set(playerIds ?? []));
    });
  }

  private onMicLevel(level: number): void {
    this.zone.run(() => {
      this.micLevel.next(this.clampPercent(level));
    });
  }

  private loadVolumes(): Record<string, number> {
    try {
      const stored = JSON.parse(this.storeService.get(volumesStoreKey, '{}'));
      const volumes: Record<string, number> = {};
      for (const [name, value] of Object.entries(stored ?? {})) {
        if (typeof value === 'number' && isFinite(value)) {
          volumes[name] = this.clampPercent(value * 100);
        }
      }
      return volumes;
    } catch {
      return {};
    }
  }

  private saveVolumes(volumes: Record<string, number>): void {
    const stored: Record<string, number> = {};
    for (const [name, value] of Object.entries(volumes)) {
      stored[name] = value / 100;
    }
    this.storeService.set(volumesStoreKey, JSON.stringify(stored));
  }

  private clampPercent(value: number): number {
    return Math.max(0, Math.min(100, Math.round(Number(value) || 0)));
  }
}
