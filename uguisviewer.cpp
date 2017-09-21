#include "UguisViewer.h"

#include "opencv2/opencv.hpp"

#include "qsettings.h"

#include "qlineedit.h"
#include "qpushbutton.h"
#include "qlayout.h"
#include "qlabel.h"
#include "qstring.h"
#include "qlcdnumber.h"
#include "qcheckbox.h"
#include "qtextedit.h"
#include "qtextstream.h"
#include "qslider.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "qdebug.h"

#include "qfiledialog.h"
#include "qdir.h"
#include "qmenu.h"
#include <QMouseEvent>

#include "qvector3d.h"
#include "QClickableLabel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>


UguisViewer::UguisViewer(QWidget *parent)
    : QWidget(parent)
{
    appsettings = new QSettings("UguisViewer.ini", QSettings::IniFormat);

    QGridLayout* lay = new QGridLayout();

    //image
    but_up = new QPushButton(tr("&Prev"));
    lay->addWidget(but_up, 0, 0, 1, 2);
    lab_img_o = new QLabel();
    lay->addWidget(lab_img_o, 1, 0);
    lab_img_f = new QLabel();
    lay->addWidget(lab_img_f, 2, 0);
    lab_img = new QClickableLabel();
    lab_img->setMouseTracking(false);
    lay->addWidget(lab_img, 1, 1, 2, 1);
    but_down = new QPushButton(tr("&Next"));
    lay->addWidget(but_down, 3, 0, 1, 2);


    //click info
    QHBoxLayout *lay_click = new QHBoxLayout();
    lab_pix = new QLabel();
    lab_pix->setFont(QFont("Arial", 10));
    lay_click->addWidget(lab_pix);
    lab_pix_cl = new QLabel();
    lab_pix_cl->setFont(QFont("Arial", 10));
    lay_click->addWidget(lab_pix_cl);
    lab_pix_dr = new QLabel();
    lab_pix_dr->setFont(QFont("Arial", 10));
    lay_click->addWidget(lab_pix_dr);
    lab_pix_unit = new QLabel("[pix]");
    lab_pix_unit->setFont(QFont("Arial", 10));
    lay_click->addWidget(lab_pix_unit);
    lay->addLayout(lay_click, 4, 0, 1, 2);

    QHBoxLayout *lay_stage = new QHBoxLayout();
    lab_stg = new QLabel();
    lab_stg->setFont(QFont("Arial", 10));
    lay_stage->addWidget(lab_stg);
    lab_stg_cl = new QLabel();
    lab_stg_cl->setFont(QFont("Arial", 10));
    lay_stage->addWidget(lab_stg_cl);
    lab_stg_dr = new QLabel();
    lab_stg_dr->setFont(QFont("Arial", 10));
    lay_stage->addWidget(lab_stg_dr);
    lab_stg_unit = new QLabel("[micron]");
    lab_stg_unit->setFont(QFont("Arial", 10));
    lay_stage->addWidget(lab_stg_unit);
    lay->addLayout(lay_stage, 5, 0, 1, 2);



    //file path
    QHBoxLayout *lay_file = new QHBoxLayout();
    line_file = new QLineEdit(QString("no file yet"));
    but_file = new QPushButton(tr("&Read file"));
    lcd_img = new QLCDNumber();
    chk_filterd = new QCheckBox("filterd");
    //chk_filterd->setCheckState(Qt::Checked);
    lay_file->addWidget(line_file);
    lay_file->addWidget(but_file);
    lay_file->addWidget(lcd_img);
    lay_file->addWidget(chk_filterd);
    lay->addLayout(lay_file, 6, 0, 1, 4);

    sli_z = new QSlider();
    lay->addWidget(sli_z, 0, 2, 6, 1);

    //texteditarea
    txt_clicked = new QTextEdit();
    txt_clicked->setFixedWidth(230);
    txt_clicked->setText("#px py lz stagex y z\n");
    lay->addWidget(txt_clicked, 0, 3, 5, 1);
    but_writxt = new QPushButton(tr("&Save File"));
    lay->addWidget(but_writxt, 5, 3, 1, 1);
    //but_make = new QPushButton(tr("&Make pict"));
    //lay->addWidget(but_make, 4, 2, 1, 1);


    //prev,nextfile
    QHBoxLayout *lay_prevnextfile = new QHBoxLayout();
    but_prevfile = new QPushButton(tr("Prev File"));
    lay_prevnextfile->addWidget(but_prevfile);
    but_nextfile = new QPushButton(tr("Next File"));
    lay_prevnextfile->addWidget(but_nextfile);
    lay->addLayout(lay_prevnextfile, 7, 0, 1, 4);


    //layout
    setLayout(lay);


    //connect signal-slot
    connect(but_file, SIGNAL(clicked()), this, SLOT(loadImg()));
    connect(but_up, SIGNAL(clicked()), this, SLOT(imgUp()));
    connect(but_down, SIGNAL(clicked()), this, SLOT(imgDown()));

    connect(but_writxt, SIGNAL(clicked()), this, SLOT(writeTxtFile()));
    connect(this, SIGNAL(wheelEvent(QWheelEvent*)), this, SLOT(changeLayer(QWheelEvent*)));
    connect(lab_img, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(labMouseClicked(QMouseEvent*)));
    connect(lab_img, SIGNAL(mouseMoved(QMouseEvent*)), this, SLOT(labMouseMoved(QMouseEvent*)));
    connect(sli_z, SIGNAL(valueChanged(int)), this, SLOT(changeToNthLayer(int)));

    connect(but_prevfile, SIGNAL(clicked()), this, SLOT(readPrevFile()));
    connect(but_nextfile, SIGNAL(clicked()), this, SLOT(readNextFile()));

    but_up->setDisabled(true);
    but_down->setDisabled(true);
    but_prevfile->setDisabled(true);
    but_nextfile->setDisabled(true);
    sli_z->setDisabled(true);

}

