export function addStyleSheet(dst, styleSheet) {
  const newStyle = document.createElement("style");
  newStyle.innerHTML = styleSheet.toString();

  let styleList = dst.querySelectorAll("style");
  if (styleList.length > 0) {
    styleList[styleList.length - 1].after(newStyle);
  } else {
    dst.appendChild(newStyle);
  }
}

export const changedEvent = new Event("changed", {
  bubbles: true,
  composed: true,
});

// setTimeout(() => {
//   console.log("dispatching");
//   document.dispatchEvent(changedEvent);
// }, 1000);
