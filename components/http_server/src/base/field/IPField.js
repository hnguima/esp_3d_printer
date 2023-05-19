import { addStyleSheet, changedEvent } from "../utils";
import { Field } from "./Field";
import styles from "./IPField.css";

export class IPField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const ipWrapper = document.createElement("div");
    ipWrapper.setAttribute("class", "ip-wrapper");

    const ipArr = (this.ipArr = []);

    for (var i = 0; i < 4; i++) {
      let ipField = document.createElement("input");
      ipField.setAttribute("class", "data-field");
      ipField.value = 0;

      ipField.onclick = () => {
        ipField.select();
      };

      ipWrapper.appendChild(ipField);
      this.ipArr.push(ipField);
    }

    this.field = {
      value: "",
      addEventListener(type, func) {
        ipArr.forEach((element) => {
          element.addEventListener(type, func);
        });
      },
    };

    const dataLabel = (this.dataLabel = document.createElement("span"));
    dataLabel.setAttribute("class", "data-label");
    dataLabel.innerHTML = (options && options.label) || null;

    this.wrapper.appendChild(dataLabel);
    this.wrapper.appendChild(ipWrapper);
  }

  get value() {
    if (this.isValid()) {
      return this.field.value;
    } else {
      return null;
    }
  }

  set value(val) {
    this.field.value = val;

    if (val) {
      val.split(".").forEach((element, index) => {
        this.ipArr[index].value = element;
      });

      // this.ipArr.forEach((field) => {
      //   field.value = val[index++];
      // });
    }
  }

  isValid() {
    this.field.value = "";
    let valid = false;

    this.ipArr.every((field) => {
      field.value = field.value.replace(/\D+/g, "");

      field.value = parseInt(field.value);
      if (field.value === "NaN") {
        field.value = "";
      }
      if (parseInt(field.value) >= 255) {
        field.value = 255;
      }

      if (field.value == "") {
        console.error("launch popup empty ip");
        this.field.value = "";
        this.value = null;

        valid = false;
        return false;
      }

      this.field.value += field.value + ".";

      valid = true;
      return true;
    });

    this.field.value = this.field.value.substring(
      0,
      this.field.value.length - 1
    );

    return valid;
  }
}

customElements.define("ip-field", IPField);
