#include "ofxOnnxRuntime.h"
#include "ofMain.h"

namespace ofxOnnxRuntime
{
#ifdef _MSC_VER
	static std::wstring to_wstring(const std::string &str)
	{
		unsigned len = str.size() * 2;
		setlocale(LC_CTYPE, "");
		wchar_t *p = new wchar_t[len];
		mbstowcs(p, str.c_str(), len);
		std::wstring wstr(p);
		delete[] p;
		return wstr;
	}
#endif

	void BaseHandler::setup(const std::string & onnx_path, const BaseSetting & base_setting)
	{
		Ort::SessionOptions session_options;
		if (base_setting.infer_type == INFER_TENSORRT) {
			OrtTensorRTProviderOptions op;
			memset(&op, 0, sizeof(op));
			op.device_id = base_setting.device_id;
			op.trt_fp16_enable = 1;
			op.trt_engine_cache_enable = 1;
			std::string path = ofToDataPath(onnx_path, true);
			ofStringReplace(path, ".onnx", "_trt_cache");
			op.trt_engine_cache_path = path.c_str();
			session_options.AppendExecutionProvider_TensorRT(op);
		}
		if (base_setting.infer_type == INFER_CUDA || base_setting.infer_type == INFER_TENSORRT) {
			OrtCUDAProviderOptions op;
			op.device_id = base_setting.device_id;
			session_options.AppendExecutionProvider_CUDA(op);
		}
		this->setup2(onnx_path, session_options);
	}

	void BaseHandler::setup2(const std::string & onnx_path, const Ort::SessionOptions & session_options)
	{
		std::string path = ofToDataPath(onnx_path, true);
#ifdef _MSC_VER
		ort_session = std::make_shared<Ort::Session>(ort_env, to_wstring(path).c_str(), session_options);
#else
		ort_session = std::make_shared<Ort::Session>(ort_env, path.c_str(), session_options);
#endif

		Ort::AllocatorWithDefaultOptions allocator;

		// 2. input name & input dims
		input_node_names.resize(1);
        input_node_names_.resize(1);
        input_node_names_[0] = ort_session->GetInputNameAllocated(0, allocator).get();
        input_node_names[0] = input_node_names_[0].data();

		// 3. type info.
		Ort::TypeInfo type_info = ort_session->GetInputTypeInfo(0);
		auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
		input_tensor_size = 1;
		input_node_dims = tensor_info.GetShape();
        for (unsigned int i = 0; i < input_node_dims.size(); ++i) {
            if (input_node_dims.at(i) == -1){
                input_tensor_size = input_tensor_size;
            }else{
                input_tensor_size *= input_node_dims.at(i);
            }
        }
		input_values_handler.resize(input_tensor_size);

		// 4. output names & output dimms
		num_outputs = ort_session->GetOutputCount();
		output_node_names.resize(num_outputs);
        output_node_names_.resize(num_outputs);
		output_node_dims.clear();
		output_values.clear();
		for (unsigned int i = 0; i < num_outputs; ++i)
		{
            output_node_names_[i] = ort_session->GetOutputNameAllocated(i, allocator).get();
            output_node_names[i] = output_node_names_[i].data();
			Ort::TypeInfo output_type_info = ort_session->GetOutputTypeInfo(i);
			auto output_tensor_info = output_type_info.GetTensorTypeAndShapeInfo();
			auto output_dims = output_tensor_info.GetShape();
			output_node_dims.emplace_back(output_dims);
			output_values.emplace_back(nullptr);
		}
	}

	Ort::Value& BaseHandler::run()
	{
		auto input_tensor_ = Ort::Value::CreateTensor<float>(
			memory_info_handler, input_values_handler.data(), input_tensor_size,
			input_node_dims.data(), input_node_dims.size());
		ort_session->Run(Ort::RunOptions{ nullptr }, input_node_names.data(), &input_tensor_, input_node_names.size(),
			output_node_names.data(), output_values.data(), output_node_names.size());

		if (output_values.size() == 1) {
			return output_values.at(0);
		}
		else {
			return dummy_tensor;
		}
	}
}
