/********************************************************************************
** Form generated from reading UI file 'TopWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOPWINDOW_H
#define UI_TOPWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TopWindowClass
{
public:
    QWidget *centralWidget;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *graphicsLayout;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TopWindowClass)
    {
        if (TopWindowClass->objectName().isEmpty())
            TopWindowClass->setObjectName(QStringLiteral("TopWindowClass"));
        TopWindowClass->resize(1116, 824);
        centralWidget = new QWidget(TopWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayoutWidget = new QWidget(centralWidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(30, 30, 1041, 591));
        graphicsLayout = new QHBoxLayout(horizontalLayoutWidget);
        graphicsLayout->setSpacing(6);
        graphicsLayout->setContentsMargins(11, 11, 11, 11);
        graphicsLayout->setObjectName(QStringLiteral("graphicsLayout"));
        graphicsLayout->setContentsMargins(0, 0, 0, 0);
        TopWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(TopWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1116, 26));
        TopWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(TopWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        TopWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(TopWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        TopWindowClass->setStatusBar(statusBar);

        retranslateUi(TopWindowClass);

        QMetaObject::connectSlotsByName(TopWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *TopWindowClass)
    {
        TopWindowClass->setWindowTitle(QApplication::translate("TopWindowClass", "TopWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TopWindowClass: public Ui_TopWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOPWINDOW_H
