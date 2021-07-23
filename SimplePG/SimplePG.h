#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SimplePG.h"
#include "QMouseEvent"
#include "NewEventFilter.h"
#include <windows.h>
#include "QDebug"

class NewEventFilter;
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
	bool pressed;
	NewEventFilter* NEF;
private slots:
	void grid_btn_clicked();
	//void on_pushButton_clicked(QMouseEvent);
	void on_closeButton_clicked();
	void on_mainBtn_clicked();

private:
	Ui::SimplePGClass* ui;
	int x = 0;
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);
	//WNDCLASSEXW wcex;
};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
