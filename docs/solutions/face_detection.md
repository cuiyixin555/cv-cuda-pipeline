---
layout: forward
target: https://developers.google.com/mediapipe/solutions/vision/face_detector/
title: Face Detection
parent: MediaPipe Legacy Solutions
nav_order: 1
---

# MediaPipe Face Detection
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
As of May 10, 2023, this solution was upgraded to a new MediaPipe
Solution. For more information, see the
[MediaPipe Solutions](https://developers.google.com/mediapipe/solutions/vision/face_detector)
site.*

----

## Overview

MediaPipe Face Detection is an ultrafast face detection solution that comes with
6 landmarks and multi-face support. It is based on
[BlazeFace](https://arxiv.org/abs/1907.05047), a lightweight and well-performing
face detector tailored for mobile GPU inference. The detector's super-realtime
performance enables it to be applied to any live viewfinder experience that
requires an accurate facial region of interest as an input for other
task-specific models, such as 3D facial keypoint estimation (e.g.,
[MediaPipe Face Mesh](./face_mesh.md)), facial features or expression
classification, and face region segmentation. BlazeFace uses a lightweight
feature extraction network inspired by, but distinct from
[MobileNetV1/V2](https://ai.googleblog.com/2018/04/mobilenetv2-next-generation-of-on.html),
a GPU-friendly anchor scheme modified from
[Single Shot MultiBox Detector (SSD)](https://arxiv.org/abs/1512.02325), and an
improved tie resolution strategy alternative to non-maximum suppression. For
more information about BlazeFace, please see the [Resources](#resources)
section.

## Solution APIs

### Configuration Options

Naming style and availability may differ slightly across platforms/languages.

#### model_selection

An integer index `0` or `1`. Use `0` to select a short-range model that works
best for faces within 2 meters from the camera, and `1` for a full-range model
best for faces within 5 meters. For the full-range option, a sparse model is
used for its improved inference speed. Please refer to the
[model cards](./models.md#face_detection) for details. Default to `0` if not
specified.

#### min_detection_confidence

Minimum confidence value (`[0.0, 1.0]`) from the face detection model for the
detection to be considered successful. Default to `0.5`.

### Output

Naming style may differ slightly across platforms/languages.

#### detections

Collection of detected faces, where each face is represented as a detection
proto message that contains a bounding box and 6 key points (right eye, left
eye, nose tip, mouth center, right ear tragion, and left ear tragion). The
bounding box is composed of `xmin` and `width` (both normalized to `[0.0, 1.0]`
by the image width) and `ymin` and `height` (both normalized to `[0.0, 1.0]` by
the image height). Each key point is composed of `x` and `y`, which are
normalized to `[0.0, 1.0]` by the image width and height respectively.

### Python Solution API

Please first follow general [instructions](../getting_started/python.md) to
install MediaPipe Python package, then learn more in the companion
[Python Colab](#resources) and the usage example below.

Supported configuration options:

*   [model_selection](#model_selection)
*   [min_detection_confidence](#min_detection_confidence)

```python
import cv2
import mediapipe as mp
mp_face_detection = mp.solutions.face_detection
mp_drawing = mp.solutions.drawing_utils

# For static images:
IMAGE_FILES = []
with mp_face_detection.FaceDetection(
    model_selection=1, min_detection_confidence=0.5) as face_detection:
  for idx, file in enumerate(IMAGE_FILES):
    image = cv2.imread(file)
    # Convert the BGR image to RGB and process it with MediaPipe Face Detection.
    results = face_detection.process(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))

    # Draw face detections of each face.
    if not results.detections:
      continue
    annotated_image = image.copy()
    for detection in results.detections:
      print('Nose tip:')
      print(mp_face_detection.get_key_point(
          detection, mp_face_detection.FaceKeyPoint.NOSE_TIP))
      mp_drawing.draw_detection(annotated_image, detection)
    cv2.imwrite('/tmp/annotated_image' + str(idx) + '.png', annotated_image)

# For webcam input:
cap = cv2.VideoCapture(0)
with mp_face_detection.FaceDetection(
    model_selection=0, min_detection_confidence=0.5) as face_detection:
  while cap.isOpened():
    success, image = cap.read()
    if not success:
      print("Ignoring empty camera frame.")
      # If loading a video, use 'break' instead of 'continue'.
      continue

    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = face_detection.process(image)

    # Draw the face detection annotations on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    if results.detections:
      for detection in results.detections:
        mp_drawing.draw_detection(image, detection)
    # Flip the image horizontally for a selfie-view display.
    cv2.imshow('MediaPipe Face Detection', cv2.flip(image, 1))
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

*   Running on CPU:
    *   Graph:
        [`mediapipe/graphs/face_detection/face_detection_desktop_live.pbtxt`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/face_detection/face_detection_desktop_live.pbtxt)
    *   Target:
        [`mediapipe/examples/desktop/face_detection:face_detection_cpu`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/examples/desktop/face_detection/BUILD)
*   Running on GPU
    *   Graph:
        [`mediapipe/graphs/face_detection/face_detection_mobile_gpu.pbtxt`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/face_detection/face_detection_mobile_gpu.pbtxt)
    *   Target:
        [`mediapipe/examples/desktop/face_detection:face_detection_gpu`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/examples/desktop/face_detection/BUILD)

### Coral

Please refer to
[these instructions](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/examples/coral/README.md)
to cross-compile and run MediaPipe examples on the
[Coral Dev Board](https://coral.ai/products/dev-board).

## Resources

*   Paper:
    [BlazeFace: Sub-millisecond Neural Face Detection on Mobile GPUs](https://arxiv.org/abs/1907.05047)
    ([presentation](https://docs.google.com/presentation/d/1YCtASfnYyZtH-41QvnW5iZxELFnf0MF-pPWSLGj8yjQ/present?slide=id.g5bc8aeffdd_1_0))
    ([poster](https://drive.google.com/file/d/1u6aB6wxDY7X2TmeUUKgFydulNtXkb3pu/view))
*   [Models and model cards](./models.md#face_detection)
*   [Web demo](https://code.mediapipe.dev/codepen/face_detection)
*   [Python Colab](https://mediapipe.page.link/face_detection_py_colab)
