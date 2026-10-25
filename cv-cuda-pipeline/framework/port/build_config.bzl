""".bzl file for desktop MediaPipe proto build configs."""

load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_proto.bzl",
    _mediapipe_cc_proto_library = "mediapipe_cc_proto_library",
    _mediapipe_proto_library = "mediapipe_proto_library",
)

mediapipe_cc_proto_library = _mediapipe_cc_proto_library
mediapipe_proto_library = _mediapipe_proto_library
