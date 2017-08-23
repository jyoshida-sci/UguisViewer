#ifndef QCLICLABLELABEL_H
#define QCLICLABLELABEL_H

#include "qlabel.h"
#include <QMouseEvent>

class QClickableLabel : public QLabel
{
		Q_OBJECT
public:
	explicit QClickableLabel(QWidget *parent = 0);


protected:
	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);

signals:
	void mousePressed(QMouseEvent * e);
	void mouseMoved(QMouseEvent * e);
};
#endif // QCLICLABLELABEL_H
