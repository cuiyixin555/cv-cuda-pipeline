# CV-CUDA Pipeline

![CV-CUDA Pipeline banner](asserts/cv-cuda-pipeline.png)

CV-CUDA Pipeline is a C++ computer-vision project for building and running
real-time image and video processing graphs on Linux and Windows PCs.
The product scope is desktop C++/CUDA only: mobile apps, browser frontends,
TypeScript/JavaScript task APIs, and WebGPU backends are not included.
It uses Bazel to manage native
code, models, and third-party dependencies. The repository includes desktop
examples for tasks such as face detection, hand tracking, object detection,
and image segmentation.

## Project goals

- Compose image and video processing stages into reusable graph-based
  pipelines.
- Provide desktop examples that can process live camera input or video files.
- Use OpenCV for image and video I/O and CUDA-capable components where the
  build environment supports them.
- Keep builds and dependency configuration reproducible with Bazel.

## Requirements

- Linux (Ubuntu recommended)
- Bazelisk or a compatible Bazel installation
- OpenJDK 21 JDK, with `JAVA_HOME` set to its installation directory
- A C++20 compiler and the system development libraries required by the
  selected targets
- OpenCV development libraries for examples that use OpenCV
- Optional GPU drivers and CUDA toolkit for targets configured to use CUDA

Exact system dependencies depend on the target and platform. CPU examples
avoid requiring a CUDA-capable GPU, but still need their declared native
dependencies at build and run time.

## WSL setup, build, and run

The following commands set up a WSL Ubuntu environment. Install the JDK once:

```bash
sudo apt update
sudo apt install -y openjdk-21-jdk-headless
```

Set `JAVA_HOME` and add the JDK binaries to `PATH` in each new shell:

```bash
export JAVA_HOME=/usr/lib/jvm/java-21-openjdk-amd64
export PATH="$JAVA_HOME/bin:$PATH"
java -version
javac -version
```

Both version commands should report Java 21. To load these settings
automatically in future Bash shells, add the two `export` lines to
`~/.bashrc`, then run `source ~/.bashrc`.

Run the following commands from the repository root:

Build and run the small C++ example:

```bash
bazel build -c opt \
  //cv-cuda-pipeline/examples/desktop/hello_world:hello_world

bazel run \
  //cv-cuda-pipeline/examples/desktop/hello_world:hello_world
```

Build the CPU face-detection example:

```bash
bazel build -c opt \
  //cv-cuda-pipeline/examples/desktop/face_detection:face_detection_cpu
```

Run face detection on a video file and write an annotated output:

```bash
cd /mnt/c/Users/xincu/cv-cuda-pipeline

LD_LIBRARY_PATH=/home/cuixin/.local/opencv/usr/lib/x86_64-linux-gnu \
GLOG_logtostderr=1 \
bazel-bin/cv-cuda-pipeline/examples/desktop/face_detection/face_detection_cpu \
  --calculator_graph_config_file=cv-cuda-pipeline/graphs/face_detection/face_detection_desktop_live.pbtxt \
  --input_video_path='/mnt/c/Users/xincu/Videos/Bandicam/bandicam 2026-09-09 21-08-00-110.mp4' \
  --output_video_path=bazel-out/face_detection_output.mp4
```

The `LD_LIBRARY_PATH` above is for an OpenCV installation in the user's home
directory. If OpenCV is installed in a different non-system location, replace
it with that installation's library directory; omit it if OpenCV libraries are
available in the system library paths. The annotated video is written to
`bazel-out/face_detection_output.mp4`.

Other desktop examples and their Bazel targets are under
[`cv-cuda-pipeline/examples/desktop`](./cv-cuda-pipeline/examples/desktop).
The graph configurations used by examples are under
[`cv-cuda-pipeline/graphs`](./cv-cuda-pipeline/graphs).

### CUDA image operators

All 49 operator examples from the CV-CUDA `samples/operators` collection have
C++ command-line demos and native CUDA kernels, grouped by behavior under
[`cuda_image_ops`](./cv-cuda-pipeline/examples/desktop/cuda_image_ops):

- Pixel operations: `brightness_contrast`, `channelreorder`, `color_twist`,
  `convertto`, `flip`, `gamma_contrast`, and `threshold`.
- Geometric and format operations: `center_crop`, `advcvtcolor`, `cvtcolor`,
  `reformat`, `resize`, `hq_resize`, `pillowresize`, `copymakeborder`,
  `customcrop`, `random_resized_crop`, `crop_flip_normalize_reformat`,
  `resize_crop_convert_reformat`, `remap`, and `rotate`.
