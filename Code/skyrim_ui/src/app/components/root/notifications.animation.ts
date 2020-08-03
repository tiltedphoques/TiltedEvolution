import { trigger, state, animate, style, transition } from '@angular/animations';

export const animation = trigger('notifications', [
  state('false', style({
    'visibility': 'hidden',
    'opacity': '0',
    'transform': 'translateY(1rem)'
  })),
  transition('false => true', [
    animate('200ms ease-out', style({
      'visibility': '*',
      'opacity': '*',
      'transform': '*'
    }))
  ]),
  transition('true => false', [
    animate('200ms ease-in', style({
      'opacity': '0',
      'transform': 'translateY(1rem)'
    }))
  ])
]);
