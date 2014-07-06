/******************************************************************************
* Copyright (c) 2014, Mikhail Gusev <gusevmihs at gmail dot com>
*
* This file is part of LCDPainter.
*
* LCDPainter is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* LCDPainter is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with LCDPainter.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::~MainWindow()
{
    delete ui;
}


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sX = 800;
    sY = 800;

    setGeometry(100,50,835,632);

    s = new mainScene;
    ui->graphicsView->setScene(s);
    ui->graphicsView->adjustSize();
    ui->graphicsView->setTransformationAnchor(QGraphicsView::NoAnchor);
    ui->graphicsView->setMouseTracking(true); //иначе событие МаусМув будет вызываться только при нажатой кнопке
    //ui->graphicsView->setRenderHints(QPainter::HighQualityAntialiasing); //возможно для стирания огрызков рамки
    s2 = new dopScene;
    ui->graphicsView_2->setScene(s2);
    ui->graphicsView_2->setMaximumWidth(s2->pic->sizeX*s2->step);
    ui->graphicsView_2->setMaximumHeight(s2->pic->sizeY*s2->step);
    ui->graphicsView_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_2->adjustSize();
    ui->lineEdit->setText(QString::fromUtf8("ABC"));
    //СДЕЛАТЬ: ЕЩЁ ОДНУ СЦЕНУ С ПРЕДВ. ПРОСМОТРОМ ТЕКСТА - РЕШИТ ПРОБЛЕМУ РАСШИРЕНИЯ ЛЕЙБЛА
    //ИЛИ ПОСМОТРЕТЬ ПО ССЫЛКЕ В СЕРЕДИНЕ: http://www.cyberforum.ru/qt/thread450074-page2.html
    ui->label->setText(QString::fromUtf8("ABC"));
        ui->label->setFont(QFont("Times New Roman",16));
        //font.setFamily("Times New Roman");
        //font.setFo
        ui->label->setMaximumHeight(50); //выше этого растягиваться не будет. 50 - оптимально
        //ui->label->setMaximumWidth(100);
        //РАБОТАЕТ (не совсем так как надо) - //ui->groupBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Minimum);
        // ui->groupBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Minimum);
        //elideMode
        //textFlag
        //ui->groupBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Minimum);
        ui->label->setWordWrap(true);
        ui->label->setScaledContents(true);
    ui->pushButton_2->setText(ui->label->font().family() + ", " + QString::number(ui->label->font().pointSize()));
    ui->progressBar->hide();

    dlg_path = new QFileDialog (this);
    dlg_path->setAcceptMode(QFileDialog::AcceptSave);
    dlg_path->setFileMode(QFileDialog::AnyFile);
    dlg_path->setViewMode(QFileDialog::List);
    dlg_path->setDefaultSuffix("txt");
    dlg_path->setNameFilter("*.txt");
    dlg_path->setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dlg_path->setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dlg_path->setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dlg_path->setLabelText(QFileDialog::Accept,QString::fromUtf8("Сохранить"));
    dlg_path->setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    //dlg_path->setLabelText(QFileDialog::,QString::fromUtf8("Открыть"));
    dlg_path->setWindowTitle(QString::fromUtf8("Сохранить ..."));

    set_color = new QAction(QString::fromUtf8("Цвет"),ui->menuBar);
    set_color->setCheckable(true);
    set_transparent = new QAction(QString::fromUtf8("Прозрачность"),ui->menuBar);
    set_transparent->setCheckable(true);
    m_settings = new QMenu(QString::fromUtf8("Настройки"),ui->menuBar);
    m_settings->addAction(set_color);
    m_settings->addAction(set_transparent);
    m_about = new QMenu(QString::fromUtf8("О программе"),ui->menuBar);
    ui->menuBar->addMenu(m_settings);
    ui->menuBar->addMenu(m_about);

    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->setFloatable(false);
    QAction* tempAct = ui->mainToolBar->toggleViewAction();
    tempAct->setVisible(false); //убираем возможность скрывать ТулБар

    tool_save_area = new QAction(QIcon(":/save_area.png"),QString::fromUtf8("Сохранить область в виде кода"),ui->mainToolBar);
    tool_save_all = new QAction(QIcon(":/save_all.png"),QString::fromUtf8("Сохранить весь экран ЖК-дисплея в виде кода"),ui->mainToolBar);
    tool_clear_all = new QAction(QIcon(":/clear_all.png"),QString::fromUtf8("Очистить экран ЖК-дисплея"),ui->mainToolBar);
    tool_area = new QAction(QIcon(":/area.png"),QString::fromUtf8("Выделить область экрана ЖКИ (по-байтно)"),ui->mainToolBar);
    ui->mainToolBar->addAction(tool_save_area);
    ui->mainToolBar->addAction(tool_save_all);
    ui->mainToolBar->addAction(tool_clear_all);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(tool_area);

    connect(s, SIGNAL(clickOnScene(int, int)), s2, SLOT(onClickOnScene(int, int)));
    connect(s, SIGNAL(statusMessageShow(int, int)), this, SLOT(onStatusMessageShow(int, int)));
    connect(this, SIGNAL(showText(const QImage*, int, bool)), s, SLOT(onShowText(const QImage*, int, bool)));
    connect(this, SIGNAL(confirmText(QProgressBar*)), s, SLOT(onConfirmText(QProgressBar*)));
    connect(s, SIGNAL(addDopPixel(int, int, int)), s2, SLOT(onAddDopPixel (int, int, int)));
    connect(this, SIGNAL(deleteText()), s, SLOT(onDeleteText()));
    connect(this, SIGNAL(isTextVisible(bool*)), s, SLOT(onIsTextVisible(bool*)));
    connect(tool_clear_all, SIGNAL(triggered()), s, SLOT(onClearAllScreen()));
    connect(tool_clear_all, SIGNAL(triggered()), s2, SLOT(onClearAllScreen()));
    connect(set_transparent, SIGNAL(triggered(bool)), s, SLOT(onSetPictureTransparent(bool)));
    connect(set_color, SIGNAL(triggered(bool)), s, SLOT(onSetPictureColored(bool)));
    connect(tool_save_all, SIGNAL(triggered()), this, SLOT(onSaveAllDialog()));
    connect(tool_save_area, SIGNAL(triggered()), this, SLOT(onSaveAreaDialog()));
    connect(this, SIGNAL(menuSaveAll(QString)), s, SLOT(onSaveAll(QString)));
    connect(this, SIGNAL(menuSaveArea(QString)), s, SLOT(onSaveArea(QString)));
    connect(tool_area, SIGNAL(triggered()), s, SLOT(onSelectArea()));

    mainSceneGeometry = ui->graphicsView->sceneRect(); //теперь сцена будет только такого размера -
    ui->graphicsView->setSceneRect(mainSceneGeometry); //(не будут появляться другие полосы прокрутки)
}


void MainWindow::onStatusMessageShow(int x, int y)
{
    ui->statusBar->showMessage(QString::fromUtf8("Координаты точки: ")+"["+QString::number(x+1)+", "+QString::number(y+1)+"]",2000);
}


void MainWindow::on_pushButton_clicked()
{
    img = QImage(sX,sY,QImage::Format_RGB32);
    img.fill(qRgb(255,255,255));
    QPainter painter(&img);
    painter.setFont(ui->label->font());
    painter.drawText(0,0,img.width(),img.height(),Qt::AlignLeft,ui->label->text());
    painter.end();
    bool fl;
    int i,j;
    int nomi1=-1,nomj1=-1,nomi2=-1,nomj2=-1;
    //ищем начало рамки по Х
    fl = false;
    i = 0;
    while ((i<img.width())&&(fl==false))
    {
        j = 0;
        while ((j<img.height())&&(fl==false))
        {
            if (img.pixel(i,j) != qRgb(255,255,255))
            {
                nomi1 = i;
                fl = true;
            }
            j++;
        }
        i++;
    }
    //ищем начало рамки по Y
    fl = false;
    j = 0;
    while ((j<img.height())&&(fl==false))
    {
        i = 0;
        while ((i<img.width())&&(fl==false))
        {
            if (img.pixel(i,j) != qRgb(255,255,255))
            {
                nomj1 = j;
                fl = true;
            }
            i++;
        }
        j++;
    }
    //ищем конец рамки по Х
    fl = false;
    i = img.width()-1;
    while ((i>0)&&(fl==false))
    {
        j = img.height()-1;
        while ((j>0)&&(fl==false))
        {
            if (img.pixel(i,j) != qRgb(255,255,255))
            {
                nomi2 = i;
                fl = true;
            }
            j--;
        }
        i--;
    }
    //ищем конец рамки по Y
    fl = false;
    j = img.height()-1;
    while ((j>0)&&(fl==false))
    {
        i = img.width()-1;
        while ((i>0)&&(fl==false))
        {
            if (img.pixel(i,j) != qRgb(255,255,255))
            {
                nomj2 = j;
                fl = true;
            }
            i--;
        }
        j--;
    }
    //когда нашли все 4 координаты (или не нашли):
    if (!((nomi1==-1)||(nomj1==-1)||(nomi2==-1)||(nomj2==-1)||(nomi1>nomi2)||(nomj1>nomj2)))
    {
        img = img.copy(nomi1,nomj1,nomi2-nomi1+1,nomj2-nomj1+1);
        emit showText(&img,ui->horizontalSlider->value(),true);
        ui->toolButton->setEnabled(true);
        ui->toolButton_2->setEnabled(true);
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->label->font(), this);
    if (ok)
    {
            //int w = ui->label->width();
            //int h = ui->label->height();
            //ui->label->setMaximumSize(ui->label->width(),ui->label->height());
            //ui->groupBox->setMaximumSize(ui->groupBox->width(),ui->groupBox->height());
            //ui->label->setMinimumSize(30,10);
            //ui->label->setFixedSize(ui->label->width(),ui->label->height());
            //сделать на это время у формы не расширяемый размер (ширину)
        if (font.pointSize() > 72) font.setPointSize(72);
        //if (font.pointSize() > 16) font.setPointSize(16);
        else if (font.pointSize() < 1) font.setPointSize(1);
        ui->label->setFont(font);
            //ui->groupBox->resize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
            //СДЕЛАТЬ - разобраться с изменениями размеров лейбла
        ui->pushButton_2->setText(font.family() + ", " + QString::number(font.pointSize()));
            //ui->label->resize(10,10);
            //ui->label
            //ui->label->setMaximumSize(16777215,16777215);
            //ui->label->setMaximumSize(16777215,16777215);
            //ui->groupBox->setMaximumSize(16777215,16777215);
            //ui->label->resize(w,h);
    }
    //if (ui->label->picture() == 0) ui->statusBar->showMessage("qwerty");
}
/*
void MainWindow::resizeEvent (QResizeEvent * event)
{
    //ui->label->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    //ui->label->resize(ui->label->width()+(event->size().width()-event->oldSize().width()),ui->label->height()+(event->size().height()-event->oldSize().height()));
}*/


