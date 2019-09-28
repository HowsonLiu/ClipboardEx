/********************************************************************************
** Form generated from reading UI file 'ReminderWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REMINDERWINDOW_H
#define UI_REMINDERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReminderWindowClass
{
public:

    void setupUi(QWidget *ReminderWindowClass)
    {
        if (ReminderWindowClass->objectName().isEmpty())
            ReminderWindowClass->setObjectName(QString::fromUtf8("ReminderWindowClass"));
        ReminderWindowClass->resize(600, 400);

        retranslateUi(ReminderWindowClass);

        QMetaObject::connectSlotsByName(ReminderWindowClass);
    } // setupUi

    void retranslateUi(QWidget *ReminderWindowClass)
    {
        ReminderWindowClass->setWindowTitle(QCoreApplication::translate("ReminderWindowClass", "ReminderWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ReminderWindowClass: public Ui_ReminderWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMINDERWINDOW_H
