import { Injectable } from '@angular/core';
import { createStore, select, withProps } from '@ngneat/elf';
import { PlayerManagerTab } from '../models/player-manager-tab.enum';
import { View } from '../models/view.enum';

interface UiProps {
  view: View | null;
  playerManagerTab: PlayerManagerTab;
  connectIp: string;
  connectPort: number;
  connectName: string;
}

const uiStore = createStore(
  { name: 'auth' },
  withProps<UiProps>({
    view: null,
    playerManagerTab: PlayerManagerTab.PLAYER_LIST,
    connectIp: null,
    connectPort: null,
    connectName: null,
  }),
);

@Injectable({
  providedIn: 'root',
})
export class UiRepository {
  public readonly view$ = uiStore.pipe(select(state => state.view));
  public readonly playerManagerTab$ = uiStore.pipe(
    select(state => state.playerManagerTab),
  );
  public readonly connectIp$ = uiStore.pipe(select((state) => state.connectIp));
  public readonly connectPort$ = uiStore.pipe(select((state) => state.connectPort));
  public readonly connectName$ = uiStore.pipe(select((state) => state.connectName));

  openView(view: UiProps['view']) {
    uiStore.update(state => ({
      ...state,
      view,
    }));
  }

  closeView() {
    uiStore.update(state => ({
      ...state,
      view: null,
    }));
  }

  getView() {
    return uiStore.getValue().view;
  }

  isViewOpen() {
    return !!this.getView();
  }

  getConnectIp() {
    return uiStore.getValue().connectIp;
  }

  getConnectPort() {
    return uiStore.getValue().connectPort;
  }

  getConnectName() {
    return uiStore.getValue().connectName;
  }

  openPlayerManagerTab(playerManagerTab: UiProps['playerManagerTab']) {
    uiStore.update(state => ({
      ...state,
      playerManagerTab,
    }));
  }

  openConnectWithPasswordView(connectIp: string, connectPort: number, connectName: string) {
    uiStore.update((state) => ({
      ...state,
      view: View.CONNECT_PASSWORD,
      connectIp,
      connectPort,
      connectName,
    }))
  }
}
