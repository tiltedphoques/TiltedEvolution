import { animate, group, query, state, style, transition, trigger } from '@angular/animations';


export const controlsAnimation = trigger('controls', [
  state('false', style({
    'visibility': 'hidden',
    'opacity': '0',
    'transform': 'translateY(1rem)',
  })),
  transition('false => true', [
    query('.app-root-menu', style({
      'opacity': '0',
      'width': '0',
    }), { optional: true }),
    query('.app-root-menu button', style({
      'opacity': '0',
    }), { optional: true }),
    group([
      animate('200ms ease-out', style({
        'visibility': '*',
        'opacity': '*',
        'transform': '*'
      })),
      query('.app-root-menu', animate('200ms ease-out', style({
        'opacity': '*',
        'width': '*'
      })), { optional: true }),
    ]),
    query('.app-root-menu button', animate('200ms ease-out', style({
      'opacity': '*'
    })), { optional: true })
  ]),
  transition('true => false', [
    animate('200ms ease-in', style({
      'opacity': '0',
      'transform': 'translateY(1rem)'
    }))
  ])
]);
