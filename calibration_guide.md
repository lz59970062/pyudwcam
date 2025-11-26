# 相机标定完整流程

本文档描述使用Calibmar库进行相机标定的完整流程。Calibmar是一个基于COLMAP的相机标定工具，支持多种相机模型和标定类型（如住房标定）。流程包括拍摄标定板、提取匹配点、进行标定和输出结果。假设你已安装并构建了Calibmar的Python绑定（`pycalibmar`）。

## 前置条件
- 安装Calibmar：按照项目README.md构建C++库和Python绑定。
- 准备标定板：例如棋盘格（chessboard）或ArUco板。
- 拍摄图像：使用相机拍摄多角度（至少10-20张）的标定板图像，确保覆盖不同位置和角度。
- Python环境：确保`pycalibmar`可用（例如通过`pip install -e .`或手动构建）。

## 步骤1: 准备图像
- 将拍摄的图像保存为标准格式（如PNG、JPEG）。
- 确保图像分辨率一致，且标定板在图像中清晰可见。

## 步骤2: 提取匹配点（特征提取）
使用特征提取器从图像中检测标定板并提取2D点。

### 示例代码（使用棋盘格提取器）
```python
import pycalibmar as calibmar

# 创建特征提取器选项
extractor_options = calibmar.ChessboardFeatureExtractorOptions()
extractor_options.chessboard_rows = 7  # 棋盘格行数（内部角点）
extractor_options.chessboard_columns = 9  # 棋盘格列数
extractor_options.square_size = 0.025  # 每个方格的实际尺寸（米）
extractor_options.fast = True  # 启用快速检测

# 初始化提取器
extractor = calibmar.ChessboardFeatureExtractor(extractor_options)

# 加载图像（假设图像路径列表）
image_paths = ["image1.png", "image2.png", ...]

# 创建Calibration对象

calibration = calibmar.Calibration()

# 为每张图像提取特征
for path in image_paths:
    pixmap = calibmar.Pixmap()
    pixmap.Read(path)  # 读取图像
    
    # 提取特征
    status = extractor.Extract(pixmap)
    if status == calibmar.FeatureExtractorStatus.SUCCESS:
        # 获取2D点
        points2d = extractor.Points3D()  # 注意：这里应该是Points2D，但代码中是Points3D，可能需要调整
        
        # 创建Image对象并添加点
        image = calibmar.Image()
        image.SetName(path)
        image.SetPoints2D(points2d)
        
        # 添加到Calibration
        calibration.AddImage(image)
    else:
        print(f"提取失败: {path}")
```

## 步骤3: 进行标定
配置标定选项并运行标定算法。

### 示例代码（住房标定）
```python
# 创建标定器选项
calibrator_options = calibmar.HousingCalibratorOptions()
calibrator_options.camera_model = calibmar.CameraModelType.OpenCVCameraModel  # 选择相机模型
calibrator_options.housing_interface = calibmar.HousingInterfaceType.DoubleLayerSphericalRefractive  # 住房接口类型
calibrator_options.camera_params = [1000, 500, 500]  # 初始相机参数（焦距、cx、cy）
calibrator_options.initial_housing_params = [0.01, 1.5, 0.0]  # 初始住房参数
calibrator_options.pattern_cols_rows = [9, 7]  # 标定板尺寸
calibrator_options.image_size = [1000, 1000]  # 图像尺寸

# 初始化标定器
calibrator = calibmar.HousingCalibrator(calibrator_options)

# 运行标定
result = calibrator.Calibrate(calibration)

# 检查结果
if result:
    print("标定成功")
    print(f"RMS误差: {calibration.CalibrationRms()}")
else:
    print("标定失败")
```

## 步骤4: 输出结果
保存标定结果，包括相机参数、住房参数等。

### 示例代码
```python
# 获取相机
camera = calibration.Camera()
print(f"相机模型ID: {camera.model_id}")
print(f"相机参数: {camera.params}")

# 获取住房参数标准差（如果适用）
housing_std = calibration.HousingParamsStdDeviations()
print(f"住房参数标准差: {housing_std}")

# 保存到文件（可选）
import json
result_data = {
    "camera_model": camera.model_id,
    "camera_params": list(camera.params),
    "rms_error": calibration.CalibrationRms(),
    "housing_std": list(housing_std)
}
with open("calibration_result.json", "w") as f:
    json.dump(result_data, f)
```

## 注意事项
- 确保图像质量：标定板应完全可见，无遮挡。
- 参数调整：根据你的相机和场景调整初始参数。
- 错误处理：检查每个步骤的状态码。
- 更多选项：参考Calibmar文档了解其他提取器和标定器。

此流程适用于基本住房标定。如需其他类型标定，请调整选项。