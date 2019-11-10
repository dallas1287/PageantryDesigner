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
	void populateMeshesCb(std::vector<QString>& names);
	void updateCameraStats();

public slots:
	void armSliderChanged(int value);
	void armLineEditChanged();
	void onPlayClicked();
	void onMbActionClicked(QAction* action);
	void onLoadTextureClicked();
	void onMeshesCbChanged(const QString& name);


private:
	Ui::TopWindowClass ui;
	std::unique_ptr<GraphicsPanel> m_gPanel;
};
