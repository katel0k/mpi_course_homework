let page =
  let open Dream_html in
  let open HTML in
  html [lang "en"] [
    head [] [
      title [] "Life" ];
    body [] [
      div [] [
        button [id "next"] [txt "next"];
        button [id "prev"] [txt "prev"];
        button [id "start"] [txt "start"];
        button [id "stop"] [txt "stop"];
        select [id "preset"] [
          option [value "glider"] "glider"
        ];
        button [id "load"] [txt "load"]
      ];
      canvas [id "canvas"; width "600"; height "600"] [];
      script [src "static/index.js"] ""
    ]
  ]

let index_handler _ =
  Dream_html.to_string page
  |> Dream.html


(* let parse_json json = 
  Yojson.Safe.from_string json  *)

let static_file_handler = Dream.static "./bin/"

let preset_handler request = Dream.from_filesystem "." (Dream.param request "preset_path") request

(* let step_handler = ignore (Sys.command ""); Dream.from_filesystem "." "life/life.vtk" *)

let () =
  Dream.run
  @@ Dream.logger
  @@ Dream.router [
    Dream.get "/" index_handler;
    (* Dream.get "/next" step_handler; *)
    Dream.get "/static/**" static_file_handler;
    Dream.get "/preset/path:preset_path" preset_handler
  ]