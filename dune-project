(lang dune 3.16)

(name comparison)

(generate_opam_files true)

(source
 (github username/reponame))

(authors "Author Name")

(maintainers "Maintainer Name")

(license LICENSE)

(documentation https://url/to/documentation)

(package
 (name comparison)
 (synopsis "A short synopsis")
 (description "A longer description")
 (depends
  (ocaml (= 5.1.0))
  dune
  dream
  dream-html
  (ocaml-lsp-server :dev)
  (ocamlformat :dev)
 )
 (tags
  (topics "to describe" your project))
 (allow_empty))