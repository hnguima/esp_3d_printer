import { addStyleSheet } from "../utils";
import { Field } from "./Field";
import styles from "./SelectField.css";

class SelectField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const field = (this.field = document.createElement("select"));
    field.setAttribute("class", "data-field");
    field.style.width = (options && options.width) || "fit-content";

    let index = 0;
    if (options.options) {
      for (const option of options.options) {
        const opt = document.createElement("option");
        opt.value = option.value || index;
        opt.innerHTML = option.text;

        field.appendChild(opt);
        index++;
      }
    }
    const dataLabel = document.createElement("span");
    dataLabel.setAttribute("class", "data-label");
    dataLabel.innerHTML = (options && options.label) || "";

    this.wrapper.appendChild(dataLabel);
    this.wrapper.appendChild(field);
  }

  get value() {

    return parseInt(this.field.value);
    // return this._value;
  }

  set value(new_value) {
    this.field.value = parseInt(new_value);
  }
}

customElements.define("select-field", SelectField);
export { SelectField };
