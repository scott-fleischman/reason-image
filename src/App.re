let draw = (canvas, image) => {
  let canvasWidth = canvas##width;
  let canvasHeight = canvas##height;
  let min50 = value => max(50, value - 50);
  let rectWidth = min50(canvasWidth);
  let rectHeight = min50(canvasHeight);
  let context = canvas##getContext("2d");
  context##clearRect(0, 0, canvasWidth, canvasHeight);
  context##drawImage(image, 0, 0);

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

let imageSource: string = "https://upload.wikimedia.org/wikipedia/commons/thumb/e/ec/Mona_Lisa%2C_by_Leonardo_da_Vinci%2C_from_C2RMF_retouched.jpg/1280px-Mona_Lisa%2C_by_Leonardo_da_Vinci%2C_from_C2RMF_retouched.jpg";

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
  let imageRef = React.useRef(Js.Nullable.null);
  let drawOnCanvas = () =>
    switch (
      Js.Nullable.toOption(canvasRef->React.Ref.current),
      Js.Nullable.toOption(imageRef->React.Ref.current),
    ) {
    | (Some(canvas), Some(image)) =>
      draw(ReactDOMRe.domElementToObj(canvas), image)
    | _ => ()
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
  <div>
    <canvas
      width={string_of_int(windowSize.width)}
      height={string_of_int(windowSize.height)}
      ref={ReactDOMRe.Ref.domRef(canvasRef)}
    />
    <img
      src=imageSource
      style={ReactDOMRe.Style.make(~display="none", ())}
      ref={ReactDOMRe.Ref.domRef(imageRef)}
    />
  </div>;
};