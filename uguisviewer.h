#ifndef UGUISVIEWER_H
#define UGUISVIEWER_H

#include <QWidget>

class UguisViewer : public QWidget
{
    Q_OBJECT

public:
    UguisViewer(QWidget *parent = 0);
    ~UguisViewer();
};

#endif // UGUISVIEWER_H
