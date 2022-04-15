import { Injectable, OnDestroy } from '@angular/core';
import { Observable, Observer, concat, throwError, Subject, interval, Subscription } from 'rxjs';
import { environment } from '../../environments/environment';
import { retryWhen, take, delay } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})
export class WebSocketService implements OnDestroy {

  message = new Subject<any>();

  private ws: WebSocket;
  private _pingObs: Subscription;

  /*constructor() {
    this.ws = new WebSocket(`wss://presence.test`);
    this.ws.onopen = () => this.onOpen();
    this.ws.onclose = () => this.onClose();
    this.ws.onmessage = (evt) => {
      this.message.next(JSON.parse(evt.data));
    };
    this.ws.onerror = (evt) => console.log('ON ERROR', evt);
  }*/

  ngOnDestroy() {
    this.ws.close();
  }

  onOpen() {
    console.log('[WS] on open');
    this._pingObs = interval(environment.intervalPingWebSocket * 1000)
      .subscribe(() => {
        this.send('', true);
      });
  }

  onClose() {
    console.log('[WS] on close');
    if (this._pingObs) {
      this._pingObs.unsubscribe();
    }
  }
  /**
   * Try to send the message, if the connection is not ready, wait 500 seconds then try again.
   * If after 5 tries the connection is still not ready, return an error.
   *
   * @param msg
   */
  send(msg: any, forceSubscribe = false): Observable<any> {
    console.log(JSON.stringify(msg));
    const obs: Observable<any> = Observable.create((observer: Observer<any>) => {
      if (this.ws.readyState === WebSocket.OPEN) {
        this.ws.send(JSON.stringify(msg));
        observer.complete();
      }
      else {
        observer.error('');
      }
    })


    obs.pipe(
      retryWhen((errors) => {
        return concat(errors.pipe(delay(500), take(5)), throwError('The websocket is not connected yet'))
      })
    );

    if (forceSubscribe) {
      obs.subscribe({
        complete() { console.log('[ws] send completed'); },
        error(err) { console.error(err); }
      });
    }
    return obs;
  }
}
