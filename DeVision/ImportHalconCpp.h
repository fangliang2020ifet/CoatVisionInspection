#pragma once

#include "HalconCpp.h"
#include "HDevThread.h"

using namespace HalconCpp;

namespace HDevExportCpp
{
	// Parallel execution wrapper for write_image(...) 
	static void* _hcppthread_write_image(void *hcthread);

	static void HalconInitAOP();

	static void HalconOpenGPU(HTuple &hv_DeviceHandle);
}

// Generated stubs for parallel procedure calls. Wrapped in name
// space to avoid name conflicts with actual procedure names
namespace HDevExportCpp
{
	// Parallel execution wrapper for write_image(...) 
	static void* _hcppthread_write_image(void *hcthread)
	{
		// +++ define thread context for this procedure
		HDevThread*  hcppthread = (HDevThread*)hcthread;
		try
		{
			// Input parameters
			const HObject       &cbho_Image = hcppthread->GetInputIconicParamObject(0);
			const HTuple        &cbhv_Format = hcppthread->GetInputCtrlParamTuple(1);
			const HTuple        &cbhv_FillColor = hcppthread->GetInputCtrlParamTuple(2);
			const HTuple        &cbhv_FileName = hcppthread->GetInputCtrlParamTuple(3);

			// Call write_image
			WriteImage(cbho_Image, cbhv_Format, cbhv_FillColor, cbhv_FileName);

			// Reduce reference counter of thread object
			hcppthread->Exit();
			delete hcppthread;

		}
		catch (HException& exc)
		{
			// No exceptions may be raised from stub in parallel case,
			// so we need to store this information prior to cleanup
			bool is_direct_call = hcppthread->IsDirectCall();
			// Attempt to clean up in error case, too
			hcppthread->Exit();
			delete hcppthread;
			// Propagate exception if called directly
			if (is_direct_call)
				throw exc;
		}
		return NULL;
	}

	// Halcon 速度优化
	static void HalconInitAOP()
	{
		HTuple hv_AOP, hv_method;
		//   nil     threshold     linear     mlp  
		hv_method = "mlp";

		//测试：手动AOP优化
		HalconCpp::SetSystem("parallelize_operators", "true");
		HalconCpp::GetSystem("parallelize_operators", &hv_AOP);
		HalconCpp::OptimizeAop("median_image", "byte", "no_file", ((HTuple("file_mode").Append("model")).Append("parameters")),
			((HTuple("nil").Append(hv_method)).Append("false")));
		HalconCpp::OptimizeAop("sub_image", "byte", "no_file", ((HTuple("file_mode").Append("model")).Append("parameters")),
			((HTuple("nil").Append(hv_method)).Append("false")));
		HalconCpp::OptimizeAop("add_image", "byte", "no_file", ((HTuple("file_mode").Append("model")).Append("parameters")),
			((HTuple("nil").Append(hv_method)).Append("false")));
		HalconCpp::OptimizeAop("threshold", "byte", "no_file", ((HTuple("file_mode").Append("model")).Append("parameters")),
			((HTuple("nil").Append(hv_method)).Append("false")));
	}


	static void HalconOpenGPU(HTuple &hv_DeviceHandle)
	{
		//测试3:GPU加速，支持GPU加速的算子Halcon19.11有82个
		//GPU加速是先从CPU中将数据拷贝到GPU上处理，处理完成后再将数据从GPU拷贝到CPU上。从CPU到GPU再从GPU到CPU是要花费时间的。
		//GPU加速一定会比正常的AOP运算速度快吗?不一定!结果取决于显卡的好坏.
		HTuple  hv_DeviceIdentifiers, hv_i, hv_Nmae;

		//dev_update_off();


		HalconCpp::QueryAvailableComputeDevices(&hv_DeviceIdentifiers);
		hv_DeviceHandle = 0;
		{
			HTuple end_val34 = (hv_DeviceIdentifiers.TupleLength()) - 1;
			HTuple step_val34 = 1;
			for (hv_i = 0; hv_i.Continue(end_val34, step_val34); hv_i += step_val34)
			{
				HalconCpp::GetComputeDeviceInfo(HTuple(hv_DeviceIdentifiers[hv_i]), "name", &hv_Nmae);
				//     GeForce GTX 1050 Ti      Intel(R) HD Graphics 630 
				if (0 != (hv_Nmae == HTuple("GeForce GTX 1050 Ti")))
				{
					HalconCpp::OpenComputeDevice(HTuple(hv_DeviceIdentifiers[hv_i]), &hv_DeviceHandle);
					break;
				}
			}
		}

		if (0 != (hv_DeviceHandle != 0))
		{
			HalconCpp::SetComputeDeviceParam(hv_DeviceHandle, "asynchronous_execution", "false");
			HalconCpp::InitComputeDevice(hv_DeviceHandle, "median_image");
			//init_compute_device (DeviceHandle, 'sub_image')
			//init_compute_device (DeviceHandle, 'add_image')
			HalconCpp::ActivateComputeDevice(hv_DeviceHandle);
		}

		//HalconCpp::DeactivateComputeDevice(hv_DeviceHandle);
	}

}
