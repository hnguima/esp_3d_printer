import "preact/debug";
import { debug, h, Component, render } from "preact";

import { TextField } from "Components/Field";
import { Panel } from "Components/Panel";
import { FileExplorer } from "Components/FileExplorer";
import "./Fonts.css";
import wifiIcon from "Images/wifi.svg";
import webIcon from "Images/web.svg";

const App = () => {
  return (
    <div>
      <h1>cubic one</h1>
      <FileExplorer label={"field"} placeholder={"simple field"} />
      {/* <TextField label={"field inline"} placeholder={"inline"} inline />
      <TextField label={"field read only"} placeholder={"read only"} readonly />
      <TextField
        label={"field inline read only"}
        placeholder={"inline read only"}
        width={200}
        inline
        readonly
      />

      <Panel label="Test Panel" icon={wifiIcon} switchable>
        <TextField label={"aaaaaaaaaaaaaaaaaa"} placeholder={"simple field"} />
        <TextField label={"field"} placeholder={"simple field"} />
      </Panel>

      <Panel label="web Panel" icon={webIcon}>
        <TextField label={"aaaaaaaaaaaaaaaaaa"} placeholder={"simple field"} />
        <TextField label={"field"} placeholder={"simple field"} />
      </Panel> */}
    </div>
  );
};

render(App(), document.body);