UguisViewer::~UguisViewer()
{
    Init();
}


void UguisViewer::Init()
{
    vomat.clear();
    vfmat.clear();
    vsxyz.clear();
}


bool UguisViewer::updateImg(int i)
{

    cv::Mat mat = vomat[i];

    if (chk_filterd->isChecked()){
        mat = vfmat[i];
    }

    QImage image(mat.data,
                 mat.cols,
                 mat.rows,
                 QImage::Format_RGB888);
    //QImage::Format_Indexed8);
    image = image.convertToFormat(QImage::Format_RGB32);
    lab_img->setPixmap(QPixmap::fromImage(image));

    return true;
}


bool UguisViewer::changeLayer(QWheelEvent *event)
{
    if (vomat.size() == 0) return false;

    if (event->delta()<0)
    {
        ipict++;
        if (ipict == vomat.size())ipict = vomat.size() - 1;
        if (ipict != 0) but_up->setEnabled(true);
        if (ipict == vomat.size() - 1) but_down->setDisabled(true);
        lcd_img->display(ipict);
        if (!updateImg(ipict)) return false;
    }
    else
    {
        ipict--;
        if (ipict == -1)ipict = 0;
        if (ipict == 0) but_up->setDisabled(true);
        if (ipict != vomat.size() - 1) but_down->setEnabled(true);
        lcd_img->display(ipict);
        if (!updateImg(ipict)) return false;
    }
    sli_z->setValue(ipict);
    return true;
}


bool UguisViewer::imgUp()
{
    ipict--;
    if (ipict == -1)ipict = 0;
    if (ipict == 0) but_up->setDisabled(true);
    if (ipict != vomat.size() - 1) but_down->setEnabled(true);
    lcd_img->display(ipict);
    if (!updateImg(ipict)) return false;
    sli_z->setValue(ipict);
    return true;
}

bool UguisViewer::imgDown()
{
    ipict++;
    if (ipict == vomat.size())ipict = vomat.size() - 1;
    if (ipict != 0) but_up->setEnabled(true);
    if (ipict == vomat.size() - 1) but_down->setDisabled(true);
    lcd_img->display(ipict);
    if (!updateImg(ipict)) return false;
    sli_z->setValue(ipict);
    return true;
}


