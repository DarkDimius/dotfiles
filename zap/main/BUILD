config_setting(
    name = "can-use-jemalloc",
    values = {
        "define": "unsanitized=true",
        "compilation_mode": "opt",
    },
)

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
    malloc = select({
        ":can-use-jemalloc": "@jemalloc//:jemalloc",
        "//conditions:default": "@bazel_tools//tools/cpp:malloc",
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
