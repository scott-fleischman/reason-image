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

let getFillSizeWithAspectRatio = (canvasSize: size, imageSize: size): size => {
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
  scaledSize;
};

let centerFitImage = (canvasSize: size, imageSize: size): rect => {
  let filledSize = getFillSizeWithAspectRatio(canvasSize, imageSize);
  let position: position = getStartCenterPosition(filledSize, canvasSize);
  {position, size: filledSize};
};

type imageLoadState =
  | ImageNotLoaded
  | ImageLoaded;

let drawCore = (canvas, image) => {
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
    imageScale.size.width,
    imageScale.size.height,
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

let draw = (imageLoadState: imageLoadState, canvas, image) => {
  switch (imageLoadState) {
  | ImageNotLoaded => ()
  | ImageLoaded => drawCore(canvas, image)
  };
};

let imageSource: string = "https://upload.wikimedia.org/wikipedia/commons/thumb/e/ec/Mona_Lisa%2C_by_Leonardo_da_Vinci%2C_from_C2RMF_retouched.jpg/1280px-Mona_Lisa%2C_by_Leonardo_da_Vinci%2C_from_C2RMF_retouched.jpg";

[@react.component]
let make = () => {
  let windowSize = WindowResize.useWindowResize();
  let (imageLoadState, setImageLoadState) =
    React.useState(_ => ImageNotLoaded);
  let canvasRef = React.useRef(Js.Nullable.null);
  let imageRef = React.useRef(Js.Nullable.null);

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
    <canvas
      width={string_of_int(windowSize.width)}
      height={string_of_int(windowSize.height)}
      ref={ReactDOMRe.Ref.domRef(canvasRef)}
    />
    <img
      src=imageSource
      style={ReactDOMRe.Style.make(~display="none", ())}
      ref={ReactDOMRe.Ref.domRef(imageRef)}
      onLoad={_ => setImageLoadState(_ => ImageLoaded)}
    />
  </div>;
};