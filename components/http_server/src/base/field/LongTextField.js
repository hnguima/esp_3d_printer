import { Field } from "./Field";
import { addStyleSheet, changedEvent } from "../utils";
import styles from "./LongTextField.css";

import { addTooltip } from "Base/tooltip";

import pencil from "Images/pencil.svg";
import check from "Images/check.svg";

export class LongTextField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const field = (this.field = document.createElement("textarea"));
    field.classList.add("data-field", "inactive");
    field.placeholder = (options && options.placeholder) || null;
    field.readOnly = true;

    field.addEventListener("input", () => {
      if (field.value.length > 300) {
        field.value = field.value.substring(0, 300);
      }
      field.style.height = field.scrollHeight + "px";
    });

    const tooltipOptions = {
      element: field,
      img: pencil,
      msg: "Clique o icone do lápis para alterar a descrição do dispositivo",
      
    }
    addTooltip(tooltipOptions);

    const editBtn = document.createElement("div");
    editBtn.className = "edit-btn";

    editBtn.style.setProperty("-webkit-mask-image", `url(${pencil})`);
    editBtn.style.setProperty("mask-image", `url(${pencil})`);

    editBtn.addEventListener("click", () => {
      if (field.readOnly) {
        field.readOnly = false;
        field.classList.remove("inactive");

        editBtn.style.backgroundColor = "var(--accept-color)"
        editBtn.style.setProperty("-webkit-mask-image", `url(${check})`);
        editBtn.style.setProperty("mask-image", `url(${check})`);

        tooltipOptions.disabled = true;
      } else {
        field.readOnly = true;
        field.classList.add("inactive");

        editBtn.style.backgroundColor = "var(--accent-color)";
        editBtn.style.setProperty("-webkit-mask-image", `url(${pencil})`);
        editBtn.style.setProperty("mask-image", `url(${pencil})`);

        tooltipOptions.disabled = false;
      }
    });

    if (options && options.label) {
      const dataLabel = (this.dataLabel = document.createElement("span"));
      dataLabel.setAttribute("class", "data-label");
      dataLabel.innerHTML = options.label + ":";
      this.wrapper.appendChild(dataLabel);
    }

    this.wrapper.appendChild(field);
    this.wrapper.appendChild(editBtn);
  }

  get placeholder() {
    return this.field.placeholder;
  }

  set placeholder(val) {
    this.field.setAttribute("placeholder", val);
  }
}

customElements.define("long-text-field", LongTextField);
