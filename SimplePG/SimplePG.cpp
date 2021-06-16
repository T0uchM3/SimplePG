#include "SimplePG.h"
#include "QDebug";

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

SimplePG::~SimplePG()
{
	delete ui;
}

void SimplePG::on_closeButton_clicked()
{
	qDebug("close");
	SimplePG::close();
}