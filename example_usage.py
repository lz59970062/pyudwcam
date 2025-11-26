import pycalibmar
import numpy as np

def main():
    # 1. Initialize Calibration object
    calibration = pycalibmar.Calibration()
    
    # 2. Set up HousingCalibrator Options
    options = pycalibmar.HousingCalibratorOptions()
    
    # Set camera parameters (Example: SimplePinhole f, cx, cy)
    options.camera_params = [864.91, 640, 512]
    
    # Set initial housing parameters
    # For Flat Port: Nx, Ny, Nz, int_dist, int_thick, na, ng, nw
    options.initial_housing_params = [0, 0, 1, 0.04, 0.014, 1.003, 1.473, 1.333]
    
    options.camera_model = pycalibmar.CameraModelType.SimplePinholeCameraModel
    options.housing_interface = pycalibmar.HousingInterfaceType.DoubleLayerPlanarRefractive
    options.pattern_cols_rows = (8, 7)
    options.image_size = (1280, 1024)
    
    # 3. Create Calibrator
    calibrator = pycalibmar.HousingCalibrator(options)
    
    # 4. Add data to calibration (In a real scenario, you would load images and extract corners)
    # Here we just demonstrate the API calls
    
    # Example: Add a 3D point
    point3d_id = calibration.AddPoint3D(np.array([1.0, 1.0, 0.0]))
    
    # Example: Add an image
    image = pycalibmar.Image()
    image.name = "test_image.png"
    
    # Add 2D points (observations)
    # In real usage, these come from feature extraction
    idx = image.AddPoint2D(np.array([100.0, 100.0]))
    
    # Link 2D point to 3D point
    image.SetPoint3DforPoint2D(point3d_id, idx)
    
    calibration.AddImage(image)
    
    print("Starting calibration (this will fail with insufficient data but demonstrates the call)...")
    try:
        calibrator.Calibrate(calibration)
        print("Calibration successful")
        print(f"RMS: {calibration.CalibrationRms()}")
    except Exception as e:
        print(f"Calibration finished (expected error with dummy data): {e}")

if __name__ == "__main__":
    main()
