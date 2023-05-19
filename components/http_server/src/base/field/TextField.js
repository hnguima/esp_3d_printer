import { Field } from "./Field";
import { addStyleSheet } from "../utils";
import styles from "./TextField.css";


class TextField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const field = (this.field = document.createElement("input"));
    field.classList.add("data-field");
    
    field.placeholder = (options && options.placeholder) || null;
    
    const dataLabel = (this.dataLabel = document.createElement("span"));
    dataLabel.setAttribute("class", "data-label");
    dataLabel.innerHTML = ((options && options.label) || null) + ":";

    this.wrapper.appendChild(dataLabel);
    this.wrapper.appendChild(field);
  }

  get placeholder() {
    return this.field.placeholder;
  }

  set placeholder(val) {
    this.field.setAttribute("placeholder", val);
  }
}

customElements.define("text-field", TextField);
export { TextField };
