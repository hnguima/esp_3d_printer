import { Field } from "./Field";
import { addStyleSheet } from "../utils";
import styles from "./TimeField.css";

class TimeField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const field = (this.field = document.createElement("input"));
    field.classList.add("data-field");

    const dataLabel = (this.dataLabel = document.createElement("span"));
    dataLabel.setAttribute("class", "data-label");
    dataLabel.innerHTML = ((options && options.label) || null) + ":";

    this.wrapper.appendChild(dataLabel);
    this.wrapper.appendChild(field);
  }

  get value() {
    return this._date.getSeconds();
  }

  set value(new_value) {

    this._date = new Date(parseInt(new_value) * 1000);
    // this._date.setUTCSeconds(new_value);

    console.log(this._date);
    this.field.value = this._date.toLocaleString();
  }
}

customElements.define("time-field", TimeField);
export { TimeField };
