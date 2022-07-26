import { ScrollingModule } from '@angular/cdk/scrolling';
import { HttpClientModule } from '@angular/common/http';
import { NgModule } from '@angular/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { FontAwesomeModule } from '@fortawesome/angular-fontawesome';
import { environment } from '../environments/environment';
import { ActionButtonsComponent } from './components/action-buttons/action-buttons.component';
import { ChatComponent } from './components/chat/chat.component';
import { CheckboxComponent } from './components/checkbox/checkbox.component';
import { ConnectComponent } from './components/connect/connect.component';
import { DebugComponent } from './components/debug/debug.component';
import { DisconnectComponent } from './components/disconnect/disconnect.component';
import { DropdownOptionComponent } from './components/dropdown/dropdown-option.component';
import { DropdownComponent } from './components/dropdown/dropdown.component';
import { ErrorComponent } from './components/error/error.component';
import { GroupComponent } from './components/group/group.component';
import { LoadingComponent } from './components/loading/loading.component';
import { NotificationPopupContainerComponent } from './components/notification-popup-container/notification-popup-container.component';
import { NotificationPopupComponent } from './components/notification-popup/notification-popup.component';
import { NotificationsComponent } from './components/notifications/notifications.component';
import { OrderComponent } from './components/order/order.component';
import { PartyMenuComponent } from './components/party-menu/party-menu.component';
import { PlayerListComponent } from './components/player-list/player-list.component';
import { PlayerManagerComponent } from './components/player-manager/player-manager.component';
import { PopupComponent } from './components/popup/popup.component';
import { RootComponent } from './components/root/root.component';
import { ServerListComponent } from './components/server-list/server-list.component';
import { SettingsComponent } from './components/settings/settings.component';
import { ToggleComponent } from './components/toggle/toggle.component';
import { WindowComponent } from './components/window/window.component';
import { HealthDirective } from './directives/health.directive';
import { AssetService } from './services/asset.service';
import { ClientService } from './services/client.service';
import { ErrorService } from './services/error.service';
import { GroupService } from './services/group.service';
import { PlayerListService } from './services/player-list.service';
import { SettingService } from './services/setting.service';
import { SoundService } from './services/sound.service';
import { StoreService } from './services/store.service';
import { TranslocoRootModule } from './transloco-root.module';


@NgModule({
  declarations: [
    ChatComponent,
    CheckboxComponent,
    ConnectComponent,
    DebugComponent,
    DisconnectComponent,
    DropdownComponent,
    DropdownOptionComponent,
    ErrorComponent,
    GroupComponent,
    LoadingComponent,
    NotificationPopupComponent,
    NotificationPopupContainerComponent,
    NotificationsComponent,
    OrderComponent,
    PartyMenuComponent,
    PlayerListComponent,
    PlayerManagerComponent,
    ActionButtonsComponent,
    PopupComponent,
    RootComponent,
    ServerListComponent,
    SettingsComponent,
    ToggleComponent,
    WindowComponent,

    HealthDirective,
  ],
  imports: [
    BrowserAnimationsModule,
    BrowserModule,
    FontAwesomeModule,
    FormsModule,
    HttpClientModule,
    ReactiveFormsModule,
    ScrollingModule,
    TranslocoRootModule,
  ],
  providers: [
    ClientService,
    AssetService,
    SoundService,
    ErrorService,
    GroupService,
    PlayerListService,
    StoreService,
    SettingService,

    ...environment.providers,
  ],
  bootstrap: [RootComponent],
})
export class AppModule {
}
