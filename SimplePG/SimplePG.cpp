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
	ui->topBox->installEventFilter(this);

	QObjectList list = ui->groupBox->children();
	qDebug() << "Number of children " << list.count();
	buttonList = ui->groupBox->findChildren<QPushButton*>();
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
			opTimer = new QTimer(this);
			connect(opTimer, SIGNAL(timeout()), this, SLOT(transTrig()));
			//with 50ms interval
			opTimer->start(50);
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
		opTimer->stop();
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
		opTimer->stop();
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
			//mii.hbrBack = hBrush;
			mii.fType = 0;

			mii.fState = MF_ENABLED;
			SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_SIZE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MOVE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mii);

			mii.fState = MF_GRAYED;
			//*********attempt to make a dark context menu*****
			//MENUINFO mi = { 0 };
			//mi.cbSize = sizeof(mi);
			//mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
			//mi.hbrBack = CreateSolidBrush(RGB(33, 33, 33));

			//HMENU hMenu = ::GetMenu(hWnd);
			//SetMenuInfo(hMenu, &mi);
			//*******************************
			ICONINFO ii = { 0 };
			//SetIcon
			//	WINDOWPLACEMENT wp;
			//GetWindowPlacement((HWND)this->winId(), &wp);
			//placements relative to the window not the whole screen
			int x = qme->globalX();
			int y = qme->globalY();
			LPARAM cmd = TrackPopupMenu(hMenu, (TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD), x, y, 0, (HWND)this->winId(), nullptr);

			if (cmd)
				PostMessageW((HWND)this->winId(), WM_SYSCOMMAND, WPARAM(cmd), 0);
		}
	}

	//close window
	QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
	if (keyEvent->key() == Qt::Key_Escape)
		SimplePG::close();
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
	peekTimer = new QTimer(this);
	connect(peekTimer, SIGNAL(timeout()), this, SLOT(startPeek()));
	peekTimer->setSingleShot(true);
	startPeek();
}
//first stage after clicking "start"
void SimplePG::startPeek()
{
	if (peek)
	{
		enableClick = true;
		secondRun = false;
		randList.clear();
		charList.clear();
		for (QPushButton* qpb : buttonList)
		{
		again:
			if (randList.size() == randStuff.size())
			{
				if (secondRun)
				{
					break;
				}
				//starting second half
				randList.clear();
				secondRun = true;
				goto again;
			}
			rand = QRandomGenerator::global()->bounded(0, 6);
			//check if rand exists in randList, if not
			//rand get assigned to a button
			//this will guarantee the buttonList buttons order
			//will match randList character's order
			//which will come handy later
			if (!randList.contains(rand))
			{
				randList.append(rand);
				qpb->setText(randStuff[rand]);
				charList.append(randStuff[rand]);
			}
			else
			{
				goto again;
			}
		}
		peek = false;
		peekTimer->start(2000);
	}
	else
	{
		for (QPushButton* qpb : buttonList)
		{
			qpb->setText("*");
		}
		enableClick = true;
		peek = true;
	}
}

void SimplePG::grid_btn_clicked()
{
	//can't click grid buttons till the start button get clicked
	if (!enableClick)
		return;
	if (comboCheck.size() == 2)
		return;
	//first click on any button
	if (comboCheck.size() == 0)
	{
		btn1 = qobject_cast<QPushButton*>(sender());
		btn1->setText(charList[buttonList.indexOf(btn1)]);
		comboCheck.append(charList[buttonList.indexOf(btn1)]);
		//prevent clicking an already correct (combo) buttons
		if (buttonOut.contains(btn1))
			return;
		//turning off only one (clicked) button
		twoDown = false;
		turnOffTimer = new QTimer(this);
		connect(turnOffTimer, SIGNAL(timeout()), this, SLOT(turnOff()));
		turnOffTimer->start(2000);
		turnOffTimer->setSingleShot(true);
		return;
	}
	//second click on any button just after clicking the first
	if (comboCheck.size() == 1)
	{
		btn2 = qobject_cast<QPushButton*>(sender());
		btn2->setText(charList[buttonList.indexOf(btn2)]);
		comboCheck.append(charList[buttonList.indexOf(btn2)]);
		//prevent clicking the same button twice
		if (btn1->objectName() == btn2->objectName())
			return;
		//if the content of the both buttons are the same
		if (comboCheck[0] == comboCheck[1])
		{
			//correct combo => those buttons aren't clickable anymore
			buttonOut.append(btn1);
			buttonOut.append(btn2);
			//if we don't stop the timer, the buttons will turn off (*)
			turnOffTimer->stop();
			//clear combo list
			comboCheck.clear();
		}
		else
		{
			turnOffTimer->stop();
			twoDown = true;
			turnOffTimer = new QTimer(this);
			connect(turnOffTimer, SIGNAL(timeout()), this, SLOT(turnOff()));
			//fast turn off when different chars
			turnOffTimer->start(500);
			turnOffTimer->setSingleShot(true);
		}
	}
}

void SimplePG::turnOff()
{
	if (!twoDown)
	{
		btn1->setText("*");
		comboCheck.clear();
	}
	else
	{
		btn1->setText("*");
		btn2->setText("*");
		comboCheck.clear();
	}
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