- Filters and image statistics: `adaptivethreshold`, `averageblur`,
  `bilateral_filter`, `boxblur`, `clahe`, `conv2d`, `erase`, `gaussian`,
  `gaussiannoise`, `histogrameq`, `joint_bilateral_filter`, `laplacian`,
  `median_blur`, `morphology`, and `normalize`.
- Compositing and advanced operations: `bndbox`, `composite`,
  `cvcuda_tv_augment`, `cvcuda_tv_color`, `cvcuda_tv_compose`,
  `cvcuda_tv_geometry`, `cvcuda_tv_misc`, `inpaint`, `label`, `osd`, `stack`,
  `warp_affine`, and `warp_perspective`.

Set `CUDA_PATH` if the toolkit is not discoverable from `PATH` or the platform's
default installation location, and choose the CUDA architecture supported by
your GPU (the example below uses `sm_89`):

```bash
export CUDA_PATH=/usr/local/cuda
bazel build --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/desktop/cuda_image_ops/...
bazel test --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/desktop/cuda_image_ops/...
bazel run --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/desktop/cuda_image_ops:cuda_image_ops -- \
  --operator flip --input /path/to/input.jpg --output /path/to/output.jpg
```

Each operator family README documents its CLI, default parameters, and
operator-specific limitations. The samples use OpenCV for image I/O; processing
is performed by CUDA kernels.

## Testing

C++ unit tests are defined alongside their libraries in Bazel `BUILD` files.
For example:

```bash
bazel test -c opt \
  //cv-cuda-pipeline/examples/desktop/autoflip/quality:kinematic_path_solver_test
```

### CV-CUDA native unit and system tests

The migrated CV-CUDA C++/CUDA tests and their Bazel targets are in
[`cv-cuda-pipeline/cvcuda/tests`](./cv-cuda-pipeline/cvcuda/tests/BUILD). Build
and run them from the repository root. They require a CUDA toolkit, a
compatible NVIDIA driver and GPU, and the architecture flag for that GPU.
The commands below use `/usr/local/cuda` and `sm_89` as examples; adjust them
for your environment.

Compile all currently defined CV-CUDA native test targets without running
them:

```bash
export CUDA_PATH=/usr/local/cuda
bazel build --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_types_unit_test \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_types_system_test \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_types_standalone_test \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_cudatools_system_test \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_version_api_override_test \
  //cv-cuda-pipeline/cvcuda/tests:cvcuda_unit_test \
  //cv-cuda-pipeline/cvcuda/tests:cvcuda_system_test \
  //cv-cuda-pipeline/cvcuda/tests:cvcuda_system_smoke_test
```

Run a single test target:

```bash
bazel test --@rules_cuda//cuda:archs=sm_89 --test_output=errors \
  //cv-cuda-pipeline/cvcuda/tests:cvcuda_unit_test
```

Run the complete native test matrix. The CV-CUDA system suite is large and
sharded by Bazel, so allow up to 30 minutes:

```bash
bazel test --@rules_cuda//cuda:archs=sm_89 --test_timeout=1800 \
  --test_output=errors \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_types_unit_test \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_types_system_test \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_types_standalone_test \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_cudatools_system_test \
  //cv-cuda-pipeline/cvcuda/tests:nvcv_version_api_override_test \
  //cv-cuda-pipeline/cvcuda/tests:cvcuda_unit_test \
  //cv-cuda-pipeline/cvcuda/tests:cvcuda_system_test \
  //cv-cuda-pipeline/cvcuda/tests:cvcuda_system_smoke_test
```

To run selected GoogleTest cases, pass a filter after `--`:

```bash
bazel test --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/cvcuda/tests:cvcuda_system_test \
  --test_arg=--gtest_filter='OpAdjustContrast.*'
```

Replace `sm_89` with the architecture supported by your GPU. `bazel test`
builds the requested target before executing it; use `bazel build` when you
only want to compile. Test logs are available with `--test_output=errors` for
failures or `--test_output=all` to show all test output.

## Contributing

Contributions should follow the repository's existing Bazel, C++, and
formatting conventions. Include focused tests for changes when applicable.

## References

- [MediaPipe](https://github.com/google-ai-edge/mediapipe)
- [NVIDIA CV-CUDA](https://github.com/CVCUDA/CV-CUDA.git)
- [Bazel CUDA rules](https://github.com/bazel-contrib/rules_cuda.git)
