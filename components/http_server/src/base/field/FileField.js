import { addStyleSheet } from "../utils";
import { Field } from "./Field";
import styles from "./FileField.css";

class FileField extends Field {
  constructor(options) {
    // Always call super first in constructor
    super(options);
    addStyleSheet(this.shadow, styles);

    const fileDropArea = document.createElement("div");
    fileDropArea.className = "file-drop-area";

    const fileImg = document.createElement("div");
    // fileImg.src = (options && options.img) || "";

    fileImg.style.setProperty("-webkit-mask-image", `url(${options.img})`);
    fileImg.style.setProperty("mask-image", `url(${options.img})`);

    const fileMsg = (this.fileMsg = document.createElement("span"));
    fileMsg.innerHTML = (options && options.msg) || null;

    const field = (this.field = document.createElement("input"));
    field.setAttribute("class", "data-field");
    field.type = "file";
    field.accept = (options && options.formats).join(",");

    fileDropArea.appendChild(fileImg);
    fileDropArea.appendChild(fileMsg);
    fileDropArea.appendChild(field);

    this.file;

    field.onchange = () => {
      this.file = this.field.files[0];
      this.fileMsg.innerHTML = `Arquivo: ${this.file.name}<br>Tamanho: ${
        this.file.size
      } bytes (${this.file.size / 1000} Kb)`;
      button.classList.remove("hide");
    };

    const dataLabel = document.createElement("span");
    dataLabel.classList.add("data-label");
    dataLabel.innerHTML = (options && options.label) + ":" || null;

    const button = (this.button = document.createElement("button"));
    button.classList.add("button");
    button.classList.add("hide");
    button.innerHTML = (options && options.buttonMsg) || null;
    button.onclick = () => {
      if (options && typeof options.callback === "function") {
        options.callback();
      }
    };

    this.wrapper.appendChild(dataLabel);
    this.wrapper.appendChild(fileDropArea);
    this.wrapper.appendChild(button);
  }
}

customElements.define("file-field", FileField);
export { FileField };
