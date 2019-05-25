type position = {
  x: float,
  y: float,
};

type size = {
  width: float,
  height: float,
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

let getStartCenterPositionSingleAxis = (size: float, maxSize: float): float =>
  if (size < maxSize) {
    let remaining: float = maxSize -. size;
    remaining /. 2.0;
  } else {
    0.0;
  };

let getStartCenterPosition = (size: size, maxSize: size): position => {
  x: getStartCenterPositionSingleAxis(size.width, maxSize.width),
  y: getStartCenterPositionSingleAxis(size.height, maxSize.height),
};

// Get the scale factor to make the image fill the canvas and maintain its aspect ratio
let getFullSizeScaleFactorWithAspectRatio =
    (canvasSize: size, imageSize: size): scaleFactor => {
  let widthRatio: float = canvasSize.width /. imageSize.width;
  let scale: float =
    // If we apply the scale factor from the width ratio to the image height,
    // and that scaled image height fits in the canvas, then use this ratio.
    // The scaled width will be full canvas width
    // The scaled height will <= the canvas height.
    if (widthRatio *. imageSize.height <= canvasSize.height) {
      widthRatio;
    } else {
      // Else the height is too tall based on the width scaling.
      // So scale based on the height difference.
      // The scaled width will be <= the canvas width.
      // The scaled height will be the full canvas height.
      let heightRatio: float = canvasSize.height /. imageSize.height;
      heightRatio;
    };
  {scale: scale};
};

let scaleValue = (value: float, scaleFactor: scaleFactor): float =>
  value *. scaleFactor.scale;

let scaleSize = (size: size, scaleFactor: scaleFactor): size => {
  width: scaleValue(size.width, scaleFactor),
  height: scaleValue(size.height, scaleFactor),
};

let drawCore = (canvas, image) => {
  let canvasSize: size = {width: canvas##width, height: canvas##height};
  let imageSize: size = {
    width: image##naturalWidth,
    height: image##naturalHeight,
  };

  let context = canvas##getContext("2d");
  context##clearRect(0, 0, canvasSize.width, canvasSize.height);

  let imageToCanvasScaleFactor: scaleFactor =
    getFullSizeScaleFactorWithAspectRatio(canvasSize, imageSize);
  let imageSizeScaledToCanvas =
    scaleSize(imageSize, imageToCanvasScaleFactor);
  let centeredPosition =
    getStartCenterPosition(imageSizeScaledToCanvas, canvasSize);

  let zoomScaleFactor: scaleFactor = {scale: 1.0};
  context##save();
  context##imageSmoothingEnabled #= true;
  context##imageSmoothingQuality #= "high";
  context##translate(centeredPosition.x, centeredPosition.y);
  context##scale(
    imageToCanvasScaleFactor.scale,
    imageToCanvasScaleFactor.scale,
  );
  context##scale(zoomScaleFactor.scale, zoomScaleFactor.scale);
  context##drawImage(image, 0, 0);
  context##restore();

  Js.log4("draw()", imageSize, canvasSize, centeredPosition);
};

let draw = (imageLoadState: imageLoadState, canvas, image) => {
  switch (imageLoadState) {
  | ImageNotLoaded => ()
  | ImageLoaded => drawCore(canvas, image)
  };
};

let portraitSource: string = "https://upload.wikimedia.org/wikipedia/commons/a/a8/Van_Gogh_Self-Portrait_with_Straw_Hat_1887-Metropolitan.jpg";
let landscapeSource: string = "https://upload.wikimedia.org/wikipedia/commons/thumb/e/ea/Van_Gogh_-_Starry_Night_-_Google_Art_Project.jpg/2560px-Van_Gogh_-_Starry_Night_-_Google_Art_Project.jpg";

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