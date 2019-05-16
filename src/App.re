let draw = canvas => {
  let context = canvas##getContext("2d");
  context##fillStyle #= "rgb(200, 0, 0)";
  context##fillRect(10, 10, 50, 50);

  context##fillStyle #= "rgba(0, 0, 200, 0.5)";
  context##fillRect(30, 30, 50, 50);
};

[@react.component]
let make = () => {
  let canvasRef = React.useRef(Js.Nullable.null);
  React.useEffect(() => {
    switch (Js.Nullable.toOption(canvasRef->React.Ref.current)) {
    | None => ()
    | Some(canvas) => draw(ReactDOMRe.domElementToObj(canvas))
    };
    None;
  });
  <canvas width="150" height="150" ref={ReactDOMRe.Ref.domRef(canvasRef)} />;
};