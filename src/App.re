let draw = canvas => {
  let canvasWidth = canvas##width;
  let canvasHeight = canvas##height;
  let min50 = value => max(50, value - 50);
  let rectWidth = min50(canvasWidth);
  let rectHeight = min50(canvasHeight);
  let context = canvas##getContext("2d");
  context##clearRect(0, 0, canvasWidth, canvasHeight);

  context##fillStyle #= "rgb(200, 0, 0)";
  context##fillRect(10, 10, rectWidth, rectHeight);

  context##fillStyle #= "rgba(0, 0, 200, 0.5)";
  context##fillRect(30, 30, rectWidth, rectHeight);
  print_endline(
    "draw(width:"
    ++ string_of_int(canvasWidth)
    ++ ",height:"
    ++ string_of_int(canvasHeight)
    ++ ")",
  );
};

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

[@react.component]
let make = () => {
  let (windowSize, setWindowSize) = React.useState(getWindowSize);
  let canvasRef = React.useRef(Js.Nullable.null);
  let drawOnCanvas = () =>
    switch (Js.Nullable.toOption(canvasRef->React.Ref.current)) {
    | None => ()
    | Some(canvas) => draw(ReactDOMRe.domElementToObj(canvas))
    };

  React.useEffect(() => {
    drawOnCanvas();
    None;
  });
  React.useEffect(() => {
    let handleResize = _ => {
      setWindowSize(_ => getWindowSize());
    };
    addEventListener(window, "resize", handleResize);
    Some(() => removeEventListener(window, "resize", handleResize));
  });
  <canvas
    width={string_of_int(windowSize.width)}
    height={string_of_int(windowSize.height)}
    ref={ReactDOMRe.Ref.domRef(canvasRef)}
  />;
};