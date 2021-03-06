#pragma once

#include <QtWidgets/QWidget>
//#include "ui_UguisViewer.h"

#include "opencv2/opencv.hpp"


class QSettings;
class QLabel;
class QPixmap;
class QLineEdit;
class QPushButton;
class QLCDNumber;
class QCheckBox;
class QTextEdit;
class QClickableLabel;
class QSlider;
class QVector3D;

class UguisViewer : public QWidget
{
    Q_OBJECT

public:
    UguisViewer(QWidget *parent = Q_NULLPTR);
	~UguisViewer();

signals:
	void wheelEvent(QWheelEvent *);
	//void mousePressEvent(QMouseEvent *);


	private slots:
	bool updateImg(int i);
	bool imgUp();
	bool imgDown();
	bool loadImg();
	bool changeLayer(QWheelEvent *);
	void changeToNthLayer(int);
	void labMouseClicked(QMouseEvent *);
	void labMouseMoved(QMouseEvent *);
	void updateSubDisplay(QMouseEvent *);
	void writeTxtFile();
	void readPrevFile();
	void readNextFile();


private:
	QSettings* appsettings;
	QString dirName;
	QString fileName;

	std::vector<cv::Mat> vomat;//original
	std::vector<cv::Mat> vfmat;//filterd

	QPixmap pix1;
	QPixmap pix2;
	QPixmap pix3;

	QClickableLabel* lab_img;
	QLabel* lab_img_o;
	QLabel* lab_img_f;

	QLabel* lab_pix;
	QLabel* lab_pix_cl;
	QLabel* lab_pix_dr;
	QLabel* lab_pix_unit;

	QLabel* lab_stg;
	QLabel* lab_stg_cl;
	QLabel* lab_stg_dr;
	QLabel* lab_stg_unit;

	QLineEdit* line_file;
	QPushButton* but_file;
	QLCDNumber* lcd_img;
	QCheckBox*  chk_enlarge;
	QCheckBox*  chk_filterd;

	QPushButton* but_up;
	QPushButton* but_down;
	QPushButton* but_make;
	QPushButton* but_writxt;

	QTextEdit* txt_clicked;
	QSlider* sli_z;

	QPushButton* but_prevfile;
	QPushButton* but_nextfile;

	int ipict;

    QString document_unit;
    std::vector<QVector3D> vsxyz;
    int wi;
	int he;
    double start_x, start_y, start_z;
    double end_x, end_y, end_z;
    double mm_px, mm_py;
	double Sh;
    int clkx, clky, clkz;
    double clksz;

	void Init();

	bool ImportFile(QString FileName);
	bool readIthFileInCurrentDir(int i);
    void getTheDarkestZ(int x, int y, int z,  int range);

};


