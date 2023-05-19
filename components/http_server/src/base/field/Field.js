import styles from "./Field.css";
import { addStyleSheet, changedEvent } from "../utils";

export class Field extends HTMLElement {
  constructor(options) {
    // Always call super first in constructor
    super();
    this.shadow = this.attachShadow({ mode: "open" });
    addStyleSheet(this.shadow, styles);

    this.wrapper = document.createElement("div");
    this.wrapper.setAttribute("class", "wrapper");
    // this.wrapper.className = styles["wrapper"];

    this.shadow.appendChild(this.wrapper);

    this.name = (options && options.name) || "";
    if (options && options.panel) {
      // options.container.appendChild(this);
      options.panel.addField(this, this.name);
    }

    this.options = options;
  }

  setupEventListener() {
    (async () => {
      while (!this.field) {
        await new Promise((resolve) => setTimeout(resolve, 1000));
      }

      this.field.addEventListener("input", () => {
        if (this.isValid()) {
          this.dispatchEvent(changedEvent);
        }
      });

      if (this.options && this.options.readonly == true) {
        this.field.classList.add("read-only");
      }
      if (this.options && this.options.inline == true) {
        this.wrapper.classList.add("inline");
        this.field.classList.add("inline");
      }
    })();
  }

  get value() {
    if (this.isValid() && this.field) {
      return this.field.value;
    } else {
      return null;
    }
  }

  set value(val) {
    this.field.value = val;
  }

  get label() {
    return this.dataLabel.innerHTML;
  }

  set label(val) {
    this.dataLabel.innerHTML = val + ":";
  }

  isValid() {
    return true;
  }
}

customElements.define("base-field", Field);
