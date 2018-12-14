cc_library(
  name = "http_parser",
  srcs = ["deps/http-parser/http_parser.c"],
  hdrs = ["deps/http-parser/http_parser.h"],
  includes = ["deps/http-parser/"],
)

cc_library(
  name = "headers",
  hdrs = glob([
    "src/*.h",
    "include/git2/**/*.h",
  ]),
  includes = ["include"],
)

cc_library(
  name = "zlib",
  srcs = glob(["deps/zlib/*.c"]),
  hdrs = glob(["deps/zlib/*.h"]),
  deps = [
    ":headers",
  ],
  includes = [
    "deps/zlib",
  ],
  copts = [
    "-DSTDC=1",
    "-Wno-shift-negative-value",
  ],
)

cc_library(
  name = "libgit2",
  hdrs = glob(["include/**/*.h"]),
  srcs = glob([
    "src/*.c", "src/*.h",
    "src/hash/*.c", "src/hash/*.h",
    "src/unix/*.c", "src/unix/*.h",
    "src/transports/*.c", "src/transports/*.h",
    "src/xdiff/*.h", "src/xdiff/*.c",
  ], exclude = [
    "src/hash/hash_win32.h",
    "src/hash/hash_win32.c",
    "src/transports/winhttp.c",
  ]) + [

  ],
  deps = [
    ":headers",
    ":http_parser",
    ":zlib",
  ],
  copts = [
    "-Iexternal/libgit2/src",
    "-Wno-unused-function",
  ],
  visibility = ["//visibility:public"],
)
