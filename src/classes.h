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

#ifndef CLASSES_H
#define CLASSES_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QProgressBar>
#include <QFile>
#include <QTextStream>

class picture
{
    public:
     picture(int x, int y);
     int sizeX;
     int sizeY;
};

class mainScene: public QGraphicsScene
{
    Q_OBJECT
    public:
     mainScene();
     QList < QList < QGraphicsRectItem*> > mas;
     picture* pic;
     qreal step;
     QColor color0;
     QColor color1;
     QColor colorB;
     bool is_colored; //показывать ли на главной сцене в цвете
     bool is_transparent; //показывать ли на главной сцене прозрачность

     QGraphicsPixmapItem* textPix; //показанная на экране картинка с вновь добавленным текстом
     QImage textImg; //"виртуальная" картинка с вновь добавленным текстом
     bool textLetTrack; //можно ли двигать картинку с текстом вслед за курсором (перемещать её)

     QGraphicsRectItem* areaRect; //сама рамка
     bool is_area_shown; //показана ли рамка выделения области
     QPair <int, int> areaP; //координаты позиции и размера рамки
     QPair <int, int> areaS;
     int areaLetAction; //можно ли изменять размер рамки или двигать её вслед за курсором (и как):
                        //0-нельзя ничего, 1 - можно двигать; далее трансформация за сторону:
                        //2-правая сторона(право/лево), 3 - левая сторона (право/лево)
                        //4 - нижняя сторона (вверх/вниз), 5 - верхняя, 6 - вниз+вправо,
                        //...
     virtual void mousePressEvent (QGraphicsSceneMouseEvent *mouseEvent);
     virtual void mouseMoveEvent (QGraphicsSceneMouseEvent *mouseEvent);
     virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *mouseEvent);
    signals:
     void clickOnScene (int pos, int but);
     void statusMessageShow (int x, int y);
     void addDopPixel (int j, int i, int col);
     void repaintScene ();
    public slots:
     void onShowText (const QImage* img, int black, bool is_new);
     void onConfirmText (QProgressBar* pb);
     void onDeleteText ();
     void onIsTextVisible (bool* ok);
     void onClearAllScreen ();
     void onSetPictureTransparent (bool is);
     void onSetPictureColored (bool is);
     void onSaveAll (QString path);
     void onSaveArea (QString path);
     void onSelectArea ();

};

class dopScene: public QGraphicsScene
{
    Q_OBJECT
    public:
     dopScene();
     QList < QList < QGraphicsRectItem*> > mas;
     picture* pic;
     qreal step;
     QColor color0;
     QColor color1;
     QColor colorB;
    public slots:
     void onClickOnScene (int pos, int but);
     void onAddDopPixel (int j, int i, int col);
     void onClearAllScreen ();
};

#endif // CLASSES_H
