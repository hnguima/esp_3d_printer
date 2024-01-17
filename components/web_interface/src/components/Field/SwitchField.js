import { h, Component, render } from "preact";
import styles from "./Field.module.css";

export function SwitchField() {
  return (
    <div
      className={`${styles.field} 
      ${inline ? styles.inline : ""} 
      ${readonly ? styles.readonly : ""}`}
    >
      <h2 className={styles.label}>{label}</h2>
      <input
        className={styles.data}
        style={`${inline ? `width: ${width}px` : ""}`}
        placeholder={placeholder}
      ></input>
    </div>
  );
}
