import { addStyleSheet, changedEvent } from "../utils";
import { Field } from "./Field";
import styles from "./PasswordField.css";

import eye from "Images/eye.svg";

class PasswordField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const passwordDiv = document.createElement("div");

    const passwordLabel = document.createElement("span");
    passwordLabel.className = "data-label";
    passwordLabel.innerHTML = "Nova senha";

    const password = (this.field = document.createElement("input"));
    password.className = "data-field";
    password.type = "password";

    const passwordEye = document.createElement("div");
    passwordEye.className = "password-eye";
    // passwordEye.src = "Images/eye.svg";
    passwordEye.style.setProperty("-webkit-mask-image", `url(${eye})`);
    passwordEye.style.setProperty("mask-image", `url(${eye})`);

    passwordEye.onclick = () => {
      revealField(password);
    };

    const repeatDiv = document.createElement("div");

    const repeatLabel = document.createElement("span");
    repeatLabel.className = "data-label";
    repeatLabel.innerHTML = "Repita a senha:";

    const repeat = (this.repeat = document.createElement("input"));
    repeat.className = "data-field";
    repeat.type = "password";
    repeat.oninput = () => {
      if (this.isValid()) {
        this.dispatchEvent(changedEvent);
      }
    };

    const repeatEye = document.createElement("div");
    repeatEye.className = "password-eye";
    // repeatEye.src = "${eye}";
    repeatEye.style.setProperty("-webkit-mask-image", `url(${eye})`);
    repeatEye.style.setProperty("mask-image", `url(${eye})`);

    repeatEye.onclick = () => {
      revealField(repeat);
    };

    passwordDiv.appendChild(passwordLabel);
    passwordDiv.appendChild(password);
    passwordDiv.appendChild(passwordEye);

    repeatDiv.appendChild(repeatLabel);
    repeatDiv.appendChild(repeat);
    repeatDiv.appendChild(repeatEye);

    this.wrapper.appendChild(passwordDiv);
    this.wrapper.appendChild(repeatDiv);
  }

  get value() {
    return super.value;
  }

  set value(value) {
    this.field.value = value;
    this.repeat.value = value;
  }

  isValid() {
    if (this.field === undefined) {
      return false;
    }
    if (this.field.value.length < 8) {
      console.error("launch popup too small");
      return false;
    }
    if (!(this.field.value === this.repeat.value)) {
      console.error("launch popup different password");
      return false;
    }

    return true;
  }
}

function revealField(field) {
  if (field.type == "password") {
    field.type = "text";
  } else {
    field.type = "password";
  }
}

customElements.define("password-field", PasswordField);
export { PasswordField };
