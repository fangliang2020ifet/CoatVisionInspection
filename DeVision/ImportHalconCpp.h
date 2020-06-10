#pragma once

#include "HalconCpp.h"
#include "HDevThread.h"

using namespace HalconCpp;

namespace HDevExportCpp
{
	// Parallel execution wrapper for write_image(...) 
	static void* _hcppthread_write_image(void *hcthread);
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

}
