import { ChangeDetectorRef, Component, ElementRef, forwardRef, Input, Renderer2 } from '@angular/core';
import { ControlValueAccessor, NG_VALUE_ACCESSOR } from '@angular/forms';
import { DestroyService } from '../../services/destroy.service';


let checkboxCounter = 1;

const noop = () => {
};

@Component({
  selector: 'app-checkbox',
  templateUrl: './checkbox.component.html',
  styleUrls: ['./checkbox.component.scss'],
  providers: [
    {
      provide: NG_VALUE_ACCESSOR,
      useExisting: forwardRef(() => CheckboxComponent),
      multi: true,
    },
    DestroyService,
  ],
})
export class CheckboxComponent implements ControlValueAccessor {

  private onTouchedCallback: () => void = noop;
  private onChangeCallback: (_: any) => void = noop;

  checked: boolean;
  disabled: boolean = false;
  _id: string = 'app-checkbox-' + (checkboxCounter++);

  @Input() set id(id: string) {
    this.renderer.removeAttribute(this.elementRef.nativeElement, 'id');
    this._id = id;
  };

  constructor(
    private readonly cdr: ChangeDetectorRef,
    private readonly elementRef: ElementRef,
    private readonly renderer: Renderer2,
  ) {
  }

  setValue(checked: boolean) {
    this.checked = checked;
    this.onChangeCallback(checked);
  }

  writeValue(obj: boolean): void {
    this.checked = !!obj;
    this.cdr.detectChanges();
  }

  registerOnChange(fn: any) {
    this.onChangeCallback = fn;
  }

  registerOnTouched(fn: any) {
    this.onTouchedCallback = fn;
  }

  setDisabledState?(isDisabled: boolean): void {
    this.disabled = isDisabled;
    this.cdr.detectChanges();
  }

}
