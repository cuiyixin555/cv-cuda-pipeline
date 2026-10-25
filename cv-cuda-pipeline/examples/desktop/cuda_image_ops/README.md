# CUDA image operator examples

This desktop C++/CUDA example set ports the behavior of the initial batch of
seven samples from the local CV-CUDA `samples/operators` collection.
It uses OpenCV for image file I/O and implements the operations as native CUDA
kernels; it does not use CV-CUDA's Python API.

Implemented operators:

| Operator | Behavior |
| --- | --- |
| `brightness_contrast` | Brightness 1.5, contrast 1.4, shift 20, center 127 |
| `channelreorder` | Swap the first and third image channels |
| `color_twist` | Apply the sample's per-channel warm color transform |
| `convertto` | Convert uint8 to float32 with scale 1/255, then back with scale 255 |
| `flip` | Horizontal flip |
| `gamma_contrast` | Apply gamma 2.2 |
| `threshold` | Binary threshold at 128, with maximum value 255 |

Build and test with the local CUDA toolkit configured in `MODULE.bazel`:

```sh
bazel build //cv-cuda-pipeline/examples/desktop/cuda_image_ops:cuda_image_ops \
  --@rules_cuda//cuda:archs=sm_89
bazel test //cv-cuda-pipeline/examples/desktop/cuda_image_ops:image_kernels_test \
  --@rules_cuda//cuda:archs=sm_89
```

Set `sm_89` to your GPU's compute capability when building on a different
device. Supplying a native architecture avoids requiring the installed driver
to JIT-compile PTX emitted by a newer CUDA toolkit.

Run an operator on an image:

```sh
bazel run --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/desktop/cuda_image_ops:cuda_image_ops -- \
  --operator flip \
  --input /path/to/input.jpg \
  --output /path/to/output.jpg
```

Replace `flip` with any operator in the table. Inputs are decoded as 8-bit,
three-channel BGR images. CUDA errors, unsupported operators, and image I/O
errors are reported to stderr.
