import { addStyleSheet, changedEvent } from "../utils";
import { Field } from "./Field";
import styles from "./IntegerField.css";

class IntegerField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options, styles);
    addStyleSheet(this.shadow, styles);

    const field = (this.field = document.createElement("input"));
    field.setAttribute("class", "data-field");
    if (options && options.width) {
      field.style.setProperty("width", field.getAttribute("width"));
    }
    field.placeholder = (options && options.placeholder) || null;

    const dataLabel = document.createElement("span");
    dataLabel.setAttribute("class", "data-label");
    dataLabel.innerHTML = (options && options.label) + ":" || null;

    this.wrapper.appendChild(dataLabel);
    this.wrapper.appendChild(field);

    this.min = (options && options.min) || null;
    this.max = (options && options.max) || null;
  }

  isValid() {
    if (this.field) {
      this.field.value = this.field.value.match(/^[-]?\d*/g);

      if (this.field.value.charAt(0) == "0") {
        this.field.value = this.field.value.slice(1);
      }

      if (this.max) {
        if (parseInt(this.field.value) > parseInt(this.max)) {
          this.field.value = this.max;
        }
      }

      if (this.min) {
        if (parseInt(this.field.value) < parseInt(this.min)) {
          this.field.value = this.min;
        }
      }

      if (this.field.value.charAt(0) == "-") {
        setTimeout(() => {
          if (!parseInt(this.field.value)) {
            console.error("launch invalid integer popup");
          }
        }, 1000);
      }
    }

    // if (element.value == "") {
    //   console.error("launch popup empty ip");
    //   this.value = [];
    //   return false;
    // }

    // this.value.push(element.value);
    this.value = this.field.value;
    return true;
  }

  get value() {
    return this._value;
  }

  set value(new_value) {
    this._value = parseInt(new_value);
    this.field.value = this._value;
  }
}

customElements.define("integer-field", IntegerField);
export { IntegerField };
