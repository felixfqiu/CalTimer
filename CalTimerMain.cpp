
#include "Uilib.h"
#pragma comment(lib, "../DuiLib/UiLib_u.lib")

#include "WinUser.h"


//////////////////////////////////////////////////////////////////////////
// Frm类+WinMain+窗口基本

class CCalTimerFrm : public WindowImplBase {
public:
	CDuiString  GetSkinFolder     () override;
	CDuiString  GetSkinFile       () override;
	LPCTSTR     GetWindowClassName() const override;
	void        InitWindow() override;
	void        Notify(TNotifyUI& msg) override;
		// DUI_MSGTYPE_CLICK
		void NotifyClickGenText2Yan();

protected:
	CListUI* day_list_;
};



const char kCalTimerFrm_guid[] = "{4E222FE7-94A7-48DE-95F6-11BDC2CFE80B}";

/**-----------------------------------------------------------------
 * WinMain : CreateMutexA + new CCalTimerFrm()
 * @return 0    : success
 * @return etc. : error code
 */
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow) {

	//if (CreateMutexA(NULL, true, kCalTimerFrm_guid)==NULL || GetLastError()==ERROR_ALREADY_EXISTS) { return 0; }

	SYSTEMTIME st; ::GetSystemTime(&st);

	if ( CCalTimerFrm* pFrame = new CCalTimerFrm() ) {
		CPaintManagerUI::SetInstance(hInstance);
			pFrame->Create(NULL, _T("CalTimerFrm"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE, 0, 0, 500, 1000);
			pFrame->CenterWindow();
			pFrame->ShowWindow(true);
		CPaintManagerUI::MessageLoop();
	}

	return 0;
}



CDuiString CCalTimerFrm::GetSkinFolder() { return _T("Skin"); }
CDuiString CCalTimerFrm::GetSkinFile() { return _T("CalTimer\\CalTimerFrm.xml"); }
LPCTSTR CCalTimerFrm::GetWindowClassName() const { return _T("CalTimerFrm"); }






//////////////////////////////////////////////////////////////////////////
// Win32消息

void CCalTimerFrm::InitWindow()
{
	//
	// 加班说明中常用加班时间的记录格式
	//
	map<int, CDuiString> day_of_week_name_map;
	day_of_week_name_map[0] = _T("(日)");
	day_of_week_name_map[1] = _T("(一)");
	day_of_week_name_map[2] = _T("(二)");
	day_of_week_name_map[3] = _T("(三)");
	day_of_week_name_map[4] = _T("(四)");
	day_of_week_name_map[5] = _T("(五)");
	day_of_week_name_map[6] = _T("(六)");
	// 星期几

	// 几号
	map<int, CDuiString> day_of_month_name_map;
	for (int day=1; day<=9; day++)
	{
		TCHAR tchar[MAX_PATH];
		_stprintf_s(tchar, _T("0%d"), day);
		day_of_month_name_map[day] = tchar;
	}
	for (int day=10; day<=31; day++)
	{
		TCHAR tchar[MAX_PATH];
		_stprintf_s(tchar, _T("%d"), day);
		day_of_month_name_map[day] = tchar;
	}

	// 加班几个小时
	map<int, CDuiString> timer_text_map, timer_userdata_map;
	timer_text_map[ 0] = _T("---"), timer_userdata_map[ 0] = _T("");
	timer_text_map[ 5] = _T("0.5"), timer_userdata_map[ 5] = _T(" 18:00-18:30 0.5 ");
	timer_text_map[10] = _T("1.0"), timer_userdata_map[10] = _T(" 18:00-19:00 1   ");
	timer_text_map[15] = _T("1.5"), timer_userdata_map[15] = _T(" 18:00-19:30 1.5 ");
	timer_text_map[20] = _T("2.0"), timer_userdata_map[20] = _T(" 18:00-20:00 2   ");
	timer_text_map[25] = _T("2.5"), timer_userdata_map[25] = _T(" 18:00-20:30 2.5 ");
	timer_text_map[30] = _T("3.0"), timer_userdata_map[30] = _T(" 18:00-21:00 3   ");
	timer_text_map[35] = _T("3.5"), timer_userdata_map[35] = _T(" 18:00-21:30 3.5 ");
	timer_text_map[40] = _T("4.0"), timer_userdata_map[40] = _T(" 18:00-22:00 4   ");
	timer_text_map[45] = _T("4.5"), timer_userdata_map[45] = _T(" 18:00-22:30 4.5 ");
	timer_text_map[50] = _T("5.0"), timer_userdata_map[50] = _T(" 18:00-23:00 5   ");

	//
	// 取界面指针
	//
	day_list_ = (CListUI*)( m_PaintManager.FindControl(_T("CalTimerList")) );

	//
	// 从xml里取当前月，及 1号 对应是星期几
	//
 	CDuiString month_name = day_list_->GetUserData();
	CDuiString first_days_day_of_week_str = m_PaintManager.FindControl(_T("first_day's_day_of_week"))->GetUserData();
	int first_days_day_of_week_i = atoi( first_days_day_of_week_str.GetStringA().c_str() );

	//
	//  生成列表项
	//
	for (int day=1; day<=31; day++)
	{
		int day_of_week_i = ((day-1)%7 + first_days_day_of_week_i) % 7;

		//
		// day_fullname
		//
		CDuiString day_fullname;
		day_fullname = month_name + day_of_month_name_map[day] + day_of_week_name_map[day_of_week_i];

		//
		// item 
		//
		int todo=0; todo++;
		CListContainerElementUI* item = new CListContainerElementUI();
		item->SetFixedHeight(24);

		// 几月几号
		{
			CButtonUI* p = new CButtonUI();
			p->SetFixedWidth(64);
			p->SetFont(3);
			p->SetTextColor(0x007700);

			p->SetText(day_fullname);

			item->Add(p);
		}

		// 默认加班2.5小时
		for (int hour_time10 = 0; hour_time10 <= 50; hour_time10 += 5)
		{
			COptionUI* p = new COptionUI();			
			p->SetFont(2);
			p->SetSelectedTextColor(0xffffff);

			p->SetGroup(day_fullname);

			p->SetText    ( timer_text_map    [hour_time10] );
			p->SetUserData( timer_userdata_map[hour_time10] );

			item->Add(p);
		}

		// 5、6、7默认不加班
		if ( day_of_week_i == 5 || day_of_week_i == 6 || day_of_week_i == 0 )
		{
			( (COptionUI*) item->GetItemAt(1) )->Selected(true);
		}
		else
		{
			( (COptionUI*) item->GetItemAt(6) )->Selected(true);
		}

		day_list_->Add(item);
	}
}






//////////////////////////////////////////////////////////////////////////
// Ui消息

void CCalTimerFrm::Notify(TNotifyUI& msg) {
	if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0) {
		CDuiString name = msg.pSender->GetName();

		if ( name == _T("Txt2Yan" ) ) { NotifyClickGenText2Yan(); }
	}
}

void CCalTimerFrm::NotifyClickGenText2Yan()
{
	FILE* fp = fopen("10.txt", "wt");

	for (int day=1; day<=31; day++)
	{
		CListContainerElementUI* item = (CListContainerElementUI*) day_list_->GetItemAt(day-1);

		CDuiString timer_text = item->GetItemAt(0)->GetText();

		for (int hour_time10 = 0; hour_time10 <= 50; hour_time10 += 5)
		{
			COptionUI* hour_time_option = (COptionUI*) item->GetItemAt(hour_time10 / 5 +1);
			if ( hour_time_option->IsSelected() )
			{
				timer_text += hour_time_option->GetUserData();

				fprintf_s( fp, "%s\n", timer_text.GetStringA().c_str());
				break;
			}
		}
	}

	fclose(fp);

	::ExitProcess(0);
}