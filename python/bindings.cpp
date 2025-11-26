#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>

#include "calibmar/core/calibration.h"
#include "calibmar/core/image.h"
#include "calibmar/core/pixmap.h"
#include "calibmar/calibrators/housing_calibrator.h"
#include "calibmar/calibrators/basic_calibrator.h"
#include "calibmar/extractors/chessboard_extractor.h"
#include "calibmar/core/camera_models.h"

namespace py = pybind11;
using namespace calibmar;

// Helper to convert cv::Mat to/from numpy array
// This is a simplified version. For production, consider using a library like ndarray_converter or similar.
// For now, we will rely on Pixmap::Assign and manual data copying if needed, or just expose Read/Write.
// But users likely want to pass numpy arrays (images loaded with cv2 or PIL).

void bind_colmap_types(py::module& m) {
    py::class_<colmap::Camera>(m, "Camera")
        .def(py::init<>())
        .def_readwrite("model_id", &colmap::Camera::model_id)
        .def_readwrite("width", &colmap::Camera::width)
        .def_readwrite("height", &colmap::Camera::height)
        .def_readwrite("params", &colmap::Camera::params)
        .def_readwrite("refrac_model_id", &colmap::Camera::refrac_model_id)
        .def_readwrite("refrac_params", &colmap::Camera::refrac_params);
        
    // Rigid3d is a bit complex (quaternion + translation), maybe just expose as matrix or similar?
    // colmap::Rigid3d is basically a class with rotation and translation.
    py::class_<colmap::Rigid3d>(m, "Rigid3d")
        .def(py::init<>())
        .def_property("rotation", 
            [](const colmap::Rigid3d& r) { return r.rotation.coeffs(); },
            [](colmap::Rigid3d& r, const Eigen::Vector4d& q) { r.rotation = Eigen::Quaterniond(q); })
        .def_readwrite("translation", &colmap::Rigid3d::translation);
}

