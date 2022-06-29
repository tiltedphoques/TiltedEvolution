import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { FontAwesomeModule } from '@fortawesome/angular-fontawesome';
import {MatTabsModule} from '@angular/material/tabs';

import { PopupButtonsDirective } from './directives/popup-buttons.directive';
import { HealthDirective } from './directives/health.directive';

import { RootComponent } from './components/root/root.component';
import { ConnectComponent } from './components/connect/connect.component';
import { DisconnectComponent } from './components/disconnect/disconnect.component';
import { NotificationsComponent } from './components/notifications/notifications.component';
import { ChatComponent } from './components/chat/chat.component';
import { PopupComponent } from './components/popup/popup.component';
import { WindowComponent } from './components/window/window.component';

import { ClientService } from './services/client.service';
import { AssetService } from './services/asset.service';
import { SoundService } from './services/sound.service';
import { DebugComponent } from './components/debug/debug.component';
import { ErrorService } from './services/error.service';
import { ErrorComponent } from './components/error/error.component';
import { GroupService } from './services/group.service';
import { StoreService } from './services/store.service';
import { SettingService } from './services/setting.service';
import { PlayerListService } from './services/player-list.service';
import { MockClientService } from './services/mock-client.service';

import { GroupComponent } from './components/group/group.component';
import { NotificationPopupComponent } from './components/notification-popup/notification-popup.component';
import { ToggleComponent } from './components/toggle/toggle.component';
import { LoadingComponent } from './components/loading/loading.component';
import { ServerListComponent } from './components/server-list/server-list.component';
import { OrderComponent } from './components/order/order.component';
import { SettingsComponent } from './components/settings/settings.component';
import { PlayerListComponent } from './components/player-list/player-list.component';
import { PlayerManagerComponent } from './components/player-manager/player-manager.component';
import { PartyMenuComponent } from './components/party-menu/party-menu.component';



@NgModule({
  declarations: [
    PopupButtonsDirective, RootComponent,
    ConnectComponent, DisconnectComponent, NotificationsComponent, ChatComponent, PopupComponent,
    WindowComponent, DebugComponent, ErrorComponent, GroupComponent, NotificationPopupComponent, HealthDirective,
    ToggleComponent,
    LoadingComponent,
    ServerListComponent, SettingsComponent, PlayerListComponent, PlayerManagerComponent, PartyMenuComponent,
    OrderComponent
  ],
  imports: [ BrowserModule, BrowserAnimationsModule, FormsModule, HttpClientModule, ReactiveFormsModule, FontAwesomeModule, MatTabsModule ],
  providers: [ ClientService, MockClientService, AssetService, SoundService,
    ErrorService, GroupService, PlayerListService, StoreService, SettingService ],
  bootstrap: [ RootComponent ]
})
export class AppModule {}
