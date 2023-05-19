import { addStyleSheet } from "Base/utils";
import { Panel } from "Base/panel";
import { Field } from "Base/field";
import { Header } from "Base/header";
import { Sidebar } from "Base/sidebar";
import { addTooltip } from "Base/tooltip";
import styles from "./Page.css";

// const _globalValue = {};
// export var globalValue = new Proxy(_globalValue, {
//   set: function (target, key, value) {
//     console.log(`${key} set to ${value}`);
//     target[key] = value;
//     return true;
//   },
// });

import save from "Images/save.svg";
import cross from "Images/cross.svg";
class Page extends HTMLElement {
  constructor(options) {
    // Always call super first in constructor
    super();

    this.shadow = this.attachShadow({ mode: "open" });
    addStyleSheet(this.shadow, styles);

    const wrapper = (this.wrapper = document.createElement("div"));
    this.wrapper.setAttribute("class", "wrapper");

    const header = (this.header = new Header());

    const mainDiv = (this.mainDiv = document.createElement("div"));
    mainDiv.setAttribute("class", "main-div");

    const containerWrapper = (this.containerWrapper =
      document.createElement("div"));
    containerWrapper.setAttribute("class", "container-wrapper");

    // containerWrapper.onclick = () => {
    //   if (!sidebar.wrapper.classList.contains("closed")) {
    //     sidebar.hide();
    //   }
    // };

    //sidebar
    const sidebar = (this.sidebar = new Sidebar());

    sidebar.hide();
    header.buttonLeft.onclick = () => {
      sidebar.hide();
    };

    //save button
    this.saveContainer = document.createElement("div");
    this.saveContainer.classList.add("save-container", "hidden");
    this.saveContainer.addEventListener("contextmenu", (e) => {
      e.preventDefault();
    });

    const saveButton = document.createElement("button");
    saveButton.className = "save-button";
    saveButton.addEventListener("click", (e) => {
      if (options && options.configFile) {
        this.save(options.configFile);
      }
      if (options && options.saveFunc) {
        options.saveFunc(this);
      }
    });
    addTooltip({
      element: saveButton,
      img: save,
      msg: "Salvar as alterações realizadas no dispositivo.",
    });

    const saveButtonImg = document.createElement("div");
    saveButtonImg.style.setProperty("-webkit-mask-image", `url(${save})`);
    saveButtonImg.style.setProperty("mask-image", `url(${save})`);

    const saveButtonText = document.createElement("span");
    saveButtonText.innerHTML = "Salvar";

    const cancelButton = document.createElement("button");
    cancelButton.className = "cancel-button";
    cancelButton.addEventListener("click", (e) => {
      this.cancel();
    });
    addTooltip({
      element: cancelButton,
      img: cross,
      msg: "Cancelar as alterações realizadas.",
    });

    const cancelButtonImg = document.createElement("div");
    cancelButtonImg.style.setProperty("-webkit-mask-image", `url(${cross})`);
    cancelButtonImg.style.setProperty("mask-image", `url(${cross})`);
    //appends
    saveButton.appendChild(saveButtonImg);
    saveButton.appendChild(saveButtonText);
    cancelButton.appendChild(cancelButtonImg);

    this.saveContainer.appendChild(saveButton);
    this.saveContainer.appendChild(cancelButton);

    wrapper.appendChild(this.saveContainer);
    wrapper.appendChild(header);
    wrapper.appendChild(mainDiv);

    mainDiv.appendChild(sidebar);
    mainDiv.appendChild(containerWrapper);

    this.shadow.appendChild(this.wrapper);

    if (options && options.container) {
      options.container.appendChild(this);
    }

    this._value = {};

    if (options && options.configFile) {
      this.loadValue(options.configFile);
    }

    this._oldValue = {};

    document.addEventListener("changed", (evt) => {
      this.hideSaveContainer(
        JSON.stringify(this.value) === JSON.stringify(this._oldValue)
      );
    });
  }

  set value(value) {
    this.containerWrapper.querySelectorAll("*").forEach((element) => {
      if (element instanceof Panel && element.name) {
        element.value = value[element.name] || {};
      }
    });
  }

  get value() {
    for (const element of this.containerWrapper.querySelectorAll("*")) {
      if (element instanceof Panel) {
        if (element.value) {
          this._value[element.name] = element.value;
        } else {
          return null;
        }
      }
    }

    return this._value;
  }

  async save(file) {
    if (this.value != null) {
      console.log(JSON.stringify(this.value, null, 2));

      const response = await fetch(file, {
        method: "PUT",
        headers: {
          "Content-type": "application/json",
        },
        body: JSON.stringify(this.value, null, 2),
      });
    }
  }

  cancel() {
    this.value = this._oldValue;
  }

  async loadValue(file) {
    const response = await fetch(file, {
      method: "GET",
    });

    const value = await response.blob();

    this.value = value;
    this._oldValue = JSON.parse(JSON.stringify(this.value));
    // console.log(value);
  }

  hideSaveContainer(hide) {
    if (hide) {
      this.saveContainer.classList.add("hidden");
    } else {
      this.saveContainer.classList.remove("hidden");
    }
  }

  addContainer() {
    const container = document.createElement("div");
    container.className = "container";
    this.containerWrapper.appendChild(container);

    return container;
  }
}

customElements.define("base-page", Page);
export { Page };
