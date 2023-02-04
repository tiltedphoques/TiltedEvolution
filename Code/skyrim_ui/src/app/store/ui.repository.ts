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
  hideVersionMismatchedServers: boolean;
  hideFullServers: boolean;
  hidePasswordProtectedServers: boolean;
  joinedLanServer: boolean;
}

const uiStore = createStore(
  { name: 'auth' },
  withProps<UiProps>({
    view: null,
    playerManagerTab: PlayerManagerTab.PLAYER_LIST,
    connectIp: null,
    connectPort: null,
    connectName: null,
    hideVersionMismatchedServers: true,
    hideFullServers: true,
    hidePasswordProtectedServers: true,
    joinedLanServer: false,
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
  public readonly hideVersionMismatchedServers$ = uiStore.pipe(select((state) => state.hideVersionMismatchedServers));
  public readonly hideFullServers$ = uiStore.pipe(select((state) => state.hideFullServers));
  public readonly hidePasswordProtectedServers$ = uiStore.pipe(select((state) => state.hidePasswordProtectedServers));
  public readonly joinedLanServer$ = uiStore.pipe(select((state) => state.joinedLanServer));

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

  hasJoinedLanServer() {
    return uiStore.getValue().joinedLanServer;
  }

  setJoinedLanServer(joinedLanServer: boolean) {
    uiStore.update(state => ({
      ...state,
      joinedLanServer,
    }))
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

  getHideVersionMismatchedServers() {
    return uiStore.getValue().hideVersionMismatchedServers;
  }

  getHideFullServers() {
    return uiStore.getValue().hideFullServers;
  }

  getHidePasswordProtectedServers() {
    return uiStore.getValue().hidePasswordProtectedServers;
  }

  setHideVersionMismatchedServers(hideVersionMismatchedServers: boolean) {
    uiStore.update(state => ({
      ...state,
      hideVersionMismatchedServers,
    }))
  }

  setHideFullServers(hideFullServers: boolean) {
    uiStore.update(state => ({
      ...state,
      hideFullServers,
    }))
  }

  setHidePasswordProtectedServers(hidePasswordProtectedServers: boolean) {
    uiStore.update(state => ({
      ...state,
      hidePasswordProtectedServers,
    }))
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
