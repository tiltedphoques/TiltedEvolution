import { Component, OnInit, ViewEncapsulation, Input } from '@angular/core';
import { Player } from '../../models/player';
import { GroupService } from '../../services/group.service';

@Component({
  selector: 'app-friend',
  templateUrl: './friend.component.html',
  styleUrls: ['./friend.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class FriendComponent implements OnInit {
  @Input('friend')
  public friend: Player;

  constructor(private groupService: GroupService) { }

  ngOnInit() {
  }

  public accept(player: Player) {
    this.groupService.accept(player);
  }

  public invite(player: Player): void {
    if (player) {
      player.invitationSent = true;
    }

    this.groupService.invite(player.id);
  }

  public isInGroup(player: Player): boolean {
    const group = this.groupService.group.value;
    if (group) {
      return ((group.owner && group.owner.name === player.name) || group.members.has(player.id));
    }
    return false;
  }

}
