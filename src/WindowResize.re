type windowSize = {
  width: int,
  height: int,
};

[@bs.val] external window: Dom.window = "";
external windowToJsObj: Dom.window => Js.t({..}) = "%identity";

[@bs.send]
external addEventListener:
  (Dom.window, string, Dom.event_like('a) => unit) => unit =
  "";

[@bs.send]
external removeEventListener:
  (Dom.window, string, Dom.event_like('a) => unit) => unit =
  "";

let getWindowSize = () => {
  let window = windowToJsObj(window);
  let width = window##innerWidth;
  let height = window##innerHeight;
  {width, height};
};

let useWindowResize = () => {
  let (windowSize, setWindowSize) = React.useState(getWindowSize);
  React.useEffect(() => {
    let handleResize = _ => {
      setWindowSize(_ => getWindowSize());
    };
    addEventListener(window, "resize", handleResize);
    Some(() => removeEventListener(window, "resize", handleResize));
  });
  windowSize;
};