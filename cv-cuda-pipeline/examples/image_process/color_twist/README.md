# CUDA color twist demo

This C++/CUDA example applies a 3 × 4 affine color matrix to an RGB8 image.
For each pixel, the demo uses:

```text
R' = 1.2 × R + 10
G' = G
B' = 0.8 × B - 10
```

Results are rounded to the nearest integer (ties to even) and clamped to
`[0, 255]`. OpenCV handles image I/O and BGR/RGB conversion; the color
transformation runs on the GPU.

## Build and test

From the repository root, with Bazel, CUDA and OpenCV development libraries:

```bash
bazel build --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/image_process/color_twist:color_twist_demo
bazel test --@rules_cuda//cuda:archs=sm_89 --test_output=errors \
  //cv-cuda-pipeline/calculators/image_process/cuda/tests:color_twist_kernel_test
```

Replace `sm_89` with your GPU architecture. The build uses
[rules_cuda](https://github.com/bazel-contrib/rules_cuda).

## Run

```bash
mkdir -p cv-cuda-pipeline/examples/image_process/color_twist/perf_asserts
bazel-bin/cv-cuda-pipeline/examples/image_process/color_twist/color_twist_demo \
  --input cv-cuda-pipeline/pictures/tabby_tiger_cat.jpg \
  --output cv-cuda-pipeline/examples/image_process/color_twist/perf_asserts/color_twist_cat.png
bazel-bin/cv-cuda-pipeline/examples/image_process/color_twist/color_twist_demo \
  --input cv-cuda-pipeline/pictures/dino.jpg \
  --output cv-cuda-pipeline/examples/image_process/color_twist/perf_asserts/color_twist_dino.png
```

`--input` is required; `--output` defaults to `cat_color_twist.png` in the
current directory. OpenCV selects the output format from its extension.

| Input | Output |
| --- | --- |
| ![Input cat](../../../pictures/tabby_tiger_cat.jpg) | ![Color twist cat](perf_asserts/color_twist_cat.png) |
| ![Input dinosaur](../../../pictures/dino.jpg) | ![Color twist dinosaur](perf_asserts/color_twist_dino.png) |

The kernel API accepts non-overlapping interleaved three-channel U8 device
buffers, byte row strides, image dimensions, a row-major 3 × 4 float matrix
passed by value, and an optional CUDA stream. The matrix rows refer to the
channels in buffer order; each row contains three weights and an additive
offset. Coefficients must be finite and each row must satisfy
`255*(abs(m0)+abs(m1)+abs(m2))+abs(m3) <= FLT_MAX/2`. The caller must keep
buffers valid until stream completion and check synchronization errors. Other
formats and batches are not implemented.

## Nsight Systems performance

On an NVIDIA RTX 500 Ada Generation Laptop GPU with CUDA 13.3 and Nsight
Systems 2026.1.3, the default Bazel build produced these results from one
sequential run per image:

| Image | Kernel latency | Estimated effective bandwidth |
| --- | ---: | ---: |
| Cat, 720 × 720 | 18.298 µs | 169.99 GB/s |
| Dinosaur, 2876 × 1699 | 217.902 µs | 134.55 GB/s |

Estimated logical bandwidth is six bytes per pixel (one three-byte image read
and one three-byte write) divided by kernel time. This is not measured DRAM
bandwidth: caching, compiler optimizations and memory transactions affect
actual traffic. Single-launch timings are not a steady-state benchmark and
exclude CPU work, image I/O and transfers.

To reproduce, use new report names if they already exist:

```bash
mkdir -p build/color_twist cv-cuda-pipeline/examples/image_process/color_twist/perf_asserts
nsys profile --trace=cuda --sample=none --cpuctxsw=none \
  --output=build/color_twist/nsys_cat \
  bazel-bin/cv-cuda-pipeline/examples/image_process/color_twist/color_twist_demo \
  --input cv-cuda-pipeline/pictures/tabby_tiger_cat.jpg \
  --output cv-cuda-pipeline/examples/image_process/color_twist/perf_asserts/profile_cat.png
nsys stats --report cuda_gpu_kern_sum --format csv build/color_twist/nsys_cat.nsys-rep
nsys profile --trace=cuda --sample=none --cpuctxsw=none \
  --output=build/color_twist/nsys_dinosaur \
  bazel-bin/cv-cuda-pipeline/examples/image_process/color_twist/color_twist_demo \
  --input cv-cuda-pipeline/pictures/dino.jpg \
  --output cv-cuda-pipeline/examples/image_process/color_twist/perf_asserts/profile_dinosaur.png
nsys stats --report cuda_gpu_kern_sum --format csv build/color_twist/nsys_dinosaur.nsys-rep
```
