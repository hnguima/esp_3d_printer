import { Panel } from "Panel";
import { FileField, TextField } from "Field";

// const { createHash } = require("crypto");
// const { createHash } = import("node:crypto");
import { sha256 } from "js-sha256";

import icon from "Images/upload.svg";
import save from "Images/save.svg";

export class FirmwarePanel extends Panel {
  constructor() {
    // Always call super first in constructor
    super();

    this.name = "firmware";

    this.title = "Firmware";
    this.icon = icon;

    new TextField({
      inline: true,
      readonly: true,
      panel: this,
      name: "fwVersion",
      label: "Versão de Firmware",
    });

    new TextField({
      inline: true,
      readonly: true,
      panel: this,
      name: "fwTimestamp",
      label: "Data da atualização",
    });

    const firmware = new FileField({
      panel: this,
      label: "Atualização de Firmware",
      img: save,
      msg: "Escolha um binário para fazer a atualização ou solte-o aqui.",
      formats: [".bin"],
      buttonMsg: "Atualizar",
      callback: async () => {
        if (
          confirm(
            "Tem certeza que deseja fazer a atualização? Esta operação pode danificar seu dispositivo."
          )
        ) {
          console.log(firmware.file);

          const file_data = await firmware.file.arrayBuffer();

          var hash = sha256.create();
          hash.update(file_data);
          console.log(hash.hex());

          const response = await fetch("update_firmware", {
            method: "PUT",
            body: file_data,
            headers: { Hash: hash.hex() },
          });

          const responseText = await response.text();

          if (responseText == "Success") {
            location.reload();
          }
        }
      },
    });
  }
}

customElements.define("firmware-panel", FirmwarePanel);
