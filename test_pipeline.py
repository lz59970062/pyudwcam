import glob  

import pycalibmar as calibmar  


image_list = glob.glob("CalibCamera/data/left*.jpg") 

print(len(image_list)) 

extractor_options = calibmar.ChessboardFeatureExtractorOptions()
extractor_options.chessboard_rows = 5  # 棋盘格行数（内部角点）
extractor_options.chessboard_columns = 4  # 棋盘格列数
extractor_options.square_size = 0.025  # 每个方格的实际尺寸（米）
extractor_options.fast = True  # 启用快速检测

print(f"Extractor Options: Rows={extractor_options.chessboard_rows}, Cols={extractor_options.chessboard_columns}, SquareSize={extractor_options.square_size}")

extractor = calibmar.ChessboardFeatureExtractor(extractor_options)



calibration = calibmar.Calibration()
image_size = (0, 0)

# 添加3D点到Calibration (只需一次)
# 提取器生成的3D点ID是从0开始的，按顺序添加以确保ID匹配
points3d_map = extractor.Points3D()
print(f"Expected 3D points count: {len(points3d_map)}")
for i in range(len(points3d_map)):
    calibration.AddPoint3D(points3d_map[i])

# 为每张图像提取特征
for path in image_list:
    pixmap = calibmar.Pixmap()
    success = pixmap.Read(path)  # 读取图像
    
    if not success:
        print(f"无法读取图像: {path}")
        continue

    if image_size == (0, 0):
        image_size = (pixmap.Width(), pixmap.Height())
        print(f"Image Size: {image_size}")

    # 创建Image对象
    image = calibmar.Image()
    image.name = path
    
    # 提取特征 (传入image和pixmap)
    # Extract会将提取的2D点和对应的3D点ID写入image对象
    status = extractor.Extract(image, pixmap)
    
    if status == calibmar.FeatureExtractorStatus.SUCCESS:
        # 添加到Calibration
        calibration.AddImage(image)
        print(f"提取成功: {path}, 2D点数量: {len(image.Points2D())}")
    else:
        print(f"提取失败: {path} (Size: {pixmap.Width()}x{pixmap.Height()})")

# --- 标定部分 (平面折射模型) ---
if len(calibration.Images()) > 0:
    print(f"\n开始标定，使用 {len(calibration.Images())} 张图像...")
    print(f"图像尺寸: {image_size}")

    calibrator_options = calibmar.HousingCalibratorOptions()
    
    # 1. 设置相机模型 (这里使用 OpenCV 模型作为示例)
    calibrator_options.camera_model = calibmar.CameraModelType.OpenCVCameraModel
    calibrator_options.image_size = image_size
    
    # 初始化相机参数 (fx, fy, cx, cy, k1, k2, p1, p2)
    w, h = image_size
    f = 1.2 * max(w, h) # 焦距粗略估计
    calibrator_options.camera_params = [f, f, w/2.0, h/2.0, 0.0, 0.0, 0.0, 0.0]
    
    # 2. 设置平面折射模型 (Flat Port)
    calibrator_options.housing_interface = calibmar.HousingInterfaceType.DoubleLayerPlanarRefractive
    
    # 3. 设置初始外壳参数
    # 格式: [Nx, Ny, Nz, distance, thickness, n_air, n_glass, n_water]
    # Nx, Ny, Nz: 接口法向量 (通常初始化为光轴方向 0, 0, 1)
    # distance: 玻璃内表面到相机中心的垂直距离 (米)
    # thickness: 玻璃厚度 (米)
    # n_*: 空气、玻璃、水的折射率
    # --- 修正：针对水上（空气中）图像 ---
    # 因为是在空气中拍摄，不存在折射。我们将所有折射率设为 1.0，模拟光线直线传播。
    # 保持 distance > 0 以避免平面穿过相机光心导致的几何计算错误。
    calibrator_options.initial_housing_params = [0.0, 0.0, 1.0, 0.05, 0.01, 1.0, 1.0, 1.0]
    
    # 4. 设置棋盘格信息 (用于初始化位姿)
    calibrator_options.pattern_cols_rows = (extractor_options.chessboard_columns, extractor_options.chessboard_rows)
    
    # 创建标定器并运行
    calibrator = calibmar.HousingCalibrator(calibrator_options)
    
    try:
        calibrator.Calibrate(calibration)
        print(f"\n标定成功!")
        print(f"RMS 误差: {calibration.CalibrationRms()}")
        print(f"标定后相机参数: {calibration.Camera().params}")
        print(f"标定后外壳参数: {calibration.Camera().refrac_params}")
        # 平面模型外壳参数: [Nx, Ny, Nz, dist, thick, na, ng, nw]
    except Exception as e:
        print(f"\n标定失败: {e}")
else:
    print("\n没有足够的有效图像进行标定。")