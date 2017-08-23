#include "QClickableLabel.h"
#include <QMouseEvent>

QClickableLabel::QClickableLabel(QWidget *parent)
	: QLabel(parent)
{
}

void QClickableLabel::mousePressEvent(QMouseEvent * e)
{
  QLabel::mousePressEvent(e);
  emit mousePressed(e);
}

void QClickableLabel::mouseMoveEvent(QMouseEvent * e)
{
	QLabel::mouseMoveEvent(e);
  emit mouseMoved(e);
}