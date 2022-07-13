import { SettingService } from '../../setting.service';
import { Asset } from '../asset';


export class AudioAsset extends Asset<AudioBuffer> {
  public constructor(content: AudioBuffer, private settings: SettingService) {
    super(content);
  }

  public play(): void {

    const gainNode = AudioAsset.context.createGain();

    if(this.settings.isAudioMuted()) {
      gainNode.gain.value = 0.0;
    } else {
      gainNode.gain.value = this.settings.getVolume();
    }
    gainNode.connect(AudioAsset.context.destination);

    const source = AudioAsset.context.createBufferSource();

    source.addEventListener('ended', () => {
      source.disconnect(gainNode);
    });

    source.buffer = this.content;

    source.connect(gainNode);
    source.start();
  }

  private static context = new AudioContext();
}
