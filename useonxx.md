#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

    try
    {
        // Initialize ONNX Runtime
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "BlazeStudioPose");
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

        // Load model
        const wchar_t *model_path = L"models/pose_estimation_mediapipe_2023mar.onnx";
        Ort::Session session(env, model_path, session_options);

        // Get input/output info
        Ort::AllocatorWithDefaultOptions allocator;
        size_t num_input_nodes = session.GetInputCount();
        size_t num_output_nodes = session.GetOutputCount();

        qDebug() << "Model loaded successfully";
        qDebug() << "Number of inputs:" << num_input_nodes;
        qDebug() << "Number of outputs:" << num_output_nodes;

        // Load and preprocess image
        cv::Mat frame = cv::imread("p2.jpg");
        if (frame.empty())
        {
            qDebug() << "Failed to load image!";
            return -1;
        }

        // Resize and normalize
        cv::Mat resized;
        cv::resize(frame, resized, cv::Size(256, 256));
        resized.convertTo(resized, CV_32F, 1.0 / 255.0);

        // Convert to RGB (ONNX models usually expect RGB)
        cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);

        // Prepare input tensor
        std::vector<int64_t> input_shape{1, 256, 256, 3};
        size_t input_tensor_size = 1 * 256 * 256 * 3;
        std::vector<float> input_tensor_values(input_tensor_size);

        // Copy image data to tensor (HWC format)
        for (int h = 0; h < 256; ++h)
        {
            for (int w = 0; w < 256; ++w)
            {
                for (int c = 0; c < 3; ++c)
                {
                    input_tensor_values[h * 256 * 3 + w * 3 + c] =
                        resized.at<cv::Vec3f>(h, w)[c];
                }
            }
        }

        // Create input tensor
        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(
            OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
            memory_info, input_tensor_values.data(), input_tensor_size,
            input_shape.data(), input_shape.size());

        // Get input/output names
        auto input_name_raw = session.GetInputNameAllocated(0, allocator);
        auto output_name_raw = session.GetOutputNameAllocated(0, allocator);
        const char *input_names[] = {input_name_raw.get()};
        const char *output_names[] = {output_name_raw.get()};

        // Run inference
        auto output_tensors = session.Run(Ort::RunOptions{nullptr},
                                          input_names, &input_tensor, 1, output_names, 1);

        // Process output
        float *floatarr = output_tensors[0].GetTensorMutableData<float>();
        auto output_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();

        qDebug() << "Inference completed";
        qDebug() << "Output shape dimensions:" << output_shape.size();
        qDebug() << "Output shape:";
        for (size_t i = 0; i < output_shape.size(); ++i) {
            qDebug() << "  Dimension" << i << ":" << output_shape[i];
        }

        // Calculate total elements
        int64_t total_elements = 1;
        for (auto dim : output_shape) {
            total_elements *= dim;
        }
        qDebug() << "Total output elements:" << total_elements;

        // Print first 100 values to see what we got
        qDebug() << "\nFirst 100 output values:";
        for (int i = 0; i < std::min(100LL, total_elements); ++i) {
            if (i % 10 == 0) qDebug() << "";  // New line every 10 values
            qDebug() << floatarr[i];
        }

        // Try to interpret as landmarks (33 landmarks, each with multiple values)
        qDebug() << "\n=== Attempting to parse as landmarks ===";
        int num_landmarks = 33;
        int values_per_landmark = total_elements / num_landmarks;
        qDebug() << "Values per landmark:" << values_per_landmark;

        // Calculate scale factors (model output is for 256x256, scale to original frame size)
        float scale_x = static_cast<float>(frame.cols) / 256.0f;
        float scale_y = static_cast<float>(frame.rows) / 256.0f;

        qDebug() << "Frame size:" << frame.cols << "x" << frame.rows;
        qDebug() << "Scale factors: x=" << scale_x << ", y=" << scale_y;

        // Draw landmarks on original frame
        for (int i = 0; i < num_landmarks; ++i)
        {
            float x = floatarr[i * values_per_landmark];     // X em pixels (256x256)
            float y = floatarr[i * values_per_landmark + 1]; // Y em pixels (256x256)

            qDebug() << "Landmark" << i << ": x=" << x << ", y=" << y;

            // Scale from 256x256 to original frame size
            int px = static_cast<int>(x * scale_x);
            int py = static_cast<int>(y * scale_y);

            qDebug() << "  -> Scaled to:" << px << "," << py;

            cv::circle(frame, cv::Point(px, py), 5, cv::Scalar(0, 255, 0), -1);
            cv::putText(frame, std::to_string(i), cv::Point(px + 5, py),
                       cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255, 0, 0), 1);
        }

        cv::imshow("BlazePose C++ with ONNX Runtime", frame);
    }
    catch (const Ort::Exception &e)
    {
        qDebug() << "ONNX Runtime error:" << e.what();
        return -1;
    }
    catch (const std::exception &e)
    {
        qDebug() << "Error:" << e.what();
        return -1;
    }
