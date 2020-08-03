import { Asset } from '../asset';

export class AudioAsset extends Asset<AudioBuffer> {
  public constructor(content: AudioBuffer) {
    super(content);
  }

  public play(): void {
    const source = AudioAsset.context.createBufferSource();

    source.addEventListener('ended', () => {
      source.disconnect(AudioAsset.context.destination);
    });

    source.buffer = this.content;

    source.connect(AudioAsset.context.destination);
    source.start();
  }

  private static context = new AudioContext();
}
