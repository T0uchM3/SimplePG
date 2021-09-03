#include "SimplePG.h"
#include "QDebug"
#include <windows.h>
#include <dwmapi.h>
#include <wingdi.h>
#include <QXmlStreamReader>
#include <QFileDialog>
#define testSpace 0x0010
//random id for the new added item (clear score)
long miid = 1010;
static QColor m_clear_color = QColor(Qt::white);

SimplePG::SimplePG(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::SimplePGClass)
{
	ui->setupUi(this);
	pressed = false;
	ui->topBox->installEventFilter(this);

	QObjectList list = ui->groupBox->children();
	qDebug() << "Number of children " << list.count();
	buttonList = ui->groupBox->findChildren<QPushButton *>();
	for (QObject *v : list)
	{
		auto qpb = qobject_cast<QPushButton *>(v);
		connect(qpb, SIGNAL(clicked()), this, SLOT(grid_btn_clicked()));
	}
	LPWSTR cScore = L"Clear Score";
	HMENU hMenu = GetSystemMenu((HWND)this->winId(), FALSE);
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
	//adding a line separator
	::InsertMenuItem(hMenu, 5, true, &mii);
	mii2.cbSize = sizeof(MENUITEMINFO);
	mii2.fType = MFT_STRING;
	mii2.fMask = MIIM_ID | MIIM_STRING | MIIM_STATE;
	//mii2.fState = MFS_HILITE;
	//mii2.hSubMenu = hMenu;
	mii2.dwTypeData = cScore;
	mii2.wID = miid;
	mii2.cch = sizeof(cScore);
	//adding the button
	::InsertMenuItem(hMenu, 6, true, &mii2);

	correctionVal = QPoint(-7, 0);
	//loading scores at startup
	loadingScore();
	//}
}

void SimplePG::mousePressEvent(QMouseEvent *event)
{
	current = event->pos();
}

void SimplePG::mouseMoveEvent(QMouseEvent *event)
{
	if (pressed)
	{
		this->move(mapToParent(event->pos() - current) - correctionVal);
		dragging = true;
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

//for fading
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
	transTriggered = true;
}

bool SimplePG::eventFilter(QObject *watched, QEvent *event)
{
	auto qme = static_cast<QMouseEvent *>(event);
	if (watched == ui->topBox && event->type() == QEvent::MouseButtonPress && qme->button() == Qt::LeftButton)
	{
		auto mouseEvent = static_cast<QMouseEvent *>(event);
		//first run this will be false
		if (pressed == false)
		{
			//capture the mouse pos
			current = mouseEvent->pos();
		}
		//marking pressed as true will make it possible for other events(release, drag) to know that the mouse got pressed
		pressed = true;
		return true;
	}
	if (watched == ui->topBox && event->type() == QEvent::MouseButtonRelease && qme->button() == Qt::LeftButton && pressed)
	{
		//no need to reset/do anything if no drag happened...
		if (!dragging)
		{
			//bug demo//
			//remove "pressed = false" -> click(press + release) title bar once-> go to other location in title bar and start dragging
			pressed = false;
			return false;
		}
		pressed = false;
		//reset and halt transition when release
		this->setWindowOpacity(1);
		opTimer->stop();
		opacityVal = 1.0;
		opCounter = 0;
		trigOnce = true;
		dragging = false;
		return true;
	}
	if (watched == ui->topBox && event->type() == QEvent::MouseButtonRelease && qme->button() == Qt::RightButton)
	{
		//trigger menu only if the release event happened inside topBox
		//get the rect of topbox and check if the mouse pos is inside it
		if (ui->topBox->rect().contains(qme->localPos().toPoint()))
		{
			HMENU hMenu = GetSystemMenu((HWND)this->winId(), FALSE);

			if (!hMenu)
				return false;
			//UTF-16
			MENUITEMINFOW mii;
			mii.cbSize = sizeof(MENUITEMINFOW);
			mii.fMask = MIIM_STATE;
			//mii.hbrBack = hBrush;
			mii.fType = 0;

			mii.fState = MF_ENABLED;
			//setting up the context menu
			SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_SIZE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MOVE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mii);
			SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mii);
			//graying these options since they're not needed
			EnableMenuItem(hMenu, SC_MAXIMIZE, MF_GRAYED);
			EnableMenuItem(hMenu, SC_MOVE, MF_GRAYED);
			EnableMenuItem(hMenu, SC_SIZE, MF_GRAYED);
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

			///***spawning the context menu (right click)***///
			//placements relative to the window not the whole screen
			int x = qme->globalX();
			int y = qme->globalY();
			LPARAM cmd = TrackPopupMenu(hMenu, (TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD), x, y, 0,
				(HWND)this->winId(), nullptr);

			if (cmd)
			{
				//sending system menu (context menu) messages to the nativeEvent
				PostMessageW((HWND)this->winId(), WM_SYSCOMMAND, static_cast<WPARAM>(cmd), 0);
				return true;
			}
			return true;
		}
	}

	//close window
	auto keyEvent = static_cast<QKeyEvent *>(event);
	if (keyEvent->key() == Qt::Key_Escape)
		close();
	else
		return false;
	return false;
}

SimplePG::~SimplePG()
{
	delete ui;
}

void SimplePG::on_closeButton_clicked()
{
	qDebug("close");
	close();
}

//clicking start button
void SimplePG::on_mainBtn_clicked()
{
	if (!cancel)
	{
		qDebug() << "main button clicked";
		//timer for hiding contents after they show up
		peekTimer = new QTimer(this);
		connect(peekTimer, SIGNAL(timeout()), this, SLOT(startPeek()));
		peekTimer->setSingleShot(true);
		//get a normal call just to show contents
		startPeek();
		counter = new QTimer(this);
		connect(counter, SIGNAL(timeout()), this, SLOT(UpdateTime()));
		ui->mainBtn->setDisabled(true);
		//updateScore();

		return;
	}
	finish(false);
}

