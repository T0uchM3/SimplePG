#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SimplePG.h"
#include "QMouseEvent"

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

private slots:
	void grid_btn_clicked();
	//void on_pushButton_clicked(QMouseEvent);
	void on_closeButton_clicked();

private:
	Ui::SimplePGClass* ui;
};
