SHELL := /bin/bash

MAKEFILE_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
MEDIAPIPE_DIR := $(MAKEFILE_DIR)/../../..

BAZEL_COMPILATION_MODE ?= opt
BAZEL_TARGET ?= cv-cuda-pipeline/examples/coral:face_detection_tpu
BAZEL_CPU ?= k8

OUT_DIR := $(MEDIAPIPE_DIR)/out/$(BAZEL_CPU)

bazel_output = $(MEDIAPIPE_DIR)/bazel-bin/$(subst :,/,$(1))

.PHONY: help
help:
	@echo "make help   - Print help"
	@echo "make build  - Run Bazel build, use BAZEL_TARGET to choose which target to build"

.PHONY: build
build:
	(cd $(MEDIAPIPE_DIR) && \
	 bazel build \
	     --crosstool_top=@crosstool//:toolchains \
	     --compiler=gcc \
	     --cpu=${BAZEL_CPU} \
	     --compilation_mode=${BAZEL_COMPILATION_MODE} \
	     --define darwinn_portable=1 \
	     --define MEDIAPIPE_DISABLE_GPU=1 \
	     --define MEDIAPIPE_EDGE_TPU=all \
	     $(BAZEL_TARGET) && \
	 mkdir -p $(OUT_DIR) && \
	 cp -f $(call bazel_output,$(BAZEL_TARGET)) $(OUT_DIR))
