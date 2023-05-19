import { addStyleSheet } from "../utils";
import { Field } from "./Field";
import { Switch } from "../switch/Switch";
import styles from "./SwitchField.css";

class SwitchField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const dataLabel = (this.dataLabel = document.createElement("span"));
    dataLabel.setAttribute("class", "data-label");
    dataLabel.innerHTML = ((options && options.label) || null) + ":";
    const field = (this.field = new Switch());

    this.wrapper.appendChild(dataLabel);
    this.wrapper.appendChild(field);
  }
}

customElements.define("switch-field", SwitchField);
export { SwitchField };
