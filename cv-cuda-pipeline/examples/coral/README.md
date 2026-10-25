# Coral Support

## Bazel Setup

You can compile MediaPipe with enabled Edge TPU support to run
[Coral models](http://coral.ai/models). Just add
`--define MEDIAPIPE_EDGE_TPU=<type>` to the `bazel` command:

* `--define MEDIAPIPE_EDGE_TPU=usb` for Coral USB devices on Linux
* `--define MEDIAPIPE_EDGE_TPU=pci` for Coral PCIe devices on Linux
* `--define MEDIAPIPE_EDGE_TPU=all` for both Coral USB and PCIe devices on Linux

You have to install `libusb` library in order to compile with USB support:

* `libusb-1.0-0-dev` on Linux

Command to compile face detection Coral example:

```bash
bazel build \
  --compilation_mode=opt \
  --define darwinn_portable=1 \
  --define MEDIAPIPE_DISABLE_GPU=1 \
  --define MEDIAPIPE_EDGE_TPU=usb \
  --linkopt=-l:libusb-1.0.so \
  cv-cuda-pipeline/examples/coral:face_detection_tpu
```

## Cross-compilation

To cross-compile for ARM32 or ARM64, install the corresponding compiler
toolchain on your system, then build with the appropriate Bazel CPU value:

```bash
# For ARM32 (e.g. Raspberry Pi)
bazel build \
    --crosstool_top=@crosstool//:toolchains \
    --compiler=gcc \
    --cpu=armv7a \
    --define darwinn_portable=1 \
    --define MEDIAPIPE_DISABLE_GPU=1 \
    --define MEDIAPIPE_EDGE_TPU=usb \
    --linkopt=-l:libusb-1.0.so \
    cv-cuda-pipeline/examples/coral:face_detection_tpu

# For ARM64 (e.g. Coral Dev Board)
bazel build \
    --crosstool_top=@crosstool//:toolchains \
    --compiler=gcc \
    --cpu=aarch64 \
    --define darwinn_portable=1 \
    --define MEDIAPIPE_DISABLE_GPU=1 \
    --define MEDIAPIPE_EDGE_TPU=usb \
    --linkopt=-l:libusb-1.0.so \
    cv-cuda-pipeline/examples/coral:face_detection_tpu build
```

The command above is already defined in the `Makefile`. For example:

```bash
make -C cv-cuda-pipeline/examples/coral \
     BAZEL_TARGET=cv-cuda-pipeline/examples/coral:face_detection_tpu \
     build
```

The output binary is copied to the corresponding `out/<platform>` directory.

To summarize everything:

| Arch  | `BAZEL_CPU` | Output      | Board                                                    |
| ----- | ----------- | ----------- | -------------------------------------------------------- |
| ARM32 | `armv7a`    | `out/armv7a`  | [Raspberry Pi](https://www.raspberrypi.org/products/)    |
| ARM64 | `aarch64`   | `out/aarch64` | [Coral Dev Board](https://coral.ai/products/dev-board/)  |

## Coral Examples

There are two Coral examples in `cv-cuda-pipeline/examples/coral` directory. Compile
them for your platform:

```bash
# ARM32 (e.g. Raspberry Pi)
make -C cv-cuda-pipeline/examples/coral \
     BAZEL_CPU=armv7a \
     BAZEL_TARGET=cv-cuda-pipeline/examples/coral:face_detection_tpu build

make -C cv-cuda-pipeline/examples/coral \
     BAZEL_CPU=armv7a \
     BAZEL_TARGET=cv-cuda-pipeline/examples/coral:object_detection_tpu build
```

Copy output binaries along with corresponding auxiliary files to your target
system. You can copy the whole `mediapipe` folder for simplicity:

```bash
scp -r mediapipe <user>@<host>:.
```

OpenCV runtime libraries need to be installed on your target system:

```bash
sudo apt-get install -y \
    libopencv-core-dev \
    libopencv-highgui-dev \
    libopencv-calib3d-dev \
    libopencv-features2d-dev \
    libopencv-imgproc-dev \
    libopencv-video-dev
```

If you are going to connect Coral USB accelerator to your target system then
you'll also need `libusb` library:

```shell
sudo apt-get install -y \
   libusb-1.0-0
```

Connect USB camera and Coral device to your target system and run the copied
binaries:

```bash
# Face Detection
GLOG_logtostderr=1 ./face_detection_tpu --calculator_graph_config_file \
    cv-cuda-pipeline/examples/coral/graphs/face_detection_desktop_live.pbtxt

# Object Detection
GLOG_logtostderr=1 ./object_detection_tpu --calculator_graph_config_file \
    cv-cuda-pipeline/examples/coral/graphs/object_detection_desktop_live.pbtxt
```
