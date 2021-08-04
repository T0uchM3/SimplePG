#include "SimplePG.h"
#include "QDebug"
#include <windows.h>
#include <dwmapi.h>
#include <wingdi.h>

#define testSpace 0x0010
long miid = 1010;
static QColor m_clear_color = QColor(Qt::white);

SimplePG::SimplePG(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::SimplePGClass)
{
	ui->setupUi(this);
	pressed = false;
	//ui->centralWidget->installEventFilter(this);
	ui->topBox->installEventFilter(this);

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

	correctionVal = QPoint(-7, 0);
	//}
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
		this->move(mapToParent(event->pos() - current) - correctionVal);
		if (trigOnce)
		{
			//keep calling transTrig
			timer = new QTimer(this);
			connect(timer, SIGNAL(timeout()), this, SLOT(transTrig()));
			//with 50ms interval
			timer->start(50);
			//once each hold
			trigOnce = false;
		}
	}
}
void SimplePG::transTrig()
{
	//with no interruption this runs 5 times
	//with each run we reduce the window's opacity
	//to create kinda of smooth transition
	opCounter++;
	if (opCounter == 5)
		timer->stop();
	opacityVal = opacityVal - 0.05;
	this->setWindowOpacity(opacityVal);
}
bool SimplePG::eventFilter(QObject* watched, QEvent* event)
{
	QMouseEvent* qme = static_cast<QMouseEvent*>(event);
	if (watched == ui->topBox && event->type() == QEvent::MouseButtonPress && qme->button() == Qt::LeftButton)
	{
		QMouseEvent* mouseEvent = (QMouseEvent*)event;
		if (pressed == false)
		{
			current = mouseEvent->pos();
		}
		pressed = true;
		return true;
	}
	if (watched == ui->topBox && event->type() == QEvent::MouseButtonRelease && qme->button() == Qt::LeftButton)
	{
		pressed = false;
		//reset and halt transition when release
		this->setWindowOpacity(1);
		timer->stop();
		opacityVal = 1.0;
		opCounter = 0;
		trigOnce = true;
		return true;
	}
	if (watched == ui->topBox && event->type() == QEvent::MouseButtonRelease && qme->button() == Qt::RightButton)
	{
		//trigger menu only if the release event happened inside topBox
		//get the rect of topbox and check if the mouse pos is inside it
		if (ui->topBox->rect().contains(qme->localPos().toPoint()))
		{
			HMENU hMenu = ::GetSystemMenu((HWND)this->winId(), FALSE);

			if (!hMenu)
				return false;

			MENUITEMINFOW mii;
			mii.cbSize = sizeof(MENUITEMINFOW);
			mii.fMask = MIIM_STATE;
			mii.fType = 0;

			mii.fState = MF_ENABLED;
			SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_SIZE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MOVE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mii);

			mii.fState = MF_GRAYED;

			WINDOWPLACEMENT wp;
			GetWindowPlacement((HWND)this->winId(), &wp);
			//placements relative to the window not the whole screen
			int x = qme->globalX();
			int y = qme->globalY();
			LPARAM cmd = TrackPopupMenu(hMenu, (TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD), x, y, 0, (HWND)this->winId(), nullptr);

			if (cmd)
				PostMessageW((HWND)this->winId(), WM_SYSCOMMAND, WPARAM(cmd), 0);
		}
	}

	else
		return false;
}

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//qDebug() << "WNDPROCC";
	PAINTSTRUCT		ps;
	if (message == WM_NCPAINT)
	{
		HDC hdc = GetWindowDC(hWnd);
		qDebug() << "WM_PPAAAAINT";
	}
	LRESULT lRet = 0;
	if (message == WM_ACTIVATE)
	{
		qDebug("WM_ACTIVEATE");
		LPWSTR s = L"hello world!";

		SetWindowTextW(hWnd, s);
	}
	if (message == WM_CREATE)
	{
		qDebug("WM_CREATE");
	}
	if (message == WM_CONTEXTMENU)
	{
		qDebug("WM_CONTEXTMENU");
	}
	return 0;
}