#include "TopWindow.h"
#include "GraphicsPanel.h"

TopWindow::TopWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	GraphicsPanel* gPanel = new GraphicsPanel(this);
	ui.graphicsLayout->addWidget(gPanel);
}

void TopWindow::updateFrameCt(int value)
{
	ui.label_FrameCt->setText(QString::number(value));
}