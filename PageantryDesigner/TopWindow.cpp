#include "TopWindow.h"

TopWindow::TopWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_gPanel = new GraphicsPanel(this);
	ui.graphicsLayout->addWidget(m_gPanel);
	connect(ui.cbAnimations, qOverload<int>(&QComboBox::currentIndexChanged), m_gPanel, &GraphicsPanel::onAnimCbChanged);
	connect(ui.armSlider, &QSlider::valueChanged, this, &TopWindow::armSliderChanged);
	connect(ui.armLineEdit, &QLineEdit::returnPressed, this, &TopWindow::armLineEditChanged);
	connect(ui.btnPlay, &QPushButton::clicked, this, &TopWindow::onPlayClicked);
}

void TopWindow::updateFrameCt(int value)
{
	ui.label_FrameCt->setText(QString::number(value));
}

void TopWindow::populateAnimCb(std::vector<QString>& names)
{
	for (auto name : names)
		ui.cbAnimations->addItem(name);
}

void TopWindow::armSliderChanged(int value)
{
	QString str = QString::number(value);
	m_gPanel->setAnimationFrame(value);
	ui.armLineEdit->setText(str);
}

void TopWindow::armLineEditChanged()
{
	QString text = ui.armLineEdit->displayText();
	bool isOk;
	int val = text.toInt(&isOk);
	if (isOk && val <= 100 && val >= 0)
		ui.armSlider->setSliderPosition(val);
}

void TopWindow::onPlayClicked()
{
	m_gPanel->onPlayClicked();
	if (m_gPanel->isPaused())
		ui.btnPlay->setText("Play");
	else
		ui.btnPlay->setText("Stop");
}