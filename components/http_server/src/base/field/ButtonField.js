import { addStyleSheet } from "../utils";
import { Field } from "./Field";
import styles from "./ButtonField.css";

class ButtonField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const dataLabel = document.createElement("span");
    dataLabel.classList.add("data-label");
    dataLabel.innerHTML = (options && options.label) + ":" || null;

    const field = (this.field = document.createElement("button"));
    field.classList.add("button");
    field.style.backgroundColor = (options && options.color) || null;
    field.onclick = () => {
      if (options && typeof options.callback === "function") {
        options.callback();
      }
    };

    const buttonImg = document.createElement("div");
    buttonImg.style.setProperty("-webkit-mask-image", `url(${options.img})`);
    buttonImg.style.setProperty("mask-image", `url(${options.img})`);

    field.appendChild(buttonImg);

    this.wrapper.appendChild(dataLabel);
    this.wrapper.appendChild(field);
  }
}

customElements.define("button-field", ButtonField);
export { ButtonField };
