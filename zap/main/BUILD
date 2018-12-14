cc_binary(
    name = "main",
    srcs = glob(["*.cc", "*.h"]),
    copts = ["--std=c++14"],
    linkopts = select({
        "//tools/config:more-static": ["-static"],
        "//conditions:default": [],
    }),
    linkstatic = select({
        "//tools/config:fastbuild": 0,
        "//conditions:default": 1,
    }),
    visibility = ["//visibility:public"],
    deps = [
        "@cxxopts",
        "@spdlog",
        "@com_google_absl//absl/strings",
        "@rang",
        "@com_googlesource_code_re2//:re2",
        "@libgit2",
	"@progressbar",
    ],
)
