import { animate, state, style, transition, trigger } from '@angular/animations';


export const fadeInOutAnimation = trigger('fadeInOut', [
  state('void', style({
    'visibility': 'hidden',
    'opacity': '0',
  })),
  transition('void => *', [
    animate('300ms ease-in', style({
      'visibility': 'visible',
      'opacity': '1',
    })),
  ]),
  transition('* => void', [
    animate('300ms ease-out', style({
      'opacity': '0',
    })),
  ]),
]);
