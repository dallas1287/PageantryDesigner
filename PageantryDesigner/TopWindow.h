#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TopWindow.h"

class TopWindow : public QMainWindow
{
	Q_OBJECT

public:
	TopWindow(QWidget *parent = Q_NULLPTR);
	void updateFrameCt(int value);

private:
	Ui::TopWindowClass ui;
};
