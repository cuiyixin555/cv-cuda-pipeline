---
layout: forward
target: https://developers.google.com/mediapipe/solutions/vision/object_detector/
title: Object Detection
parent: MediaPipe Legacy Solutions
nav_order: 9
---

# MediaPipe Object Detection
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
As of March 1, 2023, this solution was upgraded to a new MediaPipe
Solution. For more information, see the
[MediaPipe Solutions](https://developers.google.com/mediapipe/solutions/vision/object_detector/)
site.*

----

## Example Apps

Note: To visualize a graph, copy the graph and paste it into
[MediaPipe Visualizer](https://viz.mediapipe.dev/). For more information on how
to visualize its associated subgraphs, please see
[visualizer documentation](../tools/visualizer.md).

### Desktop

#### Live Camera Input

Please first see general instructions for [desktop](../getting_started/cpp.md)
on how to build MediaPipe examples.

*   Graph:
    [`mediapipe/graphs/object_detection/object_detection_desktop_live.pbtxt`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/object_detection/object_detection_desktop_live.pbtxt)
*   Target:
    [`mediapipe/examples/desktop/object_detection:object_detection_cpu`](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/examples/desktop/object_detection/BUILD)

#### Video File Input

*   With a TFLite Model

    This uses the same
    [TFLite model](https://storage.googleapis.com/mediapipe-assets/ssdlite_object_detection.tflite)
    (see also
    [model info](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/models/object_detection_saved_model/README.md))
    as in [Live Camera Input](#live-camera-input) above. The pipeline is
    implemented in this
    [graph](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/object_detection/object_detection_desktop_tflite_graph.pbtxt),
    which differs from the live-camera-input CPU-based pipeline
    [graph](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/object_detection/object_detection_mobile_cpu.pbtxt)
    simply by the additional `OpenCvVideoDecoderCalculator` and
    `OpenCvVideoEncoderCalculator` at the beginning and the end of the graph
    respectively.

    To build the application, run:

    ```bash
    bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 cv-cuda-pipeline/examples/desktop/object_detection:object_detection_tflite
    ```

    To run the application, replace `<input video path>` and `<output video
    path>` in the command below with your own paths:

    Tip: You can find a test video available in
    `cv-cuda-pipeline/examples/desktop/object_detection`.

    ```
    GLOG_logtostderr=1 bazel-bin/cv-cuda-pipeline/examples/desktop/object_detection/object_detection_tflite \
      --calculator_graph_config_file=cv-cuda-pipeline/graphs/object_detection/object_detection_desktop_tflite_graph.pbtxt \
      --input_side_packets=input_video_path=<input video path>,output_video_path=<output video path>
    ```

*   With a TensorFlow Model

    This uses the
    [TensorFlow model](https://storage.googleapis.com/mediapipe-assets/object_detection_saved_model/archive.zip)
    ( see also
    [model info](https://storage.googleapis.com/mediapipe-assets/object_detection_saved_model/README.md)),
    and the pipeline is implemented in this
    [graph](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/graphs/object_detection/object_detection_mobile_cpu.pbtxt).

    Note: The following runs TensorFlow inference on CPU. If you would like to
    run inference on GPU (Linux only), please follow
    [TensorFlow CUDA Support and Setup on Linux Desktop](../getting_started/gpu_support.md#tensorflow-cuda-support-and-setup-on-linux-desktop)
    instead.

    To build the TensorFlow CPU inference example on desktop, run:

    Note: This command also builds TensorFlow targets from scratch, and it may
    take a long time (e.g., up to 30 mins) for the first time.

    ```bash
    bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 --define no_aws_support=true --linkopt=-s \
    cv-cuda-pipeline/examples/desktop/object_detection:object_detection_tensorflow
    ```

    To run the application, replace `<input video path>` and `<output video
    path>` in the command below with your own paths:

    Tip: You can find a test video available in
    `cv-cuda-pipeline/examples/desktop/object_detection`.

    ```bash
    GLOG_logtostderr=1 bazel-bin/cv-cuda-pipeline/examples/desktop/object_detection/object_detection_tflite \
      --calculator_graph_config_file=cv-cuda-pipeline/graphs/object_detection/object_detection_desktop_tensorflow_graph.pbtxt \
      --input_side_packets=input_video_path=<input video path>,output_video_path=<output video path>
    ```

### Coral

Please refer to
[these instructions](https://github.com/google-ai-edge/mediapipe/tree/master/mediapipe/examples/coral/README.md)
to cross-compile and run MediaPipe examples on the
[Coral Dev Board](https://coral.ai/products/dev-board).

## Resources

*   [Models and model cards](./models.md#object_detection)
