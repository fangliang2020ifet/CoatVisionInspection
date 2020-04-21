// 从类型库向导中用“添加类”创建的计算机生成的 IDispatch 包装器类

//#import "C:\\Program Files (x86)\\Microsoft Office\\root\\Office16\\EXCEL.EXE" no_namespace
// CCharts 包装器类

class CCharts : public COleDispatchDriver
{
public:
	CCharts() {} // 调用 COleDispatchDriver 默认构造函数
	CCharts(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CCharts(const CCharts& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// 特性
public:

	// 操作
public:


	// Charts 方法
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
	LPDISPATCH Add(VARIANT& Before, VARIANT& After, VARIANT& Count)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0xb5, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Before, &After, &Count);
		return result;
	}
	void Copy(VARIANT& Before, VARIANT& After)
	{
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0x227, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &Before, &After);
	}
	long get_Count()
	{
		long result;
		InvokeHelper(0x76, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
		return result;
	}
	void Delete()
	{
		InvokeHelper(0x75, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void _Dummy7()
	{
		InvokeHelper(0x10007, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	LPDISPATCH get_Item(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0xaa, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	void Move(VARIANT& Before, VARIANT& After)
	{
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0x27d, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &Before, &After);
	}
	LPUNKNOWN get__NewEnum()
	{
		LPUNKNOWN result;
		InvokeHelper(0xfffffffc, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, nullptr);
		return result;
	}
	void __PrintOut(VARIANT& From, VARIANT& To, VARIANT& Copies, VARIANT& Preview, VARIANT& ActivePrinter, VARIANT& PrintToFile, VARIANT& Collate)
	{
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0x389, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &From, &To, &Copies, &Preview, &ActivePrinter, &PrintToFile, &Collate);
	}
	void PrintPreview(VARIANT& EnableChanges)
	{
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x119, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &EnableChanges);
	}
	void Select(VARIANT& Replace)
	{
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0xeb, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &Replace);
	}
	LPDISPATCH get_HPageBreaks()
	{
		LPDISPATCH result;
		InvokeHelper(0x58a, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	LPDISPATCH get_VPageBreaks()
	{
		LPDISPATCH result;
		InvokeHelper(0x58b, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
		return result;
	}
	VARIANT get_Visible()
	{
		VARIANT result;
		InvokeHelper(0x22e, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}
	void put_Visible(VARIANT& newValue)
	{
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x22e, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, &newValue);
	}
	LPDISPATCH get__Default(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	void _PrintOut(VARIANT& From, VARIANT& To, VARIANT& Copies, VARIANT& Preview, VARIANT& ActivePrinter, VARIANT& PrintToFile, VARIANT& Collate, VARIANT& PrToFileName)
	{
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0x6ec, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &From, &To, &Copies, &Preview, &ActivePrinter, &PrintToFile, &Collate, &PrToFileName);
	}
	void PrintOut(VARIANT& From, VARIANT& To, VARIANT& Copies, VARIANT& Preview, VARIANT& ActivePrinter, VARIANT& PrintToFile, VARIANT& Collate, VARIANT& PrToFileName)
	{
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0x939, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &From, &To, &Copies, &Preview, &ActivePrinter, &PrintToFile, &Collate, &PrToFileName);
	}
	LPDISPATCH Add2(VARIANT& Before, VARIANT& After, VARIANT& Count, VARIANT& NewLayout)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
		InvokeHelper(0xbee, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Before, &After, &Count, &NewLayout);
		return result;
	}

	// Charts 属性
public:

};
