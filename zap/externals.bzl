load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

def _new_libgit2_archive_impl(ctx):
  tgz = "libgit2-" + ctx.attr.version + ".tar.gz"
  ctx.download(
    ctx.attr.url,
    tgz,
    ctx.attr.sha256,
  )
  ctx.symlink(ctx.attr.build_file, "BUILD")
  cmd = ctx.execute([
    "tar", "-xzf", tgz,
    "--strip-components=1",
    "--exclude=libgit2-" + ctx.attr.version + "/tests"
  ])

  if cmd.return_code != 0:
    fail("error unpacking: " + cmd.stderr)

new_libgit2_archive = repository_rule(
  implementation=_new_libgit2_archive_impl,
  attrs = {
    "url": attr.string(mandatory = True),
    "sha256": attr.string(),
    "build_file": attr.label(mandatory = True),
    "version": attr.string(mandatory = True),
  })



# We define our externals here instead of directly in WORKSPACE
# because putting the `new_git_repository` calls here instead of there
# works around https://github.com/bazelbuild/bazel/issues/1465 when
# passing `build_file` to the `new_git_repository`.
def externals():
    git_repository(
        name = "com_googlesource_code_re2",
        commit = "7cf8b88e8f70f97fd4926b56aa87e7f53b2717e0",
        remote = "https://github.com/google/re2",
    )
    
    new_libgit2_archive(
      name = "libgit2",
      build_file = "//:libgit2.BUILD",
      sha256 = "0269ec198c54e44f275f8f51e7391681a03aa45555e2ab6ce60b0757b6bde3de",
      url = "https://github.com/libgit2/libgit2/archive/v0.24.1.tar.gz",
      version = "0.24.1",
    )

    native.new_http_archive(
        name = "gtest",
        url = "https://github.com/google/googletest/archive/release-1.8.0.zip",
        sha256 = "f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
        build_file = "gtest.BUILD",
        strip_prefix = "googletest-release-1.8.0",
    )

    # their zip archive has symlinks that bazel does not like
    new_git_repository(
        name="spdlog",
        remote="https://github.com/gabime/spdlog.git",
        commit="55680db160c3c486ccbbb40e10f6338e4d98e84d", # v0.16.3 - with eol customization
        build_file="//:spdlog.BUILD",
    )

    new_git_repository(
        name="lizard",
        remote="https://github.com/inikep/lizard.git",
        commit="6a1ed71450148c8aed57de3179b1bdd81800bada",
        build_file="//:lizard.BUILD",
    )

    new_git_repository(
        name="jemalloc",
        remote="https://github.com/jemalloc/jemalloc.git",
        commit="e8a63b87c36ac814272d73b503658431d2000055",
        build_file="//:jemalloc.BUILD",
    )

    new_git_repository(
        name="progressbar",
        remote="https://github.com/doches/progressbar.git",
        commit="c4c54f891ab05cfc411ec5c2ed147dd4cad1ccf3",
        build_file="//:progressbar.BUILD",
    )

    native.new_http_archive(
        name="cxxopts",
        url="https://github.com/jarro2783/cxxopts/archive/v2.1.0.zip",
        sha256="9cd036f58b147d21d43b27144c811b06f32dfa63a4bba89e8dace4699428a8b1",
        build_file="//:cxxopts.BUILD",
        strip_prefix = "cxxopts-2.1.0",
    )

    native.new_http_archive(
        name="rang",
        url = "https://github.com/agauniyal/rang/archive/v3.1.0.zip",
        sha256="658adeb8a36d36981d4339fc839f2deedc0e75cb421db1982041d8a0a255835d",
        build_file = "rang.BUILD",
        strip_prefix = "rang-3.1.0",
    )

    git_repository(
        name="com_google_absl",
        remote="https://github.com/abseil/abseil-cpp.git",
        commit="dd9911a004edcc34152850a6216bb3b53ad6bb82"
    )

    new_git_repository(
        name = "compdb",
        commit = "1cf753e9f3486372da026ca629ec8f6760dc31ff",
        remote = "https://github.com/grailbio/bazel-compilation-database.git",
        build_file_content = (
        """
package(default_visibility = ["//visibility:public"])
"""
        ),
    )

    native.new_http_archive(
        name="clang_6_0_0_darwin",
        url="http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-apple-darwin.tar.xz",
        build_file="//:clang.BUILD",
        sha256="0ef8e99e9c9b262a53ab8f2821e2391d041615dd3f3ff36fdf5370916b0f4268",
        type="tar.xz",
        strip_prefix="clang+llvm-6.0.0-x86_64-apple-darwin/",
    )

    native.new_http_archive(
        name="clang_6_0_0_linux",
        url="http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz",
        build_file="//:clang.BUILD",
        sha256="114e78b2f6db61aaee314c572e07b0d635f653adc5d31bd1cd0bf31a3db4a6e5",
        type="tar.xz",
        strip_prefix="clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/",
    )
