type position = {
  x: int,
  y: int,
};

type size = {
  width: int,
  height: int,
};

type rect = {
  position,
  size,
};

type scaleFactor = {scale: float};

type imageLoadState =
  | ImageNotLoaded
  | ImageLoaded;

type imageKind =
  | ImagePortrait
  | ImageLandscape;

let inverseScaleFactor = (scaleFactor: scaleFactor): scaleFactor =>
  if (scaleFactor.scale == 0.0) {
    scaleFactor;
  } else {
    {scale: 1.0 /. scaleFactor.scale};
  };

let getStartCenterPositionSingleAxis = (size: int, maxSize: int): int =>
  if (size < maxSize) {
    let remaining: int = maxSize - size;
    remaining / 2;
  } else {
    0;
  };

let getStartCenterPosition = (size: size, maxSize: size): position => {
  x: getStartCenterPositionSingleAxis(size.width, maxSize.width),
  y: getStartCenterPositionSingleAxis(size.height, maxSize.height),
};

let getFullSizeScaleFactorWithAspectRatio =
    (canvasSize: size, imageSize: size): scaleFactor => {
  let widthRatio: float =
    float_of_int(canvasSize.width) /. float_of_int(imageSize.width);
  let heightRatio: float =
    float_of_int(canvasSize.height) /. float_of_int(imageSize.height);
  let scale: float =
    if (widthRatio
        *. float_of_int(imageSize.height) <= float_of_int(canvasSize.height)) {
      widthRatio;
    } else {
      heightRatio;
    };
  {scale: scale};
};

let scaleSingle = (length: int, scaleFactor: scaleFactor): int => {
  let lengthFloat = float_of_int(length);
  let scaledFloat = lengthFloat *. scaleFactor.scale;
  int_of_float(scaledFloat);
};

let scaleSize = (size: size, scaleFactor: scaleFactor): size => {
  width: scaleSingle(size.width, scaleFactor),
  height: scaleSingle(size.height, scaleFactor),
};

let drawCore = (canvas, image) => {
  let canvasSize: size = {width: canvas##width, height: canvas##height};
  let imageSize: size = {
    width: image##naturalWidth,
    height: image##naturalHeight,
  };

  let context = canvas##getContext("2d");
  context##clearRect(0, 0, canvasSize.width, canvasSize.height);

  let scaleFactor: scaleFactor =
    getFullSizeScaleFactorWithAspectRatio(canvasSize, imageSize);
  let scaledCanvasSize =
    scaleSize(canvasSize, inverseScaleFactor(scaleFactor));
  let centeredPosition = getStartCenterPosition(imageSize, scaledCanvasSize);

  context##save();
  context##imageSmoothingQuality #= "high";
  context##scale(scaleFactor, scaleFactor);
  context##drawImage(
    image,
    centeredPosition.x,
    centeredPosition.y,
    imageSize.width,
    imageSize.height,
  );
  context##restore();

  Js.log4("draw()", scaleFactor, imageSize, scaledCanvasSize);
};

let draw = (imageLoadState: imageLoadState, canvas, image) => {
  switch (imageLoadState) {
  | ImageNotLoaded => ()
  | ImageLoaded => drawCore(canvas, image)
  };
};

let portraitSource: string = "https://upload.wikimedia.org/wikipedia/commons/thumb/e/ec/Mona_Lisa%2C_by_Leonardo_da_Vinci%2C_from_C2RMF_retouched.jpg/1280px-Mona_Lisa%2C_by_Leonardo_da_Vinci%2C_from_C2RMF_retouched.jpg";
let landscapeSource: string = "https://upload.wikimedia.org/wikipedia/commons/d/d1/Kew_Gardens_Palm_House%2C_London_-_July_2009.jpg";

let getImageSource = (imageKind: imageKind) => {
  switch (imageKind) {
  | ImagePortrait => portraitSource
  | ImageLandscape => landscapeSource
  };
};

let switchImage = (imageKind: imageKind) => {
  switch (imageKind) {
  | ImagePortrait => ImageLandscape
  | ImageLandscape => ImagePortrait
  };
};

[@react.component]
let make = () => {
  let windowSize = WindowResize.useWindowResize();
  let (imageLoadState, setImageLoadState) =
    React.useState(_ => ImageNotLoaded);
  let (imageKind, setImageKind) = React.useState(_ => ImagePortrait);
  let canvasRef = React.useRef(Js.Nullable.null);
  let imageRef = React.useRef(Js.Nullable.null);
  let changeImage = _ => {
    setImageKind(switchImage);
    setImageLoadState(_ => ImageNotLoaded);
  };

  React.useEffect(() => {
    switch (
      Js.Nullable.toOption(canvasRef->React.Ref.current),
      Js.Nullable.toOption(imageRef->React.Ref.current),
    ) {
    | (Some(canvas), Some(image)) =>
      draw(
        imageLoadState,
        ReactDOMRe.domElementToObj(canvas),
        ReactDOMRe.domElementToObj(image),
      )
    | _ => ()
    };
    None;
  });
  <div>
    <button
      style={ReactDOMRe.Style.make(
        ~position="fixed",
        ~top="0",
        ~left="0",
        ~fontSize="40pt",
        (),
      )}
      onClick=changeImage>
      {ReasonReact.string("Switch image")}
    </button>
    <canvas
      width={string_of_int(windowSize.width)}
      height={string_of_int(windowSize.height)}
      ref={ReactDOMRe.Ref.domRef(canvasRef)}
    />
    <img
      src={getImageSource(imageKind)}
      style={ReactDOMRe.Style.make(~display="none", ())}
      ref={ReactDOMRe.Ref.domRef(imageRef)}
      onLoad={_ => setImageLoadState(_ => ImageLoaded)}
    />
  </div>;
};