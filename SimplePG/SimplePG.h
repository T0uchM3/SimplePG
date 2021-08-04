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
	QPoint current;
	//difference between pointer's location and drag's start location.
	QPoint correctionVal;
	bool pressed;
	QTimer* timer;
	bool trigOnce = true;
	qreal opacityVal = 1.0;
	int opCounter = 0;
private slots:
	void grid_btn_clicked();
	void on_closeButton_clicked();
	void on_mainBtn_clicked();
	void transTrig();
private:
	Ui::SimplePGClass* ui;
	int x = 0;
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);
};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
