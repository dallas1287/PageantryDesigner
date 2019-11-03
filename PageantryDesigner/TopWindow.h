#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_TopWindow.h"
#include "GraphicsPanel.h"

class TopWindow : public QMainWindow
{
	Q_OBJECT

public:
	TopWindow(QWidget *parent = Q_NULLPTR);
	void updateFrameCt(int value);
	void populateAnimCb(std::vector<QString>& name);

public slots:
	void armSliderChanged(int value);
	void armLineEditChanged();
	void onPlayClicked();

private:
	Ui::TopWindowClass ui;
	GraphicsPanel* m_gPanel = nullptr;
};
