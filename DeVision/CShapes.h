// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ����

//#import "C:\\Program Files (x86)\\Microsoft Office\\root\\Office16\\EXCEL.EXE" no_namespace
// CShapes ��װ����

class CShapes : public COleDispatchDriver
{
public:
	CShapes() {} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CShapes(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CShapes(const CShapes& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// Shapes ����
public:
	LPDISPATCH get_Application()
	{
		LPDISPATCH result;
		InvokeHelper(0x94, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	long get_Creator()
	{
		long result;
		InvokeHelper(0x95, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH get_Parent()
	{
		LPDISPATCH result;
		InvokeHelper(0x96, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	long get_Count()
	{
		long result;
		InvokeHelper(0x76, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH Item(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0xaa, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	LPDISPATCH _Default(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	LPUNKNOWN get__NewEnum()
	{
		LPUNKNOWN result;
		InvokeHelper(0xfffffffc, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH AddCallout(long Type, float Left, float Top, float Width, float Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x6b1, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Type, Left, Top, Width, Height);
		return result;
	}
	LPDISPATCH AddConnector(long Type, float BeginX, float BeginY, float EndX, float EndY)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x6b2, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Type, BeginX, BeginY, EndX, EndY);
		return result;
	}
	LPDISPATCH AddCurve(VARIANT& SafeArrayOfPoints)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x6b7, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &SafeArrayOfPoints);
		return result;
	}
	LPDISPATCH AddLabel(long Orientation, float Left, float Top, float Width, float Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x6b9, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Orientation, Left, Top, Width, Height);
		return result;
	}
	LPDISPATCH AddLine(float BeginX, float BeginY, float EndX, float EndY)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x6ba, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, BeginX, BeginY, EndX, EndY);
		return result;
	}
	LPDISPATCH AddPicture(LPCTSTR Filename, long LinkToFile, long SaveWithDocument, float Left, float Top, float Width, float Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x6bb, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Filename, LinkToFile, SaveWithDocument, Left, Top, Width, Height);
		return result;
	}
	LPDISPATCH AddPolyline(VARIANT& SafeArrayOfPoints)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x6be, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &SafeArrayOfPoints);
		return result;
	}
	LPDISPATCH AddShape(long Type, float Left, float Top, float Width, float Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x6bf, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Type, Left, Top, Width, Height);
		return result;
	}
	LPDISPATCH AddTextEffect(long PresetTextEffect, LPCTSTR Text, LPCTSTR FontName, float FontSize, long FontBold, long FontItalic, float Left, float Top)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_R4 VTS_I4 VTS_I4 VTS_R4 VTS_R4;
		InvokeHelper(0x6c0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, PresetTextEffect, Text, FontName, FontSize, FontBold, FontItalic, Left, Top);
		return result;
	}
	LPDISPATCH AddTextbox(long Orientation, float Left, float Top, float Width, float Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x6c6, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Orientation, Left, Top, Width, Height);
		return result;
	}
	LPDISPATCH BuildFreeform(long EditingType, float X1, float Y1)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_R4;
		InvokeHelper(0x6c7, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, EditingType, X1, Y1);
		return result;
	}
	LPDISPATCH get_Range(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0xc5, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	void SelectAll()
	{
		InvokeHelper(0x6c9, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	LPDISPATCH AddFormControl(long Type, long Left, long Top, long Width, long Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4;
		InvokeHelper(0x6ca, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Type, Left, Top, Width, Height);
		return result;
	}
	LPDISPATCH AddOLEObject(VARIANT& ClassType, VARIANT& Filename, VARIANT& Link, VARIANT& DisplayAsIcon, VARIANT& IconFileName, VARIANT& IconIndex, VARIANT& IconLabel, VARIANT& Left, VARIANT& Top, VARIANT& Width, VARIANT& Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0x6cb, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &ClassType, &Filename, &Link, &DisplayAsIcon, &IconFileName, &IconIndex, &IconLabel, &Left, &Top, &Width, &Height);
		return result;
	}
	LPDISPATCH AddDiagram(long Type, float Left, float Top, float Width, float Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x880, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Type, Left, Top, Width, Height);
		return result;
	}
	LPDISPATCH AddCanvas(float Left, float Top, float Width, float Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x881, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Left, Top, Width, Height);
		return result;
	}
	LPDISPATCH AddChart(VARIANT& XlChartType, VARIANT& Left, VARIANT& Top, VARIANT& Width, VARIANT& Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0xa69, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &XlChartType, &Left, &Top, &Width, &Height);
		return result;
	}
	LPDISPATCH AddSmartArt(LPDISPATCH Layout, VARIANT& Left, VARIANT& Top, VARIANT& Width, VARIANT& Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_DISPATCH VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0xb68, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Layout, &Left, &Top, &Width, &Height);
		return result;
	}
	LPDISPATCH AddChart2(VARIANT& Style, VARIANT& XlChartType, VARIANT& Left, VARIANT& Top, VARIANT& Width, VARIANT& Height, VARIANT& NewLayout)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0xc10, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Style, &XlChartType, &Left, &Top, &Width, &Height, &NewLayout);
		return result;
	}
	LPDISPATCH AddPicture2(LPCTSTR Filename, long LinkToFile, long SaveWithDocument, float Left, float Top, float Width, float Height, long Compress)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_I4;
		InvokeHelper(0xc57, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Filename, LinkToFile, SaveWithDocument, Left, Top, Width, Height, Compress);
		return result;
	}

	// Shapes ����
public:

};
