import { trigger, state, animate, style, transition } from '@angular/animations';

export const animation = trigger('popups', [
  state('false', style({
    'visibility': 'hidden',
    'opacity': '0'
  })),
  transition('false => true', [
    animate('200ms ease-out', style({
      'visibility': '*',
      'opacity': '*'
    }))
  ]),
  transition('true => false', [
    animate('200ms ease-in', style({
      'opacity': '0'
    }))
  ])
]);
