# Description:
#   OpenCV libraries for video/image processing on Linux

licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

# Install OpenCV development files with `apt-get install libopencv-dev`.
cc_import(
    name = "opencv_calib3d",
    shared_library = "lib/x86_64-linux-gnu/libopencv_calib3d.so",
)

cc_import(
    name = "opencv_features2d",
    shared_library = "lib/x86_64-linux-gnu/libopencv_features2d.so",
)

cc_import(
    name = "opencv_highgui",
    shared_library = "lib/x86_64-linux-gnu/libopencv_highgui.so",
)

cc_import(
    name = "opencv_imgcodecs",
    shared_library = "lib/x86_64-linux-gnu/libopencv_imgcodecs.so",
)

cc_import(
    name = "opencv_imgproc",
    shared_library = "lib/x86_64-linux-gnu/libopencv_imgproc.so",
)

cc_import(
    name = "opencv_video",
    shared_library = "lib/x86_64-linux-gnu/libopencv_video.so",
)

cc_import(
    name = "opencv_videoio",
    shared_library = "lib/x86_64-linux-gnu/libopencv_videoio.so",
)

cc_import(
    name = "opencv_optflow",
    shared_library = "lib/x86_64-linux-gnu/libopencv_optflow.so",
)

cc_import(
    name = "opencv_core",
    shared_library = "lib/x86_64-linux-gnu/libopencv_core.so",
)

cc_library(
    name = "opencv",
    hdrs = glob([
        # For OpenCV 4.x
        "include/aarch64-linux-gnu/opencv4/opencv2/cvconfig.h",
        "include/arm-linux-gnueabihf/opencv4/opencv2/cvconfig.h",
        "include/x86_64-linux-gnu/opencv4/opencv2/cvconfig.h",
        "include/opencv4/opencv2/**/*.h*",
    ]),
    includes = [
        # For OpenCV 4.x
        "include/aarch64-linux-gnu/opencv4/",
        "include/arm-linux-gnueabihf/opencv4/",
        "include/x86_64-linux-gnu/opencv4/",
        "include/opencv4/",
    ],
    deps = [
        ":opencv_calib3d",
        ":opencv_features2d",
        ":opencv_highgui",
        ":opencv_imgcodecs",
        ":opencv_imgproc",
        ":opencv_video",
        ":opencv_videoio",
        ":opencv_optflow",
        ":opencv_core",
    ],
    visibility = ["//visibility:public"],
)

cc_import(
    name = "egl",
    shared_library = "lib/x86_64-linux-gnu/libEGL.so",
)

cc_import(
    name = "glesv2",
    shared_library = "lib/x86_64-linux-gnu/libGLESv2.so",
)

cc_library(
    name = "egl_gles_headers",
    hdrs = glob([
        "include/EGL/**/*.h",
        "include/GL/**/*.h",
        "include/GLES/**/*.h",
        "include/GLES2/**/*.h",
        "include/GLES3/**/*.h",
        "include/KHR/**/*.h",
    ]),
    includes = ["include"],
    deps = [
        ":egl",
        ":glesv2",
    ],
    visibility = ["//visibility:public"],
)
