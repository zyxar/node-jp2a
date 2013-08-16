{
  "targets": [
    {
      "target_name": "jp2a",
      "sources": [ "addon.cc", "jp2a-1.0.6/src/html.c", "jp2a-1.0.6/src/image.c", "jp2a-1.0.6/src/options.c", "jp2a-1.0.6/src/term.c", "jp2a-1.0.6/src/curl.c"],
      "include_dirs": [ "/opt/include", "jp2a-1.0.6", "jp2a-1.0.6/include"],
      "cflags": [
        "-DHAVE_CONFIG_H"
      ],
      "link_settings": {
        "libraries": [
          "-L/opt/lib -ljpeg -lncurses -lcurl"
        ]
      }
    }
  ]
}
