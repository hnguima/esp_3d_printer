import { Panel } from "Panel";
import { FileField, ButtonField } from "Field";

import icon from "Images/upload.svg";
import save from "Images/save.svg";
import download from "Images/download.svg";
import undo from "Images/undo.svg";
import factory from "Images/factory.svg";

import { default as root } from "App/proto.pb.js";

export class ConfigPanel extends Panel {
  constructor() {
    // Always call super first in constructor
    super();

    this.name = "config-update";

    this.title = "Configuração";
    this.icon = icon;

    const currentConfigBtn = new ButtonField({
      panel: this,
      label: "Baixar configuração atual",
      img: download,
      color: "var(--accent-color)",
      callback: async () => {
        const response = await fetch("settings", {
          method: "GET",
        });

        const valueArrayBuffer = await response.arrayBuffer();
        var value = new Uint8Array(valueArrayBuffer);

        var message = root.MainConfig.decode(value);

        var file = new File(
          [JSON.stringify(message, null, "\t")],
          "config.json"
        );

        // download file
        let hiddenElement = document.createElement("a");
        hiddenElement.href = window.URL.createObjectURL(file);
        hiddenElement.download = "config.json";
        hiddenElement.click();
      },
    });

    const lastConfigBtn = new ButtonField({
      panel: this,
      label: "Baixar configuração anterior",
      img: undo,
      color: "var(--accent-color)",
      callback: async () => {
        const response = await fetch("settings", {
          method: "GET",
          headers: { Type: "old" },
        });

        const valueArrayBuffer = await response.arrayBuffer();
        var value = new Uint8Array(valueArrayBuffer);

        var message = root.MainConfig.decode(value);

        var file = new File(
          [JSON.stringify(message, null, "\t")],
          "config_backup.json"
        );

        // download file
        let hiddenElement = document.createElement("a");
        hiddenElement.href = window.URL.createObjectURL(file);
        hiddenElement.download = "config_backup.json";
        hiddenElement.click();
      },
    });

    const factoryConfigBtn = new ButtonField({
      panel: this,
      label: "Baixar configuração de fábrica",
      img: factory,
      color: "var(--accent-color)",
      callback: async () => {
        const response = await fetch("settings", {
          method: "GET",
          headers: { Type: "default" },
        });

        const valueArrayBuffer = await response.arrayBuffer();
        var value = new Uint8Array(valueArrayBuffer);

        var message = root.MainConfig.decode(value);

        var file = new File(
          [JSON.stringify(message, null, "\t")],
          "config_fabrica.json"
        );

        // download file
        let hiddenElement = document.createElement("a");
        hiddenElement.href = window.URL.createObjectURL(file);
        hiddenElement.download = "config_fabrica.json";
        hiddenElement.click();
      },
    });

    new ButtonField({
      panel: this,
      label: "Reverter para firmware de fábrica",
      img: factory,
      color: "crimson",
      callback: () => {
        console.log(this.value);
        alert("hello from button");
      },
    });

    const configFile = new FileField({
      panel: this,
      label: "Atualizar Configuração",
      img: save,
      msg: "Escolha um .json para fazer a atualização ou solte-o aqui.",
      formats: [".json"],
      buttonMsg: "Atualizar",
      callback: async () => {
        if (
          confirm(
            "Tem certeza que deseja fazer a atualização? Esta operação pode danificar seu dispositivo."
          )
        ) {
          const file_data = JSON.parse(await configFile.file.text());
          console.log(file_data);

          var errMsg = root.MainConfig.verify(file_data);
          if (errMsg) throw Error(errMsg);

          var message = root.MainConfig.create(file_data);

          var buffer = root.MainConfig.encode(message).finish();
          console.log(buffer);

          const response = await fetch("settings", {
            method: "PUT",
            body: buffer,
          });

          const responseText = await response.text();
          console.log(responseText);

          if (responseText == "Success") {
            location.reload();
          }
        }
      },
    });

    // this.addField(currentConfigBtn);
    // this.addField(lastConfigBtn);
    // this.addField(factoryConfigBtn);
    // this.addField(configFile);
  }
}

customElements.define("config-panel", ConfigPanel);
