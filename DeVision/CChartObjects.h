// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ����

//#import "C:\\Program Files (x86)\\Microsoft Office\\root\\Office16\\EXCEL.EXE" no_namespace
// CChartObjects ��װ����

class CChartObjects : public COleDispatchDriver
{
public:
	CChartObjects() {} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CChartObjects(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CChartObjects(const CChartObjects& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// ChartObjects ����
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
	void _Dummy3()
	{
		InvokeHelper(0x10003, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	VARIANT BringToFront()
	{
		VARIANT result;
		InvokeHelper(0x25a, DISPATCH_METHOD, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}
	VARIANT _Copy()
	{
		VARIANT result;
		InvokeHelper(0xa31, DISPATCH_METHOD, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}
	VARIANT CopyPicture(long Appearance, long Format)
	{
		VARIANT result;
		static BYTE parms[] = VTS_I4 VTS_I4;
		InvokeHelper(0xd5, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms, Appearance, Format);
		return result;
	}
	VARIANT Cut()
	{
		VARIANT result;
		InvokeHelper(0x235, DISPATCH_METHOD, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}
	VARIANT Delete()
	{
		VARIANT result;
		InvokeHelper(0x75, DISPATCH_METHOD, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH Duplicate()
	{
		LPDISPATCH result;
		InvokeHelper(0x40f, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	BOOL get_Enabled()
	{
		BOOL result;
		InvokeHelper(0x258, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
		return result;
	}
	void put_Enabled(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x258, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	double get_Height()
	{
		double result;
		InvokeHelper(0x7b, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, nullptr);
		return result;
	}
	void put_Height(double newValue)
	{
		static BYTE parms[] = VTS_R8;
		InvokeHelper(0x7b, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	void _Dummy12()
	{
		InvokeHelper(0x1000c, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	double get_Left()
	{
		double result;
		InvokeHelper(0x7f, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, nullptr);
		return result;
	}
	void put_Left(double newValue)
	{
		static BYTE parms[] = VTS_R8;
		InvokeHelper(0x7f, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	BOOL get_Locked()
	{
		BOOL result;
		InvokeHelper(0x10d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
		return result;
	}
	void put_Locked(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x10d, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	void _Dummy15()
	{
		InvokeHelper(0x1000f, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	CString get_OnAction()
	{
		CString result;
		InvokeHelper(0x254, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
		return result;
	}
	void put_OnAction(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0x254, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	VARIANT get_Placement()
	{
		VARIANT result;
		InvokeHelper(0x269, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}
	void put_Placement(VARIANT& newValue)
	{
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x269, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, &newValue);
	}
	BOOL get_PrintObject()
	{
		BOOL result;
		InvokeHelper(0x26a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
		return result;
	}
	void put_PrintObject(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x26a, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	VARIANT Select(VARIANT& Replace)
	{
		VARIANT result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0xeb, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms, &Replace);
		return result;
	}
	VARIANT SendToBack()
	{
		VARIANT result;
		InvokeHelper(0x25d, DISPATCH_METHOD, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}
	double get_Top()
	{
		double result;
		InvokeHelper(0x7e, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, nullptr);
		return result;
	}
	void put_Top(double newValue)
	{
		static BYTE parms[] = VTS_R8;
		InvokeHelper(0x7e, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	void _Dummy22()
	{
		InvokeHelper(0x10016, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	BOOL get_Visible()
	{
		BOOL result;
		InvokeHelper(0x22e, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
		return result;
	}
	void put_Visible(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x22e, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	double get_Width()
	{
		double result;
		InvokeHelper(0x7a, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, nullptr);
		return result;
	}
	void put_Width(double newValue)
	{
		static BYTE parms[] = VTS_R8;
		InvokeHelper(0x7a, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	void _Dummy25()
	{
		InvokeHelper(0x10019, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	LPDISPATCH get_ShapeRange()
	{
		LPDISPATCH result;
		InvokeHelper(0x5f8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	void _Dummy27()
	{
		InvokeHelper(0x1001b, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void _Dummy28()
	{
		InvokeHelper(0x1001c, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	BOOL get_ProtectChartObject()
	{
		BOOL result;
		InvokeHelper(0x5f9, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
		return result;
	}
	void put_ProtectChartObject(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x5f9, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	BOOL get_RoundedCorners()
	{
		BOOL result;
		InvokeHelper(0x26b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
		return result;
	}
	void put_RoundedCorners(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x26b, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	LPDISPATCH get_Border()
	{
		LPDISPATCH result;
		InvokeHelper(0x80, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH get_Interior()
	{
		LPDISPATCH result;
		InvokeHelper(0x81, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	BOOL get_Shadow()
	{
		BOOL result;
		InvokeHelper(0x67, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
		return result;
	}
	void put_Shadow(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x67, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}
	VARIANT Copy()
	{
		VARIANT result;
		InvokeHelper(0x227, DISPATCH_METHOD, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH Add(double Left, double Top, double Width, double Height)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_R8 VTS_R8 VTS_R8 VTS_R8;
		InvokeHelper(0xb5, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Left, Top, Width, Height);
		return result;
	}
	long get_Count()
	{
		long result;
		InvokeHelper(0x76, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH Group()
	{
		LPDISPATCH result;
		InvokeHelper(0x2e, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH Item(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0xaa, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	LPUNKNOWN _NewEnum()
	{
		LPUNKNOWN result;
		InvokeHelper(0xfffffffc, DISPATCH_METHOD, VT_UNKNOWN, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH _Default(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}

	// ChartObjects ����
public:

};
