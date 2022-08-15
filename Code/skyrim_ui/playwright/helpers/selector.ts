export function createClassXPathSelector(cssClass: string) {
  return `[contains(concat(" ", normalize-space(@class), " "), " ${ cssClass } ")]`;
}
