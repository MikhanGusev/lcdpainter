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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QFontDialog>
#include <QProgressBar>
#include <QFileDialog>

#include "classes.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
     explicit MainWindow(QWidget *parent = 0);
     ~MainWindow();
     //virtual void resizeEvent (QResizeEvent * event);
    private:
     Ui::MainWindow *ui;

     mainScene* s; //приблежённый вид ЖК экрана (основная сцена)
     dopScene* s2; //предварительный просмотр ЖК экрана (дополнительная сцена)

     QImage img; //картинка для отрисовки на дисплеях
     int sX; //размеры картинки
     int sY;
     QFileDialog* dlg_path; //диалог для сохранения директории, в которую будут сохранятся создаваемые файлы
     QRectF mainSceneGeometry; //геометрия главной сцены при отрисовки
     QMenu* m_settings; //меню - Настройки
     QMenu* m_about;
      QAction* set_transparent; //меню - Настройки -> прозрачность
      QAction* set_color;
     QAction* tool_save_area; //кнопки панели инструментов
     QAction* tool_save_all;
     QAction* tool_clear_all;
     QAction* tool_area;
     QFont font; //шрифт, заданный в диалоге
    public slots:
     void onStatusMessageShow (int x, int y);
     void onSaveAllDialog ();
     void onSaveAreaDialog ();
    private slots:
     void on_pushButton_clicked ();
     void on_pushButton_2_clicked ();
     void on_lineEdit_textChanged (const QString &arg1);
     void on_toolButton_clicked ();
     void on_toolButton_2_clicked ();
     void on_horizontalSlider_sliderReleased ();
    signals:
     void showText (const QImage* img, int black, bool is_new);
     void confirmText (QProgressBar* pb);
     void deleteText ();
     void isTextVisible (bool* ok);
     void menuSaveAll (QString path);
     void menuSaveArea (QString path);
};

#endif // MAINWINDOW_H