void UguisViewer::changeToNthLayer(int i){
    ipict = i;
    if (ipict == 0){but_up->setDisabled(true);}
    else{but_up->setEnabled(true);}
    if (ipict == vomat.size() - 1){  but_down->setDisabled(true);}
    else{but_down->setEnabled(true);}
    lcd_img->display(ipict);
    updateImg(ipict);
}


bool UguisViewer::ImportFile(QString myFileName)
{
    //file open
    QFile file((const char *)myFileName.toLatin1().data());
    if (!file.open(QIODevice::ReadOnly)) return false;

    Init();
    fileName = myFileName;

    QString suffix = QFileInfo(fileName).suffix();

    if (suffix == "json" || suffix == "JSON")
    {
        //json file
        QString content = file.readAll();
        file.close();

        QJsonDocument jsondoc = QJsonDocument::fromJson(content.toUtf8());
        QJsonObject jsonobj = jsondoc.object();
        QJsonArray array_images = jsonobj["Images"].toArray();

        document_unit = jsonobj["DocumentType"].toObject()["unit"].toString();
        lab_stg_unit->setText("[" + document_unit + "]");

        wi = jsonobj["ImageType"].toObject()["Width"].toInt();
        he = jsonobj["ImageType"].toObject()["Height"].toInt();
        Sh = jsonobj["EmulsionType"].toObject()["Sh"].toDouble();
        mm_px = jsonobj["Interval"].toArray().at(0).toDouble();
        mm_py = jsonobj["Interval"].toArray().at(1).toDouble();

        //read img files
        for (int p = 0; p<array_images.count(); p++){
            std::string dirpath = QDir(dirName).absolutePath().toLocal8Bit().constData();
            std::string filename = array_images.at(p).toObject()["Path"].toString().toLocal8Bit().constData();
            std::string filefullpath = dirpath + "/" + filename;

            cv::Mat mat1 = cv::imread(filefullpath, 0);//gray scale
            cv::cvtColor(mat1, mat1, CV_GRAY2BGR);
            vomat.push_back(mat1);

            QVector3D v3 = QVector3D(
                        array_images.at(p).toObject()["x"].toDouble(),
                        array_images.at(p).toObject()["y"].toDouble(),
                        array_images.at(p).toObject()["z"].toDouble());
            vsxyz.emplace_back(v3);
        }

    }else{
        return false;
    }


    for (int i = 0; i < vomat.size(); i++){
        cv::Mat mat2 = vomat[i].clone();
        cv::GaussianBlur(mat2, mat2, cv::Size(3, 3), -1);
        cv::Mat smooth_img = mat2.clone();
        cv::GaussianBlur(smooth_img, smooth_img, cv::Size(51, 51), -1);
        cv::subtract(smooth_img, mat2, mat2);

        //double min,max;
        //cv::minMaxLoc( mat2,&min,&max,0,0);
        //cv::convertScaleAbs( mat2,  mat2, 255/(max-min), -255*min/(max-min));

        cv::threshold(mat2, mat2, 25, 255, CV_THRESH_TOZERO);

        vfmat.push_back(mat2);
    }


    //•À—ñˆ—‚Å‚Íˆ—‚Ì‡”Ô‚Í‚»‚Ì“s“x•Ï‚í‚éB‡”Ô‚ª•Ï‚í‚Á‚Ä‚à–â‘è‚È‚¢ˆ—‚ð‚±‚±‚Å‚â‚Á‚Ä‚¢‚éB
#ifdef _OPENMP// without_openmp: 25sec / openmp:7.5sec
#pragma omp parallel for
#endif
    for (int p = 0; p<vomat.size(); p++)
    {
        double min, max;
        cv::minMaxLoc(vfmat[p], &min, &max, 0, 0);
        cv::convertScaleAbs(vfmat[p], vfmat[p], 255 / (max - min), -255 * min / (max - min));
    }

    lab_img->setMouseTracking(true);

    ipict = 0;
    if (!updateImg(ipict)) return false;
    but_down->setEnabled(true);
    sli_z->setEnabled(true);
    sli_z->setMaximum(vfmat.size() - 1);
    but_prevfile->setEnabled(true);
    but_nextfile->setEnabled(true);

    line_file->setText(QString("%1").arg(fileName));

    //sub-display
    QImage image_o(9 * 24,
                   9 * 24,
                   QImage::Format_RGB888);
    image_o = image_o.convertToFormat(QImage::Format_RGB32);
    lab_img_o->setPixmap(QPixmap::fromImage(image_o));


    //sub-display
    QImage image_f(9 * 24,
                   9 * 24,
                   QImage::Format_RGB888);
    image_f = image_f.convertToFormat(QImage::Format_RGB32);
    lab_img_f->setPixmap(QPixmap::fromImage(image_f));

    return true;
}



