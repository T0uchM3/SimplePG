#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SimplePG.h"
#include "QMouseEvent"
#include "NewEventFilter.h"
#include <windows.h>
#include "QDebug"
#include <qobject.h>
#include <QtGui>
#include <QTimer>

class SimplePG : public QMainWindow
{
	Q_OBJECT

public:
	~SimplePG();
	SimplePG(QWidget* parent = Q_NULLPTR);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	bool eventFilter(QObject* watched, QEvent* event);
	//randomizing the buttons and then hide
	QPoint current;
	//difference between pointer's location and drag's start location.
	QPoint correctionVal;
	bool pressed;
	QTimer* opTimer;
	QTimer* turnOffTimer;
	bool trigOnce = true;
	qreal opacityVal = 1.0;
	int opCounter = 0;
	QList<QPushButton*> buttonList;
	QList<QString> randStuff = { "$", "@", "#","%", "+","-" };
	//charList will get filled with random characters from randStuff
	QList<QString> charList;
	//comboCheck will check if the two selected characters matches or not
	QList<QString> comboCheck;
	//to avoid having a duplicate set, we put the generated number
	//in a randList for checking later
	QList<int> randList;
	//buttonOut got the buttons that got clicked out
	QList<QPushButton*> buttonOut;
	//random number [0..6[
	int rand;
	//this become true, when we finish filling the first half
	//so when the second(last) half finish and find it "true"
	//we bail
	bool secondRun;
	QTimer* peekTimer;
	bool enableClick = false;
	bool peek = true;
	QPushButton* btn1;
	QPushButton* btn2;
	bool twoDown = false;
private slots:
	void grid_btn_clicked();
	void on_closeButton_clicked();
	void on_mainBtn_clicked();
	void transTrig();
	void startPeek();
	void turnOff();
private:
	Ui::SimplePGClass* ui;
	int x = 0;
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);
};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
