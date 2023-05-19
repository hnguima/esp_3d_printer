import { addStyleSheet, changedEvent } from "../utils";
import styles from "./Switch.css" assert { type: "css" };

class Switch extends HTMLElement {
  constructor() {
    // Always call super first in constructor
    super();

    const shadow = this.attachShadow({ mode: "open" });
    addStyleSheet(shadow, styles);

    const switchField = document.createElement("label");
    switchField.setAttribute("class", "switch");

    const slider = document.createElement("div");
    slider.setAttribute("class", "slider");

    this.checkbox = document.createElement("input");
    this.checkbox.setAttribute("class", "data-field");
    this.checkbox.type = "checkbox";

    switchField.appendChild(this.checkbox);
    switchField.appendChild(slider);

    shadow.appendChild(switchField);

    this._value = this.checkbox.checked;
    this.checkbox.onclick = () => {
      this._value = this.checkbox.checked;
    };
  }

  setupEventListener() {
    this.addEventListener("input", () => {
      if (this.isValid()) {
        this.dispatchEvent(changedEvent);
      }
    });
  }

  isValid() {
    return true;
  }

  get value() {
    return this._value;
  }

  set value(val) {
    this.checkbox.checked = val;
    this._value = val;
  }
}

customElements.define("base-switch", Switch);
export { Switch };
