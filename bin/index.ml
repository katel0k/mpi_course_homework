let page =
  let open Dream_html in
  let open HTML in
  html [lang "en"] [
    head [] [
      title [] "Life" ];
    body [] [
      div [] [
        button [id "next"] [txt "next"];
        button [id "start"] [txt "start"];
        button [id "stop"] [txt "stop"];
        select [id "preset"] [
          option [value "glider"] "glider"
        ];
        button [id "load"] [txt "load"]
      ];
      canvas [id "canvas"; width "600"; height "600"] [];
      script [src "static/js/main.js"] ""
    ]
  ]

let index_handler _ =
  Dream_html.to_string page
  |> Dream.html

let static_folder = "./bin/static"

let static_file_handler = Dream.static static_folder

let preset_handler request = Dream.from_filesystem static_folder (Dream.param request "preset_path") request

let buffer_saver body =
  let buffer_file = open_out "buffer" in
    Printf.fprintf buffer_file "%s" body;
    close_out buffer_file

let proto_parser inner_handler request =
  let%lwt body = Dream.body request in
    let field = Life_field.decode_pb_field @@ Pbrt.Decoder.of_string body in
      match field.field with
        | Life_field.Raw str -> buffer_saver 
          (String.concat " " [Int32.to_string(field.width); Int32.to_string(field.height); str]);
          inner_handler request
        | _ -> inner_handler request


let step_handler request =
  ignore (Sys.command "mpiexec -n 8 bin/life/one_step.exe buffer");
  Dream.from_filesystem "." "life.raw" request

let () =
  Dream.run
  @@ Dream.logger
  @@ Dream.router [
    Dream.get "/" index_handler;
    Dream.post "/next" (proto_parser @@ step_handler);
    Dream.get "/static/**" static_file_handler;
    Dream.get "/preset/path:preset_path" preset_handler
  ]