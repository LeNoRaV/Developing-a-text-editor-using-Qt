#ifndef EXIT_H
#define EXIT_H

#include "mainwindow.h"

class ExitWindow:
        public QDialog
{
    Q_OBJECT

public:
    ExitWindow(QStringList* nameFiles, QWidget* parent=nullptr);

private slots:
    void slotPushButton();
};

#endif // EXIT_H