void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    ui->label->setText(ui->lineEdit->text());
}


void MainWindow::on_toolButton_clicked()
{
    ui->lineEdit->hide();
    ui->progressBar->show();
    emit confirmText(ui->progressBar);
    ui->toolButton->setEnabled(false);
    ui->toolButton_2->setEnabled(false);
    ui->progressBar->hide();
    ui->lineEdit->show();
}


void MainWindow::on_toolButton_2_clicked()
{
    emit deleteText();
    ui->toolButton->setEnabled(false);
    ui->toolButton_2->setEnabled(false);
}


void MainWindow::on_horizontalSlider_sliderReleased()
{
    bool ok = false;
    emit isTextVisible(&ok);
    if (ok == true) emit showText(&img,ui->horizontalSlider->value(),false);
}


void MainWindow::onSaveAllDialog()
{
        QString path = "all_screen.txt";
        dlg_path->setDirectory(QDir()); //ставим текущую директорию
        dlg_path->selectFile("all_screen.txt"); //имя файла по умолчанию - all_screen.txt
        if (dlg_path->exec())
        {
            //СДЕЛАТЬ (возможно через сигнал accepted) - не смотря на то, что задан суффикс по дефолту - при сохранении файла с другим суффиксом (не .txt) диалог не выдаст предупреждение
            path = dlg_path->selectedFiles().first();
            if (!path.endsWith(".txt",Qt::CaseInsensitive)) path = path + ".txt";
            emit menuSaveAll(path);
        }
}


void MainWindow::onSaveAreaDialog()
{
    if (s->is_area_shown == true)
    {
        QString path = "symbol.txt";
        dlg_path->setDirectory(QDir()); //ставим текущую директорию
        dlg_path->selectFile("symbol.txt"); //имя файла по умолчанию - all_screen.txt
        if (dlg_path->exec())
        {
            //СДЕЛАТЬ (возможно через сигнал accepted) - не смотря на то, что задан суффикс по дефолту - при сохранении файла с другим суффиксом (не .txt) диалог не выдаст предупреждение
            path = dlg_path->selectedFiles().first();
            if (!path.endsWith(".txt",Qt::CaseInsensitive)) path = path + ".txt";
            emit menuSaveArea(path);
        }
    }
}
