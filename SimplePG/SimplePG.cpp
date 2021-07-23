#include "SimplePG.h"
#include "QDebug"
//#include <WinUser.h>
//#include <windef.h>
//#include <WinUser.h>
#include <windows.h>
//#include <winuser.h>
//#include <windef.h>

#define testSpace 0x0010
long miid = 1010;

SimplePG::SimplePG(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::SimplePGClass)
{
	ui->setupUi(this);
	pressed = false;
	ui->centralWidget->installEventFilter(this);

	QObjectList list = ui->groupBox->children();
	qDebug() << "Number of children " << list.count();
	for (QObject* v : list)
	{
		QPushButton* qpb = qobject_cast<QPushButton*>(v);

		connect(qpb, SIGNAL(clicked()), this, SLOT(grid_btn_clicked()));
	}
	LPWSTR test = L"TEST";
	HMENU hMenu = ::GetSystemMenu((HWND)this->winId(), FALSE);
	//if (hMenu != NULL)
	//{
	//	///put item at the end
	//	//::AppendMenuA(hMenu, MF_SEPARATOR, 0, 0);
	//	//::AppendMenuA(hMenu, MF_STRING, testSpace, "test");

	//::InsertMenuA(hMenu, 6, MF_STRING, testSpace, "test");
	//::InsertMenuA(hMenu, 5, MF_SEPARATOR, 0, 0);
	///seems like normal insert like the above doesn't put the item like this
	///Maximize
	///------   (out separator)
	///TEST
	///------
	///Close
	MENUITEMINFO mii, mii2 = { 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fType = MF_SEPARATOR;
	mii.fMask = MIIM_TYPE | MIIM_ID;

	::InsertMenuItem(hMenu, 5, true, &mii);
	mii2.cbSize = sizeof(MENUITEMINFO);
	mii2.fType = MFT_STRING;
	mii2.fMask = MIIM_ID | MIIM_STRING | MIIM_STATE;
	//mii2.fState = MFS_HILITE;
	//mii2.hSubMenu = hMenu;
	mii2.dwTypeData = test;
	mii2.wID = miid;
	mii2.cch = sizeof(test);

	::InsertMenuItem(hMenu, 6, true, &mii2);

	//}

	//WNDCLASSEXW wcex;
	//wcex.cbSize = sizeof(WNDCLASSEX);
	//wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//wcex.lpfnWndProc = WndProc;
	//wcex.cbClsExtra = 0;
	//wcex.cbWndExtra = 0;
	//wcex.hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
	//wcex.hIcon = NULL /* ? (HICON)hIcon : LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1))*/;
	//wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName = NULL;
	//wcex.lpszClassName = L"OPENGLWINDOW";
	//wcex.hIconSm = NULL;
	//RegisterClassExW(&wcex);
}

void SimplePG::grid_btn_clicked()
{
	QPushButton* btn = qobject_cast<QPushButton*>(sender());
	qDebug() << "pressed " << btn->objectName();
}
void SimplePG::mousePressEvent(QMouseEvent* event)
{
	current = event->pos();
}

void SimplePG::mouseMoveEvent(QMouseEvent* event)
{
	if (pressed)
	{
		this->move(mapToParent(event->pos() - current));
		this->setWindowOpacity(0.8);
	}
}

bool SimplePG::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui->centralWidget && event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = (QMouseEvent*)event;
		if (pressed == false)
		{
			current = mouseEvent->pos();
		}
		pressed = true;
		return true;
	}
	if (watched == ui->centralWidget && event->type() == QEvent::MouseButtonRelease)
	{
		pressed = false;
		this->setWindowOpacity(1);
		return true;
	}
	else
		return false;
}

//LRESULT CALLBACK SimplePG::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	qDebug() << "WNDPROCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
//	return 0;
//}

SimplePG::~SimplePG()
{
	delete ui;
}

void SimplePG::on_closeButton_clicked()
{
	qDebug("close");
	SimplePG::close();
}

void SimplePG::on_mainBtn_clicked()
{
	qDebug() << "main button clicked";
}

bool SimplePG::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
	MSG* msg = (MSG*)message;
	if (msg->message == WM_SYSCOMMAND)
	{
		///only works with the first (commented) method
		//if ((msg->wParam & 0xfff0) == testSpace)
		///can't seems to make the above works with the struct method, always returns 0
		if ((msg->wParam) == miid)
		{
			qDebug("TEST CLICKED");
			return true;
		}
	}

	return false;
}

//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	qDebug() << "WNDPROCC";
	return 0;
}