//parsing existing xml file for already stored scores
void SimplePG::loadingScore()
{
	QXmlStreamReader xml;

	QString filename = QCoreApplication::applicationDirPath() + "/score.xml";
	QFile file(filename);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		qDebug() << "error loading XML";
		return;
	}
	xml.setDevice(&file);
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement())
		{
			QString name = xml.name().toString();
			if (name == "score")
			{
				continue;
			}

			if (name == "best")
			{
				bestRecord = xml.readElementText().toInt();
				ui->bRec->setText(concTime(bestRecord));
			}
			if (name == "last")
			{
				lastRecord = xml.readElementText().toInt();
				ui->lRec->setText(concTime(lastRecord));
			}
		}
	}
	if (xml.hasError())
	{
		qDebug() << "error loading XML" << xml.errorString() << "line " << xml.lineNumber();
	}
}

void SimplePG::UpdateTime()
{
	//if (!counter->isActive()) return;
	++ctime;
	QStringList sList = concTime(ctime).split(':');
	ui->mLab->setText(sList[0]);
	ui->sLab->setText(sList[1]);
	ui->msLab->setText(sList[2]);
}

//hide and show button's contents
void SimplePG::startPeek()
{
	if (peek)
	{
		enableClick = true;
		secondRun = false;
		randList.clear();
		charList.clear();
		buttonOut.clear();

		for (QPushButton *qpb : buttonList)
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
		//second call with timer to hide contents
		peekTimer->start(2000);
		return;
	}
	//after 2 seconds this get executed
	for (QPushButton *qpb : buttonList)
	{
		qpb->setText("*");
	}
	enableClick = true;
	peek = true;
	counter->start(10);
	ui->mainBtn->setText("Cancel");
	ui->mainBtn->setDisabled(false);
	cancel = true;
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
		btn1 = qobject_cast<QPushButton *>(sender());
		btn1->setText(charList[buttonList.indexOf(btn1)]);
		//prevent clicking an already correct (combo) buttons
		if (buttonOut.contains(btn1))
			return;
		comboCheck.append(charList[buttonList.indexOf(btn1)]);
		//turning off only one (clicked) button
		twoDown = false;
		turnOffTimer = new QTimer(this);
		connect(turnOffTimer, SIGNAL(timeout()), this, SLOT(turnOff()));
		turnOffTimer->start(1000);
		turnOffTimer->setSingleShot(true);
		return;
	}
	//second click on any button just after clicking the first
	if (comboCheck.size() == 1)
	{
		btn2 = qobject_cast<QPushButton *>(sender());
		btn2->setText(charList[buttonList.indexOf(btn2)]);
		//prevent clicking an already correct (combo) buttons
		if (buttonOut.contains(btn2))
			return;
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
			comboCounter++;
			if (comboCounter == buttonList.size() / 2)
			{
				finish(true);
			}
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

//all button's contents are visible
void SimplePG::finish(bool done)
{
	counter->stop();
	ui->mLab->setText("00");
	ui->sLab->setText("00");
	ui->msLab->setText("00");
	enableClick = false;
	ui->mainBtn->setText("Start");
	for (QPushButton *qpb : buttonList)
	{
		qpb->setText("*");
	}
	cancel = false;
	if (done)
	{
		if (ctime < bestRecord || bestRecord == 0)
		{
			bestRecord = ctime;
		}
		lastRecord = ctime;
		dataSaver();
		ui->bRec->setText(concTime(bestRecord));
		ui->lRec->setText(concTime(lastRecord));
	}
	ctime = 0;
	comboCounter = 0;
}

void SimplePG::dataSaver()
{
	QXmlStreamWriter xml;
	QString filename = "score.xml";
	//save the score file in the same directory as the .exe
	auto file = new QFile(qApp->applicationDirPath() + "/" + filename);
	file->open(QFileDevice::ReadWrite);
	xml.setDevice(file);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeStartElement("score");
	xml.writeTextElement("best", QString::number(bestRecord));
	xml.writeTextElement("last", QString::number(lastRecord));
	xml.writeEndElement();
	xml.writeEndDocument();
	file->close();
}

QString SimplePG::concTime(unsigned xtime)
{
	if (xtime == 0)
		return "00:00:00";
	const int hr = xtime / 360000, min = xtime % 360000 / 6000, sec = xtime % 6000 / 100, milli = xtime % 100;
	QString sTime;
	QTextStream(&sTime) << min / 10 << min % 10 << ":" << sec / 10 << sec % 10 << ":" << milli / 10 << milli % 10;
	return sTime;
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

//for detecting the native events on qwidget
bool SimplePG::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	auto msg = static_cast<MSG *>(message);
	//for the native context menu
	if (msg->message == WM_SYSCOMMAND)
	{
		///only works with the first (commented) method
		//if ((msg->wParam & 0xfff0) == testSpace)
		///can't seems to make the above works with the struct method, always returns 0
		if ((msg->wParam) == miid)
		{
			clearScore();
			return true;
		}
	}

	return false;
}

//clearing the score from the ui and the file
void SimplePG::clearScore()
{
	//resetting the score
	bestRecord = 0;
	lastRecord = 0;
	//triggering update to local file
	dataSaver();
	ui->bRec->setText(concTime(bestRecord));
	ui->lRec->setText(concTime(lastRecord));
	qDebug("CLEARED");
}

///working wndProc, just not needed for this project :(
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//qDebug() << "WNDPROCC";
	PAINTSTRUCT ps;
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