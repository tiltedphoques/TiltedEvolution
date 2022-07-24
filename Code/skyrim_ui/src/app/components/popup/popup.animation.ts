import { animate, group, query, state, style, transition, trigger } from '@angular/animations';


export const animation = trigger('popup', [
  state('void', style({
    'visibility': 'hidden',
    'opacity': '0',
  })),
  transition('void => *', [
    query('app-window', style({
      'transform': 'scale(1.1)'
    })),
    group([
      animate('200ms ease-out', style({
        'visibility': 'visible',
        'opacity': '1',
      })),
      query('app-window', animate('200ms ease-out', style({
        'transform': 'scale(1)'
      }))),
    ])
  ]),
  transition('* => void', [
    group([
      animate('200ms ease-out', style({
        'opacity': '0'
      })),
      query('app-window', animate('200ms ease-out', style({
        'transform': 'scale(0.9)'
      }))),
    ])
  ])
]);
