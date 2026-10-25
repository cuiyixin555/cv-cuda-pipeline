# CUDA filter operator examples

This package provides native CUDA implementations of these CV-CUDA sample
behaviors. The command line accepts exactly these operator identifiers:

`adaptivethreshold`, `averageblur`, `bilateral_filter`, `boxblur`, `clahe`,
`conv2d`, `erase`, `gaussian`, `gaussiannoise`, `histogrameq`,
`joint_bilateral_filter`, `laplacian`, `median_blur`, `morphology`, `normalize`.

The demo reads and writes 8-bit BGR images through OpenCV. It uses CUDA kernels
for processing; OpenCV is used only for image I/O. Defaults follow the paired
CV-CUDA samples: 11-pixel Gaussian adaptive threshold (`C=2`), 7x7 average
blur, diameter-9 bilateral filters (`sigmaColor=sigmaSpace=75`), three
sample-position box-blur regions (21/45/11), 8x8 CLAHE (`clipLimit=2`),
3x3 cross sharpen convolution, deterministic sample erase rectangles,
9x9 Gaussian blur (`sigma=1.5`), deterministic Gaussian noise (`sigma=25`,
seed 42), grayscale histogram equalization, 7x7 median blur, 5x5 closing, and
ImageNet mean/std normalization. The joint bilateral filter derives its
guidance from grayscale luminance, matching the source sample. Laplacian and
normalize results are scaled back to 8-bit for display.

Build and run the tests for the local CUDA toolkit and Ada GPU:

```sh
bazel build //cv-cuda-pipeline/examples/desktop/cuda_image_ops/filter_ops:operator_demo \
  --@rules_cuda//cuda:archs=sm_89
bazel test //cv-cuda-pipeline/examples/desktop/cuda_image_ops/filter_ops:operator_test \
  --@rules_cuda//cuda:archs=sm_89
bazel run --@rules_cuda//cuda:archs=sm_89 \
  //cv-cuda-pipeline/examples/desktop/cuda_image_ops/filter_ops:operator_demo -- \
  --operator gaussian --input input.jpg --output output.jpg
```

The CLI reports CUDA allocation, copy, launch, execution, and cleanup errors.
