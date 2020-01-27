from conans import ConanFile, CMake

class StringChallenge(ConanFile):
    settings = 'os', 'arch', 'compiler', 'build_type'
    requires = "gtest/1.8.1@bincrafters/stable", "benchmark/1.5.0"
    generators = "cmake_find_package" # can be updated to cmake_find_package_multi if necessary

    def imports(self):
        self.copy("*.so", "bin", "lib")
        self.copy("*.dll", "bin", "bin")
        self.copy("*.dylib", "bin", "lib")

    def build(self):
        cmake = CMake(self, generator='Ninja')
        cmake.configure()
        cmake.build()
        cmake.test() # If you dont have any tests this will fail!