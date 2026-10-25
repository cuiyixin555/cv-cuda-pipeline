# CV-CUDA transform operator ports (CUDA/C++)

This package implements a separate CUDA kernel path and a C++ image CLI for
the following 14 samples from `CV-CUDA/samples/operators`:

`center_crop`, `advcvtcolor`, `cvtcolor`, `reformat`, `resize`, `hq_resize`,
`pillowresize`, `copymakeborder`, `customcrop`, `random_resized_crop`,
`crop_flip_normalize_reformat`, `resize_crop_convert_reformat`, `remap`,
`rotate`.

OpenCV is used only to decode/encode images. The image transforms run in CUDA
kernels; CLI inputs and outputs are 8-bit, three-channel BGR images. The
reformat and normalize/convert fused operators return a viewable 8-bit image
after their internal layout/value conversions.

## Build and test

```sh
bazel build //cv-cuda-pipeline/examples/desktop/cuda_image_ops/transform_ops:operator_demo \
  --@rules_cuda//cuda:archs=sm_89
bazel test //cv-cuda-pipeline/examples/desktop/cuda_image_ops/transform_ops:operator_test \
  --@rules_cuda//cuda:archs=sm_89
```

## Run

```sh
bazel run --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/desktop/cuda_image_ops/transform_ops:operator_demo -- \
  --operator resize --input /path/to/input.jpg --output /path/to/output.png
```

`--operator` and `--input` are required. The output defaults to
`operator_output.png`; `--width` and `--height` default to 224 for resize and
resized-crop operations. Center crop uses those dimensions capped at the input
size. Custom crop defaults to a 60% rectangle at 20% image offset; the fused
crop/flip/normalize sample defaults to an 80% rectangle at 10% offset.
`--x`, `--y`, `--crop-width`, and `--crop-height` can override these rectangles.
Random resized crop uses the source sample's 0.08–1.0 area and 0.75–4/3 aspect
ratio ranges with a deterministic default seed (`--seed 42`). `rotate` defaults
to 45 degrees (`--angle`). The border sample adds 30 pixels top/bottom and
60 pixels left/right with a solid orange border. Resize, HQ resize, and Pillow
resize use bilinear, antialiased Lanczos, and Lanczos interpolation
respectively. Remap uses the source sinusoidal displacement field.

Allocation, launch, synchronization, transfer, and cleanup errors are reported
instead of being ignored. The test exercises each listed operator on a small
synthetic image.
