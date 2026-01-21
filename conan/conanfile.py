from conan import ConanFile


class OFIQConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("gtest/1.14.0")
        self.requires("opencv/4.5.5")
        self.requires("taocpp-json/1.0.0-beta.13")
        self.requires("magic_enum/0.8.1")
        self.requires("gzip-hpp/0.1.0")

    def configure(self):

        self.options["opencv/*"].gapi = False
        self.options["opencv/*"].objdetect = False
        self.options["opencv/*"].photo = False
        self.options["opencv/*"].stitching = False
        self.options["opencv/*"].video = False
        self.options["opencv/*"].videoio = False
        self.options["opencv/*"].with_ffmpeg = False
        self.options["opencv/*"].with_gtk = False
        self.options["opencv/*"].with_jpeg2000 = "openjpeg"
        self.options["opencv/*"].with_jpeg = "libjpeg-turbo"
        self.options["opencv/*"].with_openexr = False
        self.options["opencv/*"].with_qt = False
        self.options["opencv/*"].with_tiff = False
        self.options["opencv/*"].with_webp = False
        self.options["opencv/*"].with_wayland = False
        self.options["opencv/*"].with_flatbuffers = False

