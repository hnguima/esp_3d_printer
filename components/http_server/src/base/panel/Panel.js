import { Field } from "Field";
import { addStyleSheet } from "../utils";
import styles from "./Panel.css";

class Panel extends HTMLElement {
  constructor(options) {
    // Always call super first in constructor
    super();

    this.shadow = this.attachShadow({ mode: "open" });
    addStyleSheet(this.shadow, styles);

    this.wrapper = document.createElement("div");
    this.wrapper.className = "wrapper";

    const titleDiv = (this.titleDiv = document.createElement("div"));
    titleDiv.className = "title-div";
    this.wrapper.appendChild(titleDiv);

    this.shadow.appendChild(this.wrapper);

    this._value = {};
    this._fields = {};

    this.tab = (options && options.tab) || null;
  }

  set title(val) {
    this.titleH2 = document.createElement("h2");
    this.titleH2.innerHTML = val;
    this.titleDiv.appendChild(this.titleH2);
  }

  set icon(val) {
    this.iconImg = document.createElement("div");
    // this.iconImg.src = val;

    this.iconImg.style.setProperty("-webkit-mask-image", `url(${val})`);
    this.iconImg.style.setProperty("mask-image", `url(${val})`);
    this.titleDiv.prepend(this.iconImg);
  }

  get value() {
    if (this.isValid()) {
      this.updateValue();
      return this._value;
    } else {
      return null;
    }
  }

  updateValue() {
    for (const field in this._fields) {
      this._value[this._fields[field].name] = this._fields[field].value;
    }
  }

  set value(val) {
    this._value = val;
    this.updateFields(val);
  }

  updateFields(val) {
    for (const prop in val) {
      if (this._fields[prop]) {
        this._fields[prop].value = val[prop];

      }
    }
  }

  isValid() {
    let isValid = true;
    for (const field in this._fields) {
      if (!this._fields[field].isValid()) {
        isValid = false;
      }
    }

    return isValid;
  }

  addField(field, name) {
    if (name !== "" && name !== undefined) {
      this._fields[name] = field;
      field.name = name;
      field.setupEventListener();
      // this._value[name] = field.value;
    }

    this.wrapper.appendChild(field);
  }
}

customElements.define("base-panel", Panel);
export { Panel };
