import pycalibmar
import numpy as np

print("pycalibmar imported successfully")

# Test CameraModelType enum
print(f"CameraModelType.OpenCVCameraModel: {pycalibmar.CameraModelType.OpenCVCameraModel}")

# Test HousingInterfaceType enum
print(f"HousingInterfaceType.DoubleLayerSphericalRefractive: {pycalibmar.HousingInterfaceType.DoubleLayerSphericalRefractive}")

# Test Pixmap
pixmap = pycalibmar.Pixmap()
print("Pixmap created")

# Test Image
image = pycalibmar.Image()
image.name = "test_image"
print(f"Image created with name: {image.name}")

# Test Calibration
calibration = pycalibmar.Calibration()
print("Calibration created")

# Test HousingCalibrator Options
options = pycalibmar.HousingCalibratorOptions()
options.camera_model = pycalibmar.CameraModelType.OpenCVCameraModel
options.housing_interface = pycalibmar.HousingInterfaceType.DoubleLayerSphericalRefractive
print("HousingCalibratorOptions created")

# Test HousingCalibrator
calibrator = pycalibmar.HousingCalibrator(options)
print("HousingCalibrator created")

print("All tests passed!")
