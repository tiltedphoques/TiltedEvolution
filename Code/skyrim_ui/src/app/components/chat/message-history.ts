/** Helper for storing sent message history */
export class MessageHistory {
  
  private stack: string[] = [];
  private offset = 0;

  constructor() {
  }

  public push(newMsg: string) {
    const isNewMessageDifferent = newMsg != this.stack[this.offset];

    if (isNewMessageDifferent) {
      this.stack.push(newMsg);  
    }

    this.offset = this.stack.length;
  }

  /**
   * @param fallback used if there is no history yet
   */
  public prev(fallback: string): string {
    this.offset -= 1;

    const isHistotyEmpty = this.stack.length === 0;
    if(isHistotyEmpty) {
      this.offset = 0;
      return fallback;
    }

    const isOffsetTooLow = this.offset < 0;
    if(isOffsetTooLow) {
      this.offset = 0;
      return this.stack[this.offset];
    }

    return this.stack[this.offset];
  }

  public next(): string {
    this.offset += 1;

    const isOffsetTooHigh = this.offset > this.stack.length - 1;
    if (isOffsetTooHigh) {
      this.offset = this.stack.length;
      return '';
    }

    return this.stack[this.offset];
  }
}
