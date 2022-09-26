import { AfterViewInit, ChangeDetectorRef, Component, ContentChildren, ElementRef, EventEmitter, forwardRef, HostListener, Input, Output, QueryList } from '@angular/core';
import { ControlValueAccessor, NG_VALUE_ACCESSOR } from '@angular/forms';
import { BehaviorSubject, startWith, takeUntil } from 'rxjs';
import { Sound, SoundService } from 'src/app/services/sound.service';
import { DestroyService } from '../../services/destroy.service';
import { DropdownOptionComponent } from './dropdown-option.component';


const noop = () => {
};

let dropdownCounter = 1;

@Component({
  selector: 'app-dropdown',
  templateUrl: './dropdown.component.html',
  styleUrls: ['./dropdown.component.scss'],
  providers: [
    {
      provide: NG_VALUE_ACCESSOR,
      useExisting: forwardRef(() => DropdownComponent),
      multi: true,
    },
    DestroyService,
  ],
})
export class DropdownComponent implements AfterViewInit, ControlValueAccessor {

  dropdownCounter = dropdownCounter++;
  isOpen = false;
  isDisabled = false;
  selected: any;
  options = new BehaviorSubject<{ text: string; value: any }[]>([]);

  @ContentChildren(DropdownOptionComponent) optionChildren!: QueryList<DropdownOptionComponent>;
  @Input() placeholder: string;
  @Output() optSelect = new EventEmitter();

  private onTouchedCallback: () => void = noop;
  private onChangeCallback: (_: any) => void = noop;
  isSelectedValue: boolean;
  key: string;
  isFocused: boolean;

  constructor(
    private readonly destroy$: DestroyService,
    private readonly soundService: SoundService,
    private readonly cdr: ChangeDetectorRef,
    private readonly elRef:ElementRef
  ) {
  }

  ngAfterViewInit() {
    this.optionChildren.changes.pipe(
      takeUntil(this.destroy$),
      startWith(null),
    ).subscribe(() => {
      const options = this.optionChildren.toArray();
      this.options.next(
        options.map(o => ({
          text: o.text,
          value: o.value,
        })),
      );
      this.cdr.detectChanges();
    });
  }

  @HostListener('focus')
  focusHandler() {
    this.selected = 0;
    this.isFocused = true;
  }

  @HostListener('focusout')
  focusOutHandler() {
    this.isFocused = false;
  }

  optionSelect(selectedOption: any, idx: number) {
    this.selected = idx;
    this.isSelectedValue = true;
    this.isOpen = false;
    this.soundService.play(Sound.Check);
    this.onChangeCallback(selectedOption);
    this.optSelect.emit(selectedOption);
  }

  toggle() {
    if (this.isDisabled) {
      return;
    }
    this.soundService.play(this.isOpen ? Sound.Cancel : Sound.Focus);

    this.isOpen = !this.isOpen;
    if (this.selected >= 0) {
      document
        .querySelector(`#li-${ this.dropdownCounter }-${ this.selected }`)
        .scrollIntoView({ behavior: 'smooth', block: 'nearest' });
    }
  }

  @HostListener('document:click', ['$event'])
  onClick(e: PointerEvent) {
    const target = (e.target as Element);
    const thisElement = this.elRef.nativeElement as Element;

    if (this.isOpen && !thisElement.contains(target)) {
      this.soundService.play(Sound.Cancel);
      this.isOpen = false;
    }
  }

  writeValue(obj: any): void {
    if (obj != null) {
      this.isSelectedValue = true;
      this.selected = this.options.getValue().findIndex(o => o.value === obj);
    } else {
      this.isSelectedValue = false;
    }
  }

  registerOnChange(fn: any) {
    this.onChangeCallback = fn;
  }

  registerOnTouched(fn: any) {
    this.onTouchedCallback = fn;
  }

  setDisabledState?(isDisabled: boolean): void {
    this.isDisabled = isDisabled;
    this.isOpen = false;
    this.cdr.detectChanges();
  }
}
