import { Injectable } from '@angular/core';
import { StoreService } from './store.service';

@Injectable({
  providedIn: 'root'
})
export class SettingService {

    private muted = false;
    private volume = 0.5;

    constructor(private storeService: StoreService) {
        this.muted = JSON.parse(this.storeService.get('audio_muted', false));
        this.volume = JSON.parse(this.storeService.get('audio_volume', 0.5));
    }

    public setVolume(volume: number) {
        this.volume = volume;
        this.storeService.set('audio_volume', volume);
    }

    public getVolume(): number {
        return this.volume;
    }

    public mute(muted: boolean) {
        this.muted = muted;
        this.storeService.set('audio_muted', muted);
    }

    public isMuted(): boolean {
        return this.muted;
    }
}