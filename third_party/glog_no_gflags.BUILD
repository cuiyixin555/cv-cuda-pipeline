load(":bazel/glog.bzl", "glog_library")

licenses(["notice"])

# gflags is not needed on mobile platforms, and tried to link in
# TODO: upstream.
glog_library(with_gflags = 0)
