#include "TopWindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include "utils.h"

TopWindow::TopWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_gPanel.reset(new GraphicsPanel(this));
	ui.graphicsTab->layout()->addWidget(m_gPanel.get());
	connect(ui.cbAnimations, qOverload<int>(&QComboBox::currentIndexChanged), m_gPanel.get(), &GraphicsPanel::onAnimCbChanged);
	connect(ui.armSlider, &QSlider::valueChanged, this, &TopWindow::armSliderChanged);
	connect(ui.armLineEdit, &QLineEdit::returnPressed, this, &TopWindow::armLineEditChanged);
	connect(ui.btnPlay, &QPushButton::clicked, this, &TopWindow::onPlayClicked);
	connect(ui.menuBar, &QMenuBar::triggered, this, &TopWindow::onMbActionClicked);
	connect(ui.cbMeshes, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &TopWindow::onMeshesCbChanged);
	connect(ui.btnLoadTexture, &QPushButton::clicked, this, &TopWindow::onLoadTextureClicked);

	m_gPanel->setFocus(Qt::FocusReason::OtherFocusReason);
}

void TopWindow::updateFrameCt(int value)
{
	ui.label_FrameCt->setText(QString::number(value));
}

void TopWindow::updateCameraStats()
{
	ui.label_FrameCt->setText(m_gPanel->getBlinn() ? "On" : "Off"); //this is doing double duty for lighting tests
	ui.labelCamPos->setText("Position: " + vectorToString(m_gPanel->getCamera().Position(), 2));
	ui.labelCamLookAt->setText("LookAt: " + vectorToString(m_gPanel->getCamera().Target(), 2));
	ui.labelCamFront->setText("Front: " + vectorToString(m_gPanel->getCamera().Front(), 2));
	ui.labelCamUp->setText("Up: " + vectorToString(m_gPanel->getCamera().Up(), 2));
	ui.labelCamRight->setText("Right: " + vectorToString(m_gPanel->getCamera().getRightVector()));
	ui.labelYPW->setText("Yaw: " + QString::number(m_gPanel->getCamera().Yaw(), 'f', 1) + 
		" Pitch: " + QString::number(m_gPanel->getCamera().Pitch(), 'f', 1) + 
		" Roll: " + QString::number(m_gPanel->getCamera().Roll(), 'f', 1));
}

void TopWindow::populateAnimCb(std::vector<QString>& names)
{
	ui.cbAnimations->clear();
	for (auto name : names)
		ui.cbAnimations->addItem(name);
}

void TopWindow::armSliderChanged(int value)
{
	QString str = QString::number(value);
	m_gPanel->setAnimationFrame(value);
	ui.armLineEdit->setText(str);
}

void TopWindow::onMbActionClicked(QAction* action)
{
	QFileDialog fdialog;
	fdialog.setFileMode(QFileDialog::ExistingFile);

	QString fileName = fdialog.getOpenFileName(this, "Choose File to Import", QStandardPaths::writableLocation(QStandardPaths::StandardLocation::HomeLocation));
	m_gPanel->importModel(fileName);
	m_gPanel->populateAnimCb();
	m_gPanel->populateMeshesCb();
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

void TopWindow::populateMeshesCb(std::vector<QString>& names)
{
	ui.cbMeshes->clear();
	for (auto name : names)
		ui.cbMeshes->addItem(name);
}

void TopWindow::onMeshesCbChanged(const QString& name)
{
	qDebug() << name;
}

void TopWindow::onLoadTextureClicked()
{
	QFileDialog fdialog;
	fdialog.setFileMode(QFileDialog::ExistingFile);

	QString fileName = fdialog.getOpenFileName(this, "Choose Texture to Import", QStandardPaths::writableLocation(QStandardPaths::StandardLocation::HomeLocation));
	
	m_gPanel->getMeshRenderer()->loadTexture(fileName, ui.cbMeshes->currentText());

	ui.leTexturePath->setText(fileName);
}