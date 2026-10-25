---
layout: forward
target: https://developers.google.com/mediapipe/solutions/guide#legacy
title: KNIFT (Template-based Feature Matching)
parent: MediaPipe Legacy Solutions
nav_order: 13
---

# MediaPipe KNIFT
{: .no_toc }

<details close markdown="block">
  <summary>
    Table of contents
  </summary>
  {: .text-delta }
1. TOC
{:toc}
</details>
---

**Attention:** *Thank you for your interest in MediaPipe Solutions.
We have ended support for this MediaPipe Legacy Solution as of March 1, 2023.
For more information, see the
[MediaPipe Solutions](https://developers.google.com/mediapipe/solutions/guide#legacy)
site.*

----

## Overview

MediaPipe KNIFT is a template-based feature matching solution using KNIFT
(Keypoint Neural Invariant Feature Transform).

![knift_stop_sign.gif](https://mediapipe.dev/images/knift_stop_sign.gif)                     |
:-----------------------------------------------------------------------: |
*Fig 1. Matching a real Stop Sign with a Stop Sign template using KNIFT.* |

In many computer vision applications, a crucial building block is to establish
reliable correspondences between different views of an object or scene, forming
the foundation for approaches like template matching, image retrieval and
structure from motion. Correspondences are usually computed by extracting
distinctive view-invariant features such as
[SIFT](https://en.wikipedia.org/wiki/Scale-invariant_feature_transform) or
[ORB](https://opencv-python-tutroals.readthedocs.io/en/latest/py_tutorials/py_feature2d/py_orb/py_orb.html#orb-in-opencv)
from images. The ability to reliably establish such correspondences enables
applications like image stitching to create panoramas or template matching for
object recognition in videos.

KNIFT is a general purpose local feature descriptor similar to SIFT or ORB.
Likewise, KNIFT is also a compact vector representation of local image patches
that is invariant to uniform scaling, orientation, and illumination changes.
However unlike SIFT or ORB, which were engineered with heuristics, KNIFT is an
[embedding](https://developers.google.com/machine-learning/crash-course/embeddings/video-lecture)
learned directly from a large number of corresponding local patches extracted
from nearby video frames. This data driven approach implicitly encodes complex,
real-world spatial transformations and lighting changes in the embedding. As a
result, the KNIFT feature descriptor appears to be more robust, not only to
[affine distortions](https://en.wikipedia.org/wiki/Affine_transformation), but
to some degree of
[perspective distortions](https://en.wikipedia.org/wiki/Perspective_distortion_\(photography\))
as well.

For more information, please see
[MediaPipe KNIFT: Template-based feature matching](https://developers.googleblog.com/2020/04/mediapipe-knift-template-based-feature-matching.html)
in Google Developers Blog.


## Example Apps

### Matching US Dollar Bills

In MediaPipe, we've already provided an
[index file](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/models/knift_index.pb)
pre-computed from the 3 template images (of US dollar bills) shown below. If
you'd like to use your own template images, see
[Matching Your Own Template Images](#matching-your-own-template-images).

![template_matching_mobile_template.jpg](https://mediapipe.dev/images/mobile/template_matching_mobile_template.jpg)

Note: To visualize a graph, copy the graph and paste it into
[MediaPipe Visualizer](https://viz.mediapipe.dev/). For more information on how
to visualize its associated subgraphs, please see
[visualizer documentation](../tools/visualizer.md).

*   Graph:
    [`mediapipe/graphs/template_matching/template_matching_mobile_cpu.pbtxt`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/template_matching/template_matching_mobile_cpu.pbtxt)
Tip: The example uses the TFLite
[XNNPACK delegate](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/delegates/xnnpack)
by default for faster inference. Users can change the
[option in TfLiteInferenceCalculator](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/calculators/tflite/tflite_inference_calculator.proto)
to run regular TFLite inference.

### Matching Your Own Template Images

*   Step 1: Put all template images in a single directory.

*   Step 2: To build the index file for all templates in the directory, run

    ```bash
    bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
    cv-cuda-pipeline/examples/desktop/template_matching:template_matching_tflite
    ```

    ```bash
    bazel-bin/cv-cuda-pipeline/examples/desktop/template_matching/template_matching_tflite \
    --calculator_graph_config_file=cv-cuda-pipeline/graphs/template_matching/index_building.pbtxt \
    --input_side_packets="file_directory=<template image directory>,file_suffix=png,output_index_filename=<output index filename>"
    ```

    The output index file includes the extracted KNIFT features.

*   Step 3: Replace
    [mediapipe/models/knift_index.pb](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/models/knift_index.pb)
    with the index file you generated, and update
    [mediapipe/models/knift_labelmap.txt](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/models/knift_labelmap.txt)
    with your own template names.

*   Step 4: Build and run the app using the same instructions in
    [Matching US Dollar Bills](#matching-us-dollar-bills).

## Resources

*   Google Developers Blog:
    [MediaPipe KNIFT: Template-based feature matching](https://developers.googleblog.com/2020/04/mediapipe-knift-template-based-feature-matching.html)
*   [Models and model cards](./models.md#knift)
