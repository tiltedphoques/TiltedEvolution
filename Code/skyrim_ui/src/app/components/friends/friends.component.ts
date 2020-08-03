import {
  Component, ViewEncapsulation, Output, EventEmitter,
  HostListener,
  OnInit,
  OnDestroy
} from '@angular/core';

import { UserService } from '../../services/user.service';
import { SoundService, Sound } from '../../services/sound.service';
import { Player } from '../../models/player';
import { ErrorService } from '../../services/error.service';
import { Group } from '../../models/group';
import { GroupService } from '../../services/group.service';
import { Subscription } from 'rxjs';
import { FormControl } from '@angular/forms';
import { debounceTime, distinctUntilChanged } from 'rxjs/operators';
import { LoadingService } from '../../services/loading.service';

@Component({
  selector: 'app-friends',
  templateUrl: './friends.component.html',
  styleUrls: [ './friends.component.scss' ],
  encapsulation: ViewEncapsulation.None
})
export class FriendsComponent implements OnInit, OnDestroy {
  @Output()
  public done = new EventEmitter();
  public searchOnlineFriends: Player[] = [];
  public searchOfflineFriends: Player[] = [];
  public isShowOffline = false;
  public isShowOnline = true;
  public searchOnline = new FormControl();

  private _onlineFriends: Player[] = [];
  private _offlineFriends: Player[] = [];
  private userSubscription: Subscription;
  private searchSubscription: Subscription;

  public constructor(
    private userService: UserService,
    private sound: SoundService,
    private errorService: ErrorService,
    private groupService: GroupService,
    private loadingService: LoadingService
  ) {
  }

  ngOnInit() {
    this.onUserSubscription();
    this.onSearchSubscription();
  }

  ngOnDestroy() {
    this.userSubscription.unsubscribe();
    this.searchSubscription.unsubscribe();
  }

  public onSearchSubscription() {
    this.searchSubscription = this.searchOnline.valueChanges
      .pipe(
        debounceTime(400),
        distinctUntilChanged()
      )
      .subscribe((value) => {
        this.searchOnlineFriends = this._onlineFriends.filter((p: Player) => {
          return p.name.toLowerCase().includes(value.toLowerCase());
        });

        this.searchOfflineFriends = this._offlineFriends.filter((p: Player) => {
          return p.name.toLowerCase().includes(value.toLowerCase());
        })
      })
  }

  static sortName(a: Player, b: Player) {
    if (a.name < b.name) {
      return -1;
    }
    else if (a.name > b.name) {
      return 1;
    }
    return 0;
  }

  private onUserSubscription() {
    this.userSubscription = this.userService.user.subscribe((user) => {
      if (user) {
        this._onlineFriends = user.friends.filter((p: Player) => p.online);
        this._offlineFriends = user.friends.filter((p: Player) => !p.online);

        this._onlineFriends.sort(FriendsComponent.sortName);
        this._offlineFriends.sort(FriendsComponent.sortName);

        this.loadingService.setLoading(false);
      }
    })
  }

  public get token(): string | undefined {
    return this.userService.user.value ? this.userService.user.value.token : undefined;
  }

  public get friends(): Player[] | undefined {
    return this.userService.user.value ? this.userService.user.value.friends : undefined;
  }

  public get group(): Group | undefined {
    return this.groupService.group.value;
  }

  public get nbOnline(): number {
    return this.searchOnline.value ? this.searchOnlineFriends.length : this._onlineFriends.length;
  }

  public get onlineFriends(): Player[] {
    return this.searchOnline.value ? this.searchOnlineFriends : this._onlineFriends;
  }

  public get offlineFriends(): Player[] {
    return this.searchOnline.value ? this.searchOfflineFriends : this._offlineFriends;
  }

  public close(): void {
    this.sound.play(Sound.Cancel);
    this.done.next();
    this.removeInvite();
  }

  public refresh(): void {
    this.loadingService.setLoading(true);
    this.userService.refreshFriends().subscribe(
      () => {},
      () => {
        this.errorService.error("An error occurred while trying to retrieve your friends' list. Try again later.");
        this.loadingService.setLoading(false);
      }
    );
  }

  private removeInvite() {
    if (this.friends) {
      this.friends.map((p: Player) => p.invitationSent = false);
    }
  }

  public toggleOnline(value: boolean) {
    this.isShowOnline = value;
  }

  public toggleOffline() {
    this.isShowOffline = !this.isShowOffline;
  }

  @HostListener('window:keydown.escape', [ '$event' ])
  // @ts-ignore
  private activate(event: KeyboardEvent): void {
    if (this.errorService.error$.value) {
      this.errorService.removeError();
    }
    else {
      this.close();
    }

    event.stopPropagation();
    event.preventDefault();
  }
}
