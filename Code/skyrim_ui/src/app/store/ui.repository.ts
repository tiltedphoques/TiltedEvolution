import { Injectable } from '@angular/core';
import { createStore, select, withProps } from '@ngneat/elf';
import { PlayerManagerTab } from '../models/player-manager-tab.enum';
import { View } from '../models/view.enum';


interface UiProps {
  view: View | null;
  playerManagerTab: PlayerManagerTab;
}

const uiStore = createStore(
  { name: 'auth' },
  withProps<UiProps>({
    view: null,
    playerManagerTab: PlayerManagerTab.PLAYER_LIST,
  }),
);


@Injectable({
  providedIn: 'root',
})
export class UiRepository {

  public readonly view$ = uiStore.pipe(select((state) => state.view));
  public readonly playerManagerTab$ = uiStore.pipe(select((state) => state.playerManagerTab));

  openView(view: UiProps['view']) {
    uiStore.update((state) => ({
      ...state,
      view,
    }));
  }

  closeView() {
    uiStore.update((state) => ({
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

  openPlayerManagerTab(playerManagerTab: UiProps['playerManagerTab']) {
    uiStore.update((state) => ({
      ...state,
      playerManagerTab,
    }));
  }

}
