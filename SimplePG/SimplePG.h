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
	bool trigOnce = true;
	qreal opacityVal = 1.0;
	int opCounter = 0;
	QList<QPushButton*> buttonList;
	QList<QString> randStuff = { "$", "@", "#","%", "+","-" };
	QList<QString> charList;
	//to avoid having a duplicate set, we put the generated number
	//in a randList for checking later
	QList<int> randList;
	//random number [0..6[
	int rand;
	//this become true, when we finish filling the first half
	//so when the second(last) half finish and find it "true"
	//we bail
	bool secondRun;
	QTimer* peekTimer;
	bool peek = true;
private slots:
	void grid_btn_clicked();
	void on_closeButton_clicked();
	void on_mainBtn_clicked();
	void transTrig();
	void startPeek();

private:
	Ui::SimplePGClass* ui;
	int x = 0;
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);
};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