bool UguisViewer::loadImg()
{
    dirName = appsettings->value("readdir").toString();
    qDebug() << "appsettings->value(readdir).toString() is ";
    qDebug() << QDir(dirName).absolutePath();

    QDir dir = QDir(dirName);

    //dialog
    fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open files"),
                dir.absolutePath(),
                tr("JSON-Files (*.json);;All Files (*)"));
    if (fileName.isEmpty())return false;

    //dir path
    dir = QFileInfo(fileName).absoluteDir();
    appsettings->setValue("readdir", dir.absolutePath());
    dirName = appsettings->value("readdir").toString();

    bool status = ImportFile(fileName);
    return status;
}


void UguisViewer::labMouseClicked(QMouseEvent* e)
{
    clkx = e->x();
    clky = e->y();
    clkz = ipict;
    clksz =  vsxyz[ipict].z();
    lab_pix_cl->setText(QString("click: %1, %2, %3").arg(clkx, 4, 10).arg(clky, 4, 10).arg(clkz, 4, 10));

    double stage[3];
    stage[0] = vsxyz[ipict].x() - (clkx - wi / 2)*mm_px;
    stage[1] = vsxyz[ipict].y() + (clky - he / 2)*mm_py;
    stage[2] = vsxyz[ipict].z();
    lab_stg_cl->setText(QString("%1, %2, %3").arg(stage[0], 7, 'f', 4).arg(stage[1], 7, 'f', 4).arg(stage[2], 7, 'f', 4));

    if (e->buttons() & Qt::LeftButton){
        QString str = txt_clicked->toPlainText();
        str += QString("%1 %2 %3 ").arg(clkx).arg(clky).arg(clkz);
        str += QString("%1 %2 %3\n").arg(stage[0], 7, 'f', 4).arg(stage[1], 7, 'f', 4).arg(stage[2], 7, 'f', 4);
        txt_clicked->setText(str);
    }
    else if (e->buttons() & Qt::RightButton)
    {
        //Right-click context menus with Qt
        //http://www.setnode.com/blog/right-click-context-menus-with-qt/
        QPoint globalPos = this->mapToGlobal(e->pos());

        QMenu myMenu;
        myMenu.addAction("Set the start point");
        myMenu.addAction("Set the end point");

        QAction* selectedItem = myMenu.exec(globalPos);
        if (selectedItem){
            if (selectedItem->text() == "Set the start point")
            {
                start_x = e->x();
                start_y = e->y();
                start_z = vsxyz[ipict].z();

                QString str = txt_clicked->toPlainText();
                str += QString("start pos: %1 %2 %3\n").arg(start_x).arg(start_y).arg(start_z);
                txt_clicked->setText(str);
            }
            else if (selectedItem->text() == "Set the end point")
            {
                end_x = e->x();
                end_y = e->y();
                end_z = vsxyz[ipict].z();
                QString str = txt_clicked->toPlainText();
                str += QString("end pos: %1 %2 %3\n").arg(end_x).arg(end_y).arg(end_z);

                double dx = -(end_x - start_x)*mm_px;
                double dy = (end_y - start_y)*mm_py;
                double dz = end_z - start_z;
                qDebug() << QString("%1 %2 %3\n").arg(dx, 7, 'f', 4).arg(dy, 7, 'f', 4).arg(dz, 7, 'f', 4);

                double range = sqrt(dx*dx + dy*dy + dz*dz);
                double rangesh = sqrt(dx*dx + dy*dy + dz*dz*Sh*Sh);
                str += QString("Range = %1 (%2)[mm]\n").arg(range,7, 'f', 4).arg(rangesh,7, 'f', 4);

                double phi = atan2(dy, dx) * 180 / M_PI;
                if (dy < 0) phi += 360.0;
                str += QString("azimuth_phi= %1[deg]\n").arg(phi,4, 'f', 1);

                double cos_theta = dz / range;
                if (cos_theta>1.0){cos_theta = 1.0;}
                if (cos_theta<-1.0){cos_theta = -1.0;}
                double theta = acos(cos_theta) * 180 / M_PI;
                double cos_theta_sh = dz*Sh / rangesh;
                if (cos_theta_sh>1.0){cos_theta_sh = 1.0;}
                if (cos_theta_sh<-1.0){cos_theta_sh = -1.0;}
                double theta_sh = acos(cos_theta_sh) * 180 / M_PI;
                str += QString("zenith_theta= %1 (%2) [deg]\n").arg(theta,4, 'f', 1).arg(theta_sh,4, 'f', 1);

                txt_clicked->setText(str);
            }

        }//if (selectedItem)
    }

}


