let draw = canvas => {
  let context = canvas##getContext("2d");
  context##fillStyle #= "rgb(200, 0, 0)";
  context##fillRect(10, 10, 50, 50);

  context##fillStyle #= "rgba(0, 0, 200, 0.5)";
  context##fillRect(30, 30, 50, 50);
};

[@react.component]
let make = () => {
  let canvas_id = "canvas";
  let canvasRef = React.useRef(Js.Nullable.null);
  React.useEffect(() =>
    switch (ReactDOMRe._getElementById(canvas_id)) {
    | None => failwith("fail")
    | Some(el) => draw(ReactDOMRe.domElementToObj(el))
    }
  );
  <canvas
    id=canvas_id
    width="150"
    height="150"
    ref={ReactDOMRe.Ref.domRef(canvasRef)}
  />;
};