PYBIND11_MODULE(pycalibmar, m) {
    m.doc() = "Python bindings for Calibmar";

    bind_colmap_types(m);

    py::enum_<CameraModelType>(m, "CameraModelType")
        .value("SimplePinholeCameraModel", CameraModelType::SimplePinholeCameraModel)
        .value("PinholeCameraModel", CameraModelType::PinholeCameraModel)
        .value("SimpleRadialCameraModel", CameraModelType::SimpleRadialCameraModel)
        .value("RadialCameraModel", CameraModelType::RadialCameraModel)
        .value("OpenCVCameraModel", CameraModelType::OpenCVCameraModel)
        .value("FullOpenCVCameraModel", CameraModelType::FullOpenCVCameraModel)
        .value("OpenCVFisheyeCameraModel", CameraModelType::OpenCVFisheyeCameraModel)
        .export_values();

    py::enum_<HousingInterfaceType>(m, "HousingInterfaceType")
        .value("DoubleLayerSphericalRefractive", HousingInterfaceType::DoubleLayerSphericalRefractive)
        .value("DoubleLayerPlanarRefractive", HousingInterfaceType::DoubleLayerPlanarRefractive)
        .export_values();

    py::class_<Pixmap> pixmap(m, "Pixmap");
    
    py::enum_<Pixmap::ReadMode>(pixmap, "ReadMode")
        .value("GRAYSCALE", Pixmap::ReadMode::GRAYSCALE)
        .value("COLOR", Pixmap::ReadMode::COLOR)
        .value("COLOR_AS_SOURCE", Pixmap::ReadMode::COLOR_AS_SOURCE)
        .export_values();

    pixmap.def(py::init<>())
        .def("Read", &Pixmap::Read, py::arg("path"), py::arg("mode") = Pixmap::ReadMode::COLOR)
        .def("Write", &Pixmap::Write)
        .def("Width", (int (Pixmap::*)()) &Pixmap::Width)
        .def("Height", (int (Pixmap::*)()) &Pixmap::Height);
        // TODO: Add numpy interop

    py::class_<Image>(m, "Image")
        .def(py::init<>())
        .def_property("name", &Image::Name, &Image::SetName)
        .def("AddPoint2D", &Image::AddPoint2D)
        .def("SetPoints2D", &Image::SetPoints2D)
        .def("Points2D", &Image::Points2D)
        .def("SetPoint3DforPoint2D", &Image::SetPoint3DforPoint2D)
        .def("Correspondences", &Image::Correspondences)
        .def("Pose", (colmap::Rigid3d& (Image::*)()) &Image::Pose);

    py::class_<Calibration>(m, "Calibration")
        .def(py::init<>())
        .def("Camera", (colmap::Camera& (Calibration::*)()) &Calibration::Camera)
        .def("AddImage", &Calibration::AddImage)
        .def("Image", (calibmar::Image& (Calibration::*)(size_t)) &Calibration::Image)
        .def("Images", (std::vector<calibmar::Image>& (Calibration::*)()) &Calibration::Images)
        .def("AddPoint3D", &Calibration::AddPoint3D)
        .def("Points3D", (std::map<uint32_t, Eigen::Vector3d>& (Calibration::*)()) &Calibration::Points3D)
        .def("CalibrationRms", &Calibration::CalibrationRms)
        .def("HousingParamsStdDeviations", (std::vector<double>& (Calibration::*)()) &Calibration::HousingParamsStdDeviations);

    py::class_<HousingCalibrator::Options>(m, "HousingCalibratorOptions")
        .def(py::init<>())
        .def_readwrite("camera_model", &HousingCalibrator::Options::camera_model)
        .def_readwrite("housing_interface", &HousingCalibrator::Options::housing_interface)
        .def_readwrite("camera_params", &HousingCalibrator::Options::camera_params)
        .def_readwrite("initial_housing_params", &HousingCalibrator::Options::initial_housing_params)
        .def_readwrite("estimate_initial_dome_offset", &HousingCalibrator::Options::estimate_initial_dome_offset)
        .def_readwrite("pattern_cols_rows", &HousingCalibrator::Options::pattern_cols_rows)
        .def_readwrite("image_size", &HousingCalibrator::Options::image_size);

    py::class_<HousingCalibrator>(m, "HousingCalibrator")
        .def(py::init<const HousingCalibrator::Options&>())
        .def("Calibrate", &HousingCalibrator::Calibrate);

    py::class_<BasicCalibrator::Options>(m, "BasicCalibratorOptions")
        .def(py::init<>())
        .def_readwrite("camera_model", &BasicCalibrator::Options::camera_model)
        .def_readwrite("use_intrinsics_guess", &BasicCalibrator::Options::use_intrinsics_guess)
        .def_readwrite("image_size", &BasicCalibrator::Options::image_size)
        .def_readwrite("fast", &BasicCalibrator::Options::fast);

    py::class_<BasicCalibrator>(m, "BasicCalibrator")
        .def(py::init<const BasicCalibrator::Options&>())
        .def("Calibrate", &BasicCalibrator::Calibrate);

    py::class_<ChessboardFeatureExtractor::Options>(m, "ChessboardFeatureExtractorOptions")
        .def(py::init<>())
        .def_readwrite("chessboard_rows", &ChessboardFeatureExtractor::Options::chessboard_rows)
        .def_readwrite("chessboard_columns", &ChessboardFeatureExtractor::Options::chessboard_columns)
        .def_readwrite("square_size", &ChessboardFeatureExtractor::Options::square_size)
        .def_readwrite("fast", &ChessboardFeatureExtractor::Options::fast);

    py::class_<ChessboardFeatureExtractor>(m, "ChessboardFeatureExtractor")
        .def(py::init<const ChessboardFeatureExtractor::Options&>())
        .def("Extract", &ChessboardFeatureExtractor::Extract)
        .def("Points3D", &ChessboardFeatureExtractor::Points3D);
        
    py::enum_<FeatureExtractor::Status>(m, "FeatureExtractorStatus")
        .value("SUCCESS", FeatureExtractor::Status::SUCCESS)
        .value("DETECTION_ERROR", FeatureExtractor::Status::DETECTION_ERROR)
        .export_values();
}