void UguisViewer::labMouseMoved(QMouseEvent* e){

    int cx = e->x();
    int cy = e->y();
    int cz = ipict;
    //qDebug() << ipict;
    //qDebug() << QString("current pos: %1, %2, %3").arg(cx, 4, 10).arg(cy, 4, 10).arg(cz, 4, 10);

    lab_pix->setText(QString("current pos: %1, %2, %3").arg(cx, 4, 10).arg(cy, 4, 10).arg(cz, 4, 10));

    double stage[3];
    stage[0] = vsxyz[ipict].x() - (clkx - wi / 2)*mm_px;
    stage[1] = vsxyz[ipict].y() + (clky - he / 2)*mm_py;
    stage[2] = vsxyz[ipict].z();
    lab_stg->setText(QString("%1, %2, %3").arg(stage[0], 7, 'f', 4).arg(stage[1], 7, 'f', 4).arg(stage[2], 7, 'f', 4));

    if (e->buttons() & Qt::LeftButton){
        int dx = cx - clkx;
        int dy = cy - clky;
        int dz = cz - clkz;
        double dsz = stage[2] - clksz;
        lab_pix_dr->setText(QString("drag: %1, %2, %3").arg(dx, 4, 10).arg(dy, 4, 10).arg(dz, 4, 10));
        lab_stg_dr->setText(QString("%1, %2, %3 (%4)").arg(dx*mm_px, 7, 'f', 4).arg(dy*mm_py, 7, 'f', 4).arg(dsz, 7, 'f', 4).arg(dsz*Sh, 7, 'f', 4));
   }

    updateSubDisplay(e);
}


