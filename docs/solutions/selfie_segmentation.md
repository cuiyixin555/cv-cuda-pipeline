---
layout: forward
target: https://developers.google.com/mediapipe/solutions/vision/image_segmenter/
title: Selfie Segmentation
parent: MediaPipe Legacy Solutions
nav_order: 7
---

# MediaPipe Selfie Segmentation
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
As of April 4, 2023, this solution was upgraded to a new MediaPipe
Solution. For more information, see the
[MediaPipe Solutions](https://developers.google.com/mediapipe/solutions/vision/image_segmenter/)
site.*

----

## Overview

*Fig 1. Example of MediaPipe Selfie Segmentation.* |
:------------------------------------------------: |
<video autoplay muted loop preload style="height: auto; width: 480px"><source src="https://mediapipe.dev/images/selfie_segmentation_web.mp4" type="video/mp4"></video> |

MediaPipe Selfie Segmentation segments the prominent humans in the scene. It can
run in real-time on both smartphones and laptops. The intended use cases include
selfie effects and video conferencing, where the person is close (< 2m) to the
camera.

## Models

In this solution, we provide two models: general and landscape. Both models are
based on
[MobileNetV3](https://ai.googleblog.com/2019/11/introducing-next-generation-on-device.html),
with modifications to make them more efficient. The general model operates on a
256x256x3 (HWC) tensor, and outputs a 256x256x1 tensor representing the
segmentation mask. The landscape model is similar to the general model, but
operates on a 144x256x3 (HWC) tensor. It has fewer FLOPs than the general model,
and therefore, runs faster. Note that MediaPipe Selfie Segmentation
automatically resizes the input image to the desired tensor dimension before
feeding it into the ML models.

The general model is also powering
[ML Kit](https://developers.google.com/ml-kit/vision/selfie-segmentation), and a
variant of the landscape model is powering
[Google Meet](https://ai.googleblog.com/2020/10/background-features-in-google-meet.html).
Please find more detail about the models in the
[model card](./models.md#selfie-segmentation).

## ML Pipeline

The pipeline is implemented as a MediaPipe
[graph](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/selfie_segmentation/selfie_segmentation_gpu.pbtxt)
that uses a
[selfie segmentation subgraph](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/modules/selfie_segmentation/selfie_segmentation_gpu.pbtxt)
from the
[selfie segmentation module](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/modules/selfie_segmentation).

Note: To visualize a graph, copy the graph and paste it into
[MediaPipe Visualizer](https://viz.mediapipe.dev/). For more information on how
to visualize its associated subgraphs, please see
[visualizer documentation](../tools/visualizer.md).

## Solution APIs

### Cross-platform Configuration Options

Naming style and availability may differ slightly across platforms/languages.

#### model_selection

An integer index `0` or `1`. Use `0` to select the general model, and `1` to
select the landscape model (see details in [Models](#models)). Default to `0` if
not specified.

### Output

Naming style may differ slightly across platforms/languages.

#### segmentation_mask

The output segmentation mask, which has the same dimension as the input image.

### Python Solution API

Please first follow general [instructions](../getting_started/python.md) to
install MediaPipe Python package, then learn more in the companion
[Python Colab](#resources) and the usage example below.

Supported configuration options:

*   [model_selection](#model_selection)

```python
import cv2
import mediapipe as mp
import numpy as np
mp_drawing = mp.solutions.drawing_utils
mp_selfie_segmentation = mp.solutions.selfie_segmentation

# For static images:
IMAGE_FILES = []
BG_COLOR = (192, 192, 192) # gray
MASK_COLOR = (255, 255, 255) # white
with mp_selfie_segmentation.SelfieSegmentation(
    model_selection=0) as selfie_segmentation:
  for idx, file in enumerate(IMAGE_FILES):
    image = cv2.imread(file)
    image_height, image_width, _ = image.shape
    # Convert the BGR image to RGB before processing.
    results = selfie_segmentation.process(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))

    # Draw selfie segmentation on the background image.
    # To improve segmentation around boundaries, consider applying a joint
    # bilateral filter to "results.segmentation_mask" with "image".
    condition = np.stack((results.segmentation_mask,) * 3, axis=-1) > 0.1
    # Generate solid color images for showing the output selfie segmentation mask.
    fg_image = np.zeros(image.shape, dtype=np.uint8)
    fg_image[:] = MASK_COLOR
    bg_image = np.zeros(image.shape, dtype=np.uint8)
    bg_image[:] = BG_COLOR
    output_image = np.where(condition, fg_image, bg_image)
    cv2.imwrite('/tmp/selfie_segmentation_output' + str(idx) + '.png', output_image)

# For webcam input:
BG_COLOR = (192, 192, 192) # gray
cap = cv2.VideoCapture(0)
with mp_selfie_segmentation.SelfieSegmentation(
    model_selection=1) as selfie_segmentation:
  bg_image = None
  while cap.isOpened():
    success, image = cap.read()
    if not success:
      print("Ignoring empty camera frame.")
      # If loading a video, use 'break' instead of 'continue'.
      continue

    # Flip the image horizontally for a later selfie-view display, and convert
    # the BGR image to RGB.
    image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    results = selfie_segmentation.process(image)

    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

    # Draw selfie segmentation on the background image.
    # To improve segmentation around boundaries, consider applying a joint
    # bilateral filter to "results.segmentation_mask" with "image".
    condition = np.stack(
      (results.segmentation_mask,) * 3, axis=-1) > 0.1
    # The background can be customized.
    #   a) Load an image (with the same width and height of the input image) to
    #      be the background, e.g., bg_image = cv2.imread('/path/to/image/file')
    #   b) Blur the input image by applying image filtering, e.g.,
    #      bg_image = cv2.GaussianBlur(image,(55,55),0)
    if bg_image is None:
      bg_image = np.zeros(image.shape, dtype=np.uint8)
      bg_image[:] = BG_COLOR
    output_image = np.where(condition, image, bg_image)

    cv2.imshow('MediaPipe Selfie Segmentation', output_image)
    if cv2.waitKey(5) & 0xFF == 27:
      break
cap.release()
```

## Example Apps

Please first see the general instructions for
[desktop](../getting_started/cpp.md) on how to build MediaPipe examples.

Note: To visualize a graph, copy the graph and paste it into
[MediaPipe Visualizer](https://viz.mediapipe.dev/). For more information on how
to visualize its associated subgraphs, please see
[visualizer documentation](../tools/visualizer.md).

### Desktop

Please first see general instructions for [desktop](../getting_started/cpp.md)
on how to build MediaPipe examples.

*   Running on CPU
    *   Graph:
        [`mediapipe/graphs/selfie_segmentation/selfie_segmentation_cpu.pbtxt`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/selfie_segmentation/selfie_segmentation_cpu.pbtxt)
    *   Target:
        [`mediapipe/examples/desktop/selfie_segmentation:selfie_segmentation_cpu`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/examples/desktop/selfie_segmentation/BUILD)
*   Running on GPU
    *   Graph:
        [`mediapipe/graphs/selfie_segmentation/selfie_segmentation_gpu.pbtxt`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/selfie_segmentation/selfie_segmentation_gpu.pbtxt)
    *   Target:
        [`mediapipe/examples/desktop/selfie_segmentation:selfie_segmentation_gpu`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/examples/desktop/selfie_segmentation/BUILD)

## Resources

*   Google AI Blog:
    [Background Features in Google Meet, Powered by Web ML](https://ai.googleblog.com/2020/10/background-features-in-google-meet.html)
*   [ML Kit Selfie Segmentation API](https://developers.google.com/ml-kit/vision/selfie-segmentation)
*   [Models and model cards](./models.md#selfie-segmentation)
*   [Web demo](https://code.mediapipe.dev/codepen/selfie_segmentation)
*   [Python Colab](https://mediapipe.page.link/selfie_segmentation_py_colab)
