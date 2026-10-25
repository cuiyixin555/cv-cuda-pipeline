# CUDA miscellaneous image operators

This package contains standalone C++/CUDA implementations of the following
CLI operator identifiers:

`bndbox`, `composite`, `cvcuda_tv_augment`, `cvcuda_tv_color`,
`cvcuda_tv_compose`, `cvcuda_tv_geometry`, `cvcuda_tv_misc`, `inpaint`,
`label`, `osd`, `stack`, `warp_affine`, `warp_perspective`.

Build and test for the RTX 500 Ada (`sm_89`) from the workspace root:

```sh
bazel build --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/desktop/cuda_image_ops/misc_ops:operator_demo
bazel test --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/desktop/cuda_image_ops/misc_ops:operator_test
```

Run an operator with `operator_demo OPERATOR INPUT OUTPUT`. Input and output
images use OpenCV's BGR byte format. The kernels run on CUDA; OpenCV is only
used for image I/O and mask decoding.

```sh
operator_demo bndbox input.jpg boxes.jpg
operator_demo composite foreground.jpg composite.jpg \
  --background background.jpg --mask mask.png
operator_demo inpaint damaged.jpg restored.jpg --mask mask.png
operator_demo stack first.jpg stack.jpg --input second.jpg --input third.jpg
operator_demo cvcuda_tv_geometry input.jpg rotated.jpg --size 640x480
```

`composite` requires a same-sized grayscale mask (nonzero selects foreground).
`inpaint` requires a same-sized grayscale mask (nonzero marks damaged pixels).
`stack` accepts zero or more additional `--input` images; all images must have
matching dimensions and are written as vertically concatenated batch entries.
`--size WIDTHxHEIGHT` is optional for `cvcuda_tv_geometry` and
`cvcuda_tv_compose`; absent it defaults to the input dimensions for geometry
and 224x224 for compose. Other operators use input dimensions and do not accept
`--size`.

The demos preserve the samples' principal behaviors and fixed example
parameters, but do not use the CV-CUDA runtime. The torchvision compose example
uses a deterministic center crop, resize, and horizontal flip for a portable
preview. The augmentation kernel approximates JPEG quality artifacts with
2x2 block sampling and quantization rather than encoding JPEG. Color correction
uses fixed brightness/contrast/saturation factors; geometry and warp operators
use fixed inverse transforms. `label` thresholds BGR luminance at 128 and
propagates connected-component labels for 128 CUDA iterations. `osd` draws
GPU-generated box, diagonal, circle, and compact CV text overlays. `inpaint`
iteratively averages neighboring unmasked pixels. These are native CUDA
implementations, not CPU fallbacks.

The synthetic CUDA test exercises every listed operator and checks representative
output properties. CUDA runtime and kernel launch/synchronization failures are
reported to stderr.
