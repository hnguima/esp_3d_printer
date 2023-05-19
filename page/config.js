import { Page } from "Page";
import { addStyleSheet } from "Base/utils";
import { addTooltip } from "Tooltip";

import { ChannelPanel } from "./panels";

// import * as Panels from "./panels";
import {
  LoginPanel,
  InfoPanel,
  FirmwarePanel,
  ConfigPanel,
  GeneralPanel,
  EthernetPanel,
  WifiPanel,
  ServerPanel,
} from "Panel";

import colors from "Styles/colors.css";
import fonts from "Styles/fonts.css";

import info from "Images/info.svg";
import web from "Images/web.svg";
import channel from "Images/channel.svg";
import tools from "Images/tools.svg";

import { default as root } from "./proto.pb.js";
// import config from "./config.json";
// import defaultConfig from "./default.json";

addStyleSheet(document.body, colors);
addStyleSheet(document.body, fonts);

const saveSettings = async () => {
  if (page.value != null) {
    console.log(JSON.stringify(page.value, null, 2));

    const payload = page.value;
    var errMsg = root.MainConfig.verify(payload);
    if (errMsg) throw Error(errMsg);

    var message = root.MainConfig.create(payload);

    var buffer = root.MainConfig.encode(message).finish();
    console.log(buffer);

    const response = await fetch("settings", {
      method: "PUT",
      body: buffer,
    });

    const responseText = await response.text();

    if(responseText == "Success") {
      // location.reload();
    }
  }
};

document.body.classList.add("light-mode");

// const header = new Header({ container: document.body });
const page = new Page({
  container: document.body,
  saveFunc: saveSettings,
});

page.header.buttonCenter.addEventListener("click", (e) => {
  location.href = "index.html";
});

const tabs = {
  info: {
    name: "Info",
    container: page.addContainer(),
    img: info,
    active: true,
  },
  net: {
    name: "Redes",
    container: page.addContainer(),
    img: web,
  },
  channel: {
    name: "Canais",
    container: page.addContainer(),
    img: channel,
  },
  tools: {
    name: "Ferramentas",
    container: page.addContainer(),
    img: tools,
  },
};

for (const tab in tabs) {
  page.sidebar.appendTab(
    tabs[tab].name,
    tabs[tab].img,
    tabs[tab].container,
    tabs[tab].active
  );
}

tabs["info"].container.appendChild(new InfoPanel());
tabs["info"].container.appendChild(new LoginPanel());

tabs["net"].container.appendChild(new EthernetPanel());
tabs["net"].container.appendChild(new WifiPanel());
// tabs["net"].container.appendChild(new ServerPanel());

tabs["channel"].container.appendChild(
  new ChannelPanel({
    name: "channel_1",
    title: "Canal 1",
  })
);
tabs["channel"].container.appendChild(
  new ChannelPanel({
    name: "channel_2",
    title: "Canal 2",
  })
);

tabs["tools"].container.appendChild(new GeneralPanel());
tabs["tools"].container.appendChild(new FirmwarePanel());
tabs["tools"].container.appendChild(new ConfigPanel());

(async () => {
  const response = await fetch("settings", {
    method: "GET",
  });

  const valueArrayBuffer = await response.arrayBuffer();
  var value = new Uint8Array(valueArrayBuffer);

  var message = root.MainConfig.decode(value);
  console.log(message);

  page.value = message;
  page._oldValue = JSON.parse(JSON.stringify(page.value));
})();
