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
  React.useEffect(() =>
    switch (Js.Nullable.to_opt(canvasRef->React.Ref.current)) {
    | None => failwith("fail")
    | Some(el) => draw(ReactDOMRe.domElementToObj(el))
    }
  );
  <canvas width="150" height="150" ref={ReactDOMRe.Ref.domRef(canvasRef)} />;
};