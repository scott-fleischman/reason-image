type position = {
  x: int,
  y: int,
};

type imageSize = {
  width: int,
  height: int,
};

type imageScale = {
  position,
  imageSize,
};

let centerFitImage = (canvasSize: imageSize, imageSize: imageSize): imageScale => {
  let widthHeightRatio =
    float_of_int(imageSize.width) /. float_of_int(imageSize.height);
  let heightWidthRatio =
    float_of_int(imageSize.height) /. float_of_int(imageSize.width);
  let widthScaledSize = {
    width: canvasSize.width,
    height: int_of_float(heightWidthRatio *. float_of_int(canvasSize.width)),
  };
  let heightScaledSize = {
    width: int_of_float(widthHeightRatio *. float_of_int(canvasSize.height)),
    height: canvasSize.height,
  };
  let scaledSize =
    if (widthScaledSize.height <= canvasSize.height) {
      widthScaledSize;
    } else {
      heightScaledSize;
    };
  let position: position = {x: 0, y: 0};
  {position, imageSize: scaledSize};
};

let draw = (canvas, image) => {
  let canvasWidth: int = canvas##width;
  let canvasHeight: int = canvas##height;

  let imageWidth: int = image##naturalWidth;
  let imageHeight: int = image##naturalHeight;

  let widthHeightRatio =
    float_of_int(imageWidth) /. float_of_int(imageHeight);

  let context = canvas##getContext("2d");
  context##clearRect(0, 0, canvasWidth, canvasHeight);

  let imageScale =
    centerFitImage(
      {width: canvasWidth, height: canvasHeight},
      {width: imageWidth, height: imageHeight},
    );
  context##drawImage(
    image,
    imageScale.position.x,
    imageScale.position.y,
    imageScale.imageSize.width,
    imageScale.imageSize.height,
  );

  print_endline(
    "draw(width:"
    ++ string_of_int(canvasWidth)
    ++ ",height:"
    ++ string_of_int(canvasHeight)
    ++ "). image:"
    ++ string_of_int(imageWidth)
    ++ "x"
    ++ string_of_int(imageHeight)
    ++ ", aspect:"
    ++ Js.Float.toString(widthHeightRatio),
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
      draw(
        ReactDOMRe.domElementToObj(canvas),
        ReactDOMRe.domElementToObj(image),
      )
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