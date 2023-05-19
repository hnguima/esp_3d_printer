import { Panel } from "Panel";
import { TextField, PasswordField } from "Field";

import icon from "Images/person.svg";

export class LoginPanel extends Panel {
  constructor() {
    // Always call super first in constructor
    super();

    this.name = "login";
    this.tab = "info";

    this.title = "Login";
    this.icon = icon;

    const username = new TextField();
    username.label = "Usuário";
    username.placeholder = "Digite um nome de usuário";

    const password = new PasswordField();

    this.addField(username, "username");
    this.addField(password, "password");
  }
}

customElements.define("login-panel", LoginPanel);