void UguisViewer::updateSubDisplay(QMouseEvent* e){
    if (vomat.size() == 0) return;

    int cx = e->x();
    int cy = e->y();

    //update sub-display for original img
    if ((cx - 4) >= 0 && (cy - 4) >= 0 && (cx + 4) <= wi && (cy + 4) <= he) {
        const int cellsize = 24;//should be 4*n;
        cv::Mat mat_o = cv::Mat(9 * cellsize, 9 * cellsize, CV_8UC3);

        for (int iy = 0; iy<9; iy++){
            for (int ix = 0; ix<9; ix++){
                int b = vomat[ipict].data[(wi*(cy - 4 + iy) + (cx - 4 + ix)) * 3];
                cv::rectangle(mat_o,
                              cv::Point(ix*cellsize, iy*cellsize),
                              cv::Point((ix + 1)*cellsize, (iy + 1)*cellsize),
                              cv::Scalar(b, b, b),
                              -1,
                              CV_AA);
                char cb[16];
                sprintf(cb, "%d", b);
                cv::putText(mat_o,
                            cb,
                            cv::Point(ix*cellsize, iy*cellsize + cellsize / 2),
                            cv::FONT_HERSHEY_SIMPLEX,
                            0.3,
                            cv::Scalar(255, 0, 0),
                            1,
                            4);
            }
        }
        cv::rectangle(mat_o, cv::Point(4 * cellsize, 4 * cellsize), cv::Point(5 * cellsize, 5 * cellsize), cv::Scalar(0, 255, 0), 1, CV_AA);

        QImage image_o(mat_o.data,
                       mat_o.cols,
                       mat_o.rows,
                       QImage::Format_RGB888);
        image_o = image_o.convertToFormat(QImage::Format_RGB32);
        lab_img_o->setPixmap(QPixmap::fromImage(image_o));




        //update sub-display for filered img
        cv::Mat mat_f = cv::Mat(9 * cellsize, 9 * cellsize, CV_8UC3);

        for (int iy = 0; iy<9; iy++){
            for (int ix = 0; ix<9; ix++){
                int b = vfmat[ipict].data[(wi*(cy - 4 + iy) + (cx - 4 + ix)) * 3];
                cv::rectangle(mat_f,
                              cv::Point(ix*cellsize, iy*cellsize),
                              cv::Point((ix + 1)*cellsize, (iy + 1)*cellsize),
                              cv::Scalar(b, b, b),
                              -1,
                              CV_AA);
                char cb[16];
                sprintf(cb, "%d", b);
                cv::putText(mat_f,
                            cb,
                            cv::Point(ix*cellsize, iy*cellsize + cellsize / 2),
                            cv::FONT_HERSHEY_SIMPLEX,
                            0.3,
                            cv::Scalar(255, 0, 0),
                            1,
                            4);
            }
        }
        cv::rectangle(mat_f, cv::Point(4 * cellsize, 4 * cellsize), cv::Point(5 * cellsize, 5 * cellsize), cv::Scalar(0, 255, 0), 1, CV_AA);

        QImage image_f(mat_f.data,
                       mat_f.cols,
                       mat_f.rows,
                       QImage::Format_RGB888);
        image_f = image_f.convertToFormat(QImage::Format_RGB32);
        lab_img_f->setPixmap(QPixmap::fromImage(image_f));
    }
}



void UguisViewer::writeTxtFile()
{

    QDir dir = appsettings->value("writedir").toString();
    qDebug() << dir.absolutePath();
    //dialog
    QString selFilter = "All files (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(
                this,
                "Save file",
                dir.absolutePath(),
                "Text files (*.txt);;All files (*.*)",
                &selFilter);

    dir = QFileInfo(fileName).absoluteDir();
    appsettings->setValue("writedir", dir.absolutePath());

    // write text file
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        QString str = txt_clicked->toPlainText();
        stream << str;
    }
}



void UguisViewer::readPrevFile()
{
    readIthFileInCurrentDir(-1);
}



void UguisViewer::readNextFile()
{
    readIthFileInCurrentDir(1);
}



bool UguisViewer::readIthFileInCurrentDir(int i)
{
    QDir dir = appsettings->value("readdir").toString();
    QStringList filter;
    filter += "*.json";
    int ifile = 0;

    QStringList path_list = dir.entryList(filter, QDir::Files);

    for (int j = 0; j < path_list.size(); j++){
        qDebug() << path_list[j];
        if (path_list[j] == QFileInfo(fileName).fileName()){
            ifile = j;
            break;
        }
    }

    if (ifile + i < 0){
        but_prevfile->setDisabled(true);
        return false;
    }
    else if (ifile + i == path_list.size()){
        but_nextfile->setDisabled(true);
        return false;
    }
    else{
        but_prevfile->setEnabled(true);
        but_nextfile->setEnabled(true);
    }

    QString ithFileName = path_list[ifile + i];
    QString ithFileNamePath = appsettings->value("readdir").toString() + "/" + ithFileName;

    ImportFile(ithFileNamePath);

    return true;
}
