> **NOTE**: This repository is hosted on [Gitlab](https://cau-git.rz.uni-kiel.de/inf-ag-koeser/calibmar) and mirrored to [Github](https://github.com/MDSKiel/calibmar). Please use Github for [issues](https://github.com/MDSKiel/calibmar/issues) and get the [releases](https://cau-git.rz.uni-kiel.de/inf-ag-koeser/calibmar/-/releases) on Gitlab.

# Calibmar

Calibmar is a camera and underwater housing calibration tool.

Features include:
- Camera housing calibration based on '[Refractive Geometry for Underwater Domes](https://doi.org/10.1016/j.isprsjprs.2021.11.006)'
	- with a Flat Port model from '[Refractive Calibration of Underwater Cameras](https://doi.org/10.1007/978-3-642-33715-4_61)'
- Camera and stereo camera calibration 
- Calibration guidance implementation of '[Calibration Wizard](https://doi.org/10.1109/iccv.2019.00158)'
- [COLMAP](https://colmap.github.io/) compliant camera models

## Install

Binaries for Windows and Linux are available at https://cau-git.rz.uni-kiel.de/inf-ag-koeser/calibmar/-/releases.

## Build from Source

### CUDA

To build with CUDA support install the latest CUDA from NVIDIA's homepage.

You will also need to specify a CUDA architecture during cmake configuration as e.g. `-DCMAKE_CUDA_ARCHITECTURES=native`.

There is a known error for Windows, when installing CUDA together with Visual Studio Build Tools (as opposed to Visual Studio IDE), where CUDA does not properly integrate into the build tools and some files have to be manually copied.

### Linux

The following build has been tested under Ubuntu 22.04.

Dependencies from default Ubuntu repositories:

    sudo apt-get install \
        git \
        cmake \
        build-essential \
        libboost-program-options-dev \
        libboost-filesystem-dev \
        libboost-graph-dev \
        libboost-system-dev \
        libboost-test-dev \
        libeigen3-dev \
        libflann-dev \
        libfreeimage-dev \
        libmetis-dev \
        libgoogle-glog-dev \
        libglew-dev \
        libsqlite3-dev \
        qtbase5-dev \
        libqt5opengl5-dev \
        libcgal-dev \
        libceres-dev \
        libopencv-dev

Configure and compile Calibmar:

	cd path/to/calibmar
    mkdir build
    cd build
    cmake .. -GNinja
    ninja

### Windows

For Windows it is recommended to use [vcpkg](https://github.com/microsoft/vcpkg). You will also need Visual Studio Build Tools.

    git clone https://github.com/microsoft/vcpkg
    cd vcpkg
    bootstrap-vcpkg.sh
    vcpkg install colmap[cuda]:x64-windows
    vcpkg install opencv[contrib]:x64-windows

This will take a while.

Configure and compile Calibmar:

	cd path/to/calibmar
    mkdir build
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
    cmake --build . --config Release

CMake Presets are available which are supported by several IDEs. Presets for windows expect the environment variable `VCPKG_ROOT` to point to the vcpkg root directory.

### Cite
If you found this work to be useful in your own research, please considering citing:
```
@InProceedings{seegraeber2025calibration,
  author    = {Felix Seegräber and Mengun She and Felix Woelk and Kevin Köser},
  booktitle = {ICCV Joint Workshop on Marine Vision},
  title     = {{A Calibration Tool for Refractive Underwater Vision}},
  year      = {2025},
}
```

## Python Bindings

This repository now includes Python bindings for easy integration with Python workflows.

### Python Installation

#### Prerequisites

Install system dependencies as described in the "Build from Source" section above, plus:

```bash
pip install pybind11 numpy
```

#### Build and Install

```bash
# Clone the repository
git clone https://github.com/MDSKiel/calibmar.git
cd calibmar

# Use the automated install script
chmod +x install.sh
./install.sh

# Or manual installation
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CUDA_ARCHITECTURES=86 -DCALIBMAR_PYTHON_BINDINGS=ON -DCALIBMAR_TESTS_ENABLED=OFF
make -j$(nproc)
cd ..
pip install .
```

### Python Usage

```python
import pycalibmar
import numpy as np

# Initialize calibration
calibration = pycalibmar.Calibration()

# Set up calibrator options
options = pycalibmar.HousingCalibratorOptions()
options.camera_params = [864.91, 640, 512]
options.initial_housing_params = [0, 0, 1, 0.04, 0.014, 1.003, 1.473, 1.333]
options.camera_model = pycalibmar.CameraModelType.SimplePinholeCameraModel
options.housing_interface = pycalibmar.HousingInterfaceType.DoubleLayerPlanarRefractive
options.pattern_cols_rows = (8, 7)
options.image_size = (1280, 1024)

# Create calibrator
calibrator = pycalibmar.HousingCalibrator(options)

# Add your calibration data here...
# (See example_usage.py for a complete example)

# Calibrate
calibrator.Calibrate(calibration)
print(f"RMS: {calibration.CalibrationRms()}")
```

### Python API Reference

#### Main Classes
- `Calibration`: Container for calibration data
- `Image`: Single image data with 2D points and correspondences
- `HousingCalibrator`: Performs the calibration optimization
- `ChessboardFeatureExtractor`: Extracts chessboard corners

#### Enums
- `CameraModelType`: Camera model types (OpenCV, Pinhole, etc.)
- `HousingInterfaceType`: Housing interface types (Flat Port, Dome Port)

See `example_usage.py` and `test_bindings.py` for detailed usage examples.