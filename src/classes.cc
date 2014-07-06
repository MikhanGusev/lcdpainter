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

#include "classes.h"

picture::picture(int x, int y)
{
    sizeX = x;
    sizeY = y;
}


mainScene::mainScene()
{
    pic = new picture(122,32);
    step = 12.0; //изначально было 16.0
    color0 = QColor(255,255,255);
    color1 = QColor(0,0,0);
    colorB = QColor (240,240,240);
    is_colored = false;
    is_transparent = false;

    textPix = new QGraphicsPixmapItem;
    addItem(textPix);
    textPix->hide();
    textLetTrack = false;

    is_area_shown = false;
    areaP.first = 0; //просто по умолчанию координаты небольшой рамки
    areaP.second = 0;
    areaS.first = 10;
    areaS.second = 16;
    areaLetAction = 0;

    int k = 0;
    setBackgroundBrush(QBrush(colorB));
    QGraphicsRectItem* box;
    QList <QGraphicsRectItem*> temp;
    for (int i=0; i<pic->sizeY; i++)
    {
        temp.clear();
        for (int j=0; j<pic->sizeX; j++)
        {
            box = new QGraphicsRectItem;
            box->setRect(j*step,i*step,step,step);
            box->setBrush(QBrush(QColor(color0)));
            box->setPen(QPen(QBrush(QColor(colorB)),2));
            box->setData(0,k);
            addItem(box);
            temp.append(box);
            k++;
        }
        mas.append(temp);
    }
}


dopScene::dopScene()
{
    pic = new picture(122,32);
    step = 2.0;
    color0 = QColor(0,64,128);
    color1 = QColor(255,255,255);
    colorB = QColor(0,240,250);
    int k = 0;

    setBackgroundBrush(QBrush(colorB));
    QGraphicsRectItem* box;
    QList <QGraphicsRectItem*> temp;
    for (int i=0; i<pic->sizeY; i++)
    {
        temp.clear();
        for (int j=0; j<pic->sizeX; j++)
        {
            box = new QGraphicsRectItem;
            box->setRect(j*step,i*step,step,step);
            box->setBrush(QBrush(color0));
            box->setPen(QPen(Qt::NoPen));
            box->setData(0,k);
            addItem(box);
            temp.append(box);
            k++;
        }
        mas.append(temp);
    }
}


void mainScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsItem* item = itemAt(mouseEvent->scenePos());
    if (item != 0)
    {
        if (item->data(0) == "text") //если нажимаем на добавленную картинку с текстом
        {
            textLetTrack = true;
        }
        else if (item->data(0) == "area") //если нажимаем на рамку (область выделения)
        {
            //СДЕЛАТЬ - определение не по areaS а по реальным коор-ам рамки
            if ((mouseEvent->scenePos().x()>areaS.first*step-step)&&(mouseEvent->scenePos().y()>areaS.second*step-step))
                areaLetAction = 6; //меняем размеры рамки
            else
                areaLetAction = 1; //двигаем рамку
        }
        else //если нажимаем на квадратик (пиксель) экрана
        {
            if (mouseEvent->button() == Qt::LeftButton)
                static_cast <QGraphicsRectItem*> (item)->setBrush(QBrush(color1));
            else if (mouseEvent->button() == Qt::RightButton)
                static_cast <QGraphicsRectItem*> (item)->setBrush(QBrush(color0));
            emit clickOnScene(item->data(0).toInt(), mouseEvent->button());
        }
    }
}


void mainScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qreal cx, cx2;
    qreal cy, cy2;
    int nx,nx2;
    int ny,ny2;
    int w,v;
    if (textLetTrack == true) //если перемещали картинку с текстом
    {
        nx = qRound(textPix->scenePos().x()/step);
        ny = qRound(textPix->scenePos().y()/step);
        cx = nx*step;
        cy = ny*step;
        textPix->translate(cx-textPix->scenePos().x(),cy-textPix->scenePos().y());
        textLetTrack = false;
        this->update(); //обновляем ВСЮ сцену, чтобы убрать огрызки (долго делает)
    }
    if (areaLetAction != 0) //если было какое-то действие с рамкой:
    {
        //вычисляем "пиксели" на дисплее из реальных координат
        nx = qRound(areaRect->rect().x()/step);
        ny = qRound(areaRect->rect().y()/step);
        nx2 = qRound(areaRect->rect().width()/step);
        ny2 = qRound(areaRect->rect().height()/step);
        switch (areaLetAction) //в зависимости от действия мышкой:
        {       
            case 1: //либо выравниваем её новое положение
                    //возвращаем рамку на дисплей, если она сдвинута за его пределы
                    if ((nx<0)&&(ny<0)) { nx = 0; ny = 0; }
                    else if ((nx<0)&&(ny>=0)) nx = 0;
                    else if ((ny<0)&&(nx>=0)) ny = 0;
                    if (((nx+nx2)>pic->sizeX)&&((ny+ny2)>pic->sizeY)) { nx = pic->sizeX-nx2; ny = pic->sizeY-ny2; }
                    else if (((nx+nx2)>pic->sizeX)&&((ny+ny2)<=pic->sizeY)) nx = pic->sizeX-nx2;
                    else if (((ny+ny2)>pic->sizeY)&&((nx+nx2)<=pic->sizeX)) ny = pic->sizeY-ny2;
                cx = nx*step;
                cy = ny*step;
                areaRect->setRect(cx,cy,areaRect->rect().width(),areaRect->rect().height());
            break;
            case 6: //либо выравниваем её новый размер
                    //выравниваем высоту рамки до размера, кратного байту (8 клеток)
                    w = ny2/8;
                    v = ny2%8;
                    if ((w<=0)||((w==1)&&(v<5))) ny2 = 8;
                    else if (((w==1)&&(v>=5))||((w==2)&&(v<5))) ny2 = 16;
                    else if (((w==2)&&(v>=5))||((w==3)&&(v<5))) ny2 = 24;
                    else if ((w>=4)||((w==3)&&(v>=5))) ny2 = 32;
                    //если длина рамки превышает длину дисплея - тоже выравниваем
                    if (nx2>pic->sizeX) nx2 = pic->sizeX;
                    //если длина рамки меньше 1 "пикселя" (глюк) - делаем 1
                    else if (nx2<1) nx2 = 1;
                    //возвращаем рамку на дисплей, если она сдвинута за его пределы
                    if ((nx<0)&&(ny<0)) { nx = 0; ny = 0; }
                    else if ((nx<0)&&(ny>=0)) nx = 0;
                    else if ((ny<0)&&(nx>=0)) ny = 0;
                    if (((nx+nx2)>pic->sizeX)&&((ny+ny2)>pic->sizeY)) { nx = pic->sizeX-nx2; ny = pic->sizeY-ny2; }
                    else if (((nx+nx2)>pic->sizeX)&&((ny+ny2)<=pic->sizeY)) nx = pic->sizeX-nx2;
                    else if (((ny+ny2)>pic->sizeY)&&((nx+nx2)<=pic->sizeX)) ny = pic->sizeY-ny2;
                cx = nx*step;
                cy = ny*step;
                cx2 = nx2*step;
                cy2 = ny2*step;
                areaRect->setRect(cx,cy,cx2,cy2);
            break;
            default:
            break;
        }
        areaP.first = nx; //сохраняем новые координаты в шагах
        areaP.second = ny;
        areaS.first = nx + nx2;
        areaS.second = ny + ny2;
        areaLetAction = 0; //больше ничего не делать с рамкой
        this->update(); //обновляем ВСЮ сцену, чтобы убрать огрызки (долго делает)
    }
}


void mainScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QVariant var;
    int k;
    QGraphicsItem* item = itemAt(mouseEvent->scenePos());
    if (item != 0)
    {
        var = item->data(0);
        if (var == "area") //если курсор над областью выделения (рамкой)
        {
            //СДЕЛАТЬ - определение не по areaS а по реальным коор-ам рамки
            if ((mouseEvent->scenePos().x()>areaS.first*step-step)&&(mouseEvent->scenePos().y()>areaS.second*step-step))
                areaRect->setCursor(Qt::SizeFDiagCursor);
            else areaRect->setCursor(Qt::OpenHandCursor);
        }
        else if (var != "text") //если курсор над квадратиком экрана
        {
            k = item->data(0).toInt();
            emit statusMessageShow(k/pic->sizeX,k%pic->sizeX);
            //emit statusMessageShow(static_cast <QGraphicsRectItem*> (item)->brush().color().red(),static_cast <QGraphicsRectItem*> (item)->brush().color().blue());
        }
    }
    if (textLetTrack == true) //если было нажатие по картинке с текстом - перемещаем её
    {
        textPix->translate(mouseEvent->scenePos().x()-mouseEvent->lastScenePos().x(),mouseEvent->scenePos().y()-mouseEvent->lastScenePos().y());
    }
    switch (areaLetAction) //действия с рамкой, если на неё нажали
    {
        case 1: //если было нажатие на центр рамки - двигаем её
            areaRect->setRect(areaRect->rect().x()+(mouseEvent->scenePos().x()-mouseEvent->lastScenePos().x()),areaRect->rect().y()+(mouseEvent->scenePos().y()-mouseEvent->lastScenePos().y()),areaRect->rect().width(),areaRect->rect().height());
        break;
        case 6: //если было нажатие на край рамки - трансформируем её
            //чтобы не была меньше чем одна клетка - проверяем
            if (areaRect->rect().width()<step) areaRect->setRect(areaRect->rect().x(),areaRect->rect().y(),step,areaRect->rect().height()+(mouseEvent->scenePos().y()-mouseEvent->lastScenePos().y()));
            if (areaRect->rect().height()<step) areaRect->setRect(areaRect->rect().x(),areaRect->rect().y(),areaRect->rect().width()+(mouseEvent->scenePos().x()-mouseEvent->lastScenePos().x()),step);
            areaRect->setRect(areaRect->rect().x(),areaRect->rect().y(),areaRect->rect().width()+(mouseEvent->scenePos().x()-mouseEvent->lastScenePos().x()),areaRect->rect().height()+(mouseEvent->scenePos().y()-mouseEvent->lastScenePos().y()));
        break;
        default:
        break;
    }
}



void dopScene::onClickOnScene (int pos, int but)
{
    if (but == Qt::LeftButton)
        mas[pos/pic->sizeX][pos%pic->sizeX]->setBrush(QBrush(color1));
    else if (but == Qt::RightButton)
        mas[pos/pic->sizeX][pos%pic->sizeX]->setBrush(QBrush(color0));
}



void mainScene::onShowText(const QImage *img, int black, bool is_new)
{
    if (is_area_shown == true)
    {
        removeItem(areaRect); //убираем рамку выделения области (если она была показана)
        is_area_shown = false;
    }

    textImg = QImage(img->width()*step,img->height()*step,QImage::Format_ARGB32);
    textImg.fill(qRgba(255,255,255,0));
    QPainter painter(&textImg);

    int color;
    int i,j;
    for (i=0; i<img->width(); i++)
    {
        for (j=0; j<img->height(); j++)
        {
            color = qGray(img->pixel(i,j));
            if (is_colored == true)
            {
                if (color < black) painter.setBrush(QBrush(img->pixel(i,j)));
                else
                {
                    if (is_transparent == true) painter.setBrush(Qt::NoBrush);
                    else painter.setBrush(QBrush(color0));
                }
            }
            else
            {
                if (color < black) painter.setBrush(QBrush(color1));
                else
                {
                    if (is_transparent == true) painter.setBrush(Qt::NoBrush);
                    else painter.setBrush(QBrush(color0));
                }
            }
            painter.setPen(QPen(QBrush(QColor(colorB)),2));
            painter.drawRect(i*step,j*step,1*step,1*step);
        }
    }

    painter.setBrush(QBrush(QColor(255,255,255,5))); //почти прозрачный
    painter.setPen(QPen(QBrush(QColor(255,0,0)),2));
    painter.drawRect(0,0,img->width()*step,img->height()*step);
    painter.end();

    removeItem(textPix);
    if (is_new == true) textPix = new QGraphicsPixmapItem; //если надо поместить в точку 0,0 - заного создаём
    textPix->setData(0,"text");
    textPix->setPixmap(QPixmap::fromImage(textImg));
    addItem(textPix);
    textPix->show();
}



void mainScene::onConfirmText(QProgressBar* pb)
{
    QPointF coord;
    int xx,yy;
    //если картинка полностью вынесена за пределы рабочей области - не отрисовываем, иначе отрисовываем её часть или всю
    if (!((textPix->scenePos().y() >= mas[0][0]->scenePos().y()+pic->sizeY*step)
     ||(textPix->scenePos().y()+textImg.height() <=  mas[0][0]->scenePos().y())
     ||(textPix->scenePos().x()+textImg.width() <= mas[0][0]->scenePos().x())
     ||(textPix->scenePos().x() >= mas[0][0]->scenePos().x()+pic->sizeX*step)))
    {
        if (is_area_shown == true)
            areaRect->hide(); //прячем рамку, чтобы текст запечатлелся, потом её возвращаем
        pb->setMaximum(pic->sizeX);
        pb->setValue(0);
        for (int i=0; i<pic->sizeX; i++)
        {
            pb->setValue(i+1);
            for (int j=0; j<pic->sizeY; j++)
            {
                xx = mas[j][i]->data(0).toInt()%pic->sizeX; //высчитываем номер квадратика (не обязательно)
                yy = mas[j][i]->data(0).toInt()/pic->sizeX;
                coord.setX(xx*step); //берём его реальные координаты (они совпадают с координатами сцены)
                coord.setY(yy*step);
                if ((itemAt(coord)!=0)&&(itemAt(coord)->data(0)=="text"))
                {
                    coord = textPix->mapFromScene(coord); //преобразуем координаты сцены (а т.е. квадратика) в координаты картинки
                    QColor c = textImg.pixel(qRound(coord.x())+2,qRound(coord.y())+2); //коорд-ты пикселя на картинке те же, только сдвинутые на 2 чтобы не трогать рамку 
                    if (is_transparent == true)
                    {
                        if (c != qRgba(255,255,255,0)) //меняем цвет, если он не прозрачный. Иначе - оставляем то, что есть
                        {
                            mas[j][i]->setBrush(QBrush(c));
                            emit addDopPixel(j,i,1);
                        }
                    }
                    else
                    {
                        mas[j][i]->setBrush(QBrush(c));
                        if (c !=  qRgb(255,255,255)) emit addDopPixel(j,i,1);
                        else addDopPixel(j,i,0);
                    }
                }
            }
        }
        if (is_area_shown == true) areaRect->show();
    }
    textPix->hide();
}



void dopScene::onAddDopPixel (int j, int i, int col)
{
    if (col == 1) mas[j][i]->setBrush(QBrush(color1));
    else if (col == 0) mas[j][i]->setBrush(QBrush(color0));
    else if (col == 2) mas[j][i]->setBrush(QBrush(colorB));
}



void mainScene::onDeleteText()
{
    textPix->hide();
}



void mainScene::onIsTextVisible(bool* ok)
{
   *ok = textPix->isVisible();
}



void mainScene::onClearAllScreen()
{
    for(int i=0; i<pic->sizeX; i++)
    for (int j=0; j<pic->sizeY; j++)
        mas[j][i]->setBrush(QBrush(color0));
    this->update();
}



void dopScene::onClearAllScreen()
{
    for(int i=0; i<pic->sizeX; i++)
    for (int j=0; j<pic->sizeY; j++)
        mas[j][i]->setBrush(QBrush(color0));
}


void mainScene::onSetPictureTransparent(bool is)
{
    is_transparent = is;
}


void mainScene::onSetPictureColored(bool is)
{
    is_colored = is;
}


//СДЕЛАТЬ - Сохранение НЕ для дисплея фиксированной длины и высоты (32x122)! (ПОКА ТАК)
void mainScene::onSaveAll(QString path)
{
    QFile file (path);
    //СДЕЛАТЬ - проверку на недопустимое имя файла и выдачу сообщения
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out << "unsigned char mas[4][122] = //" << QString::fromUtf8("122x32 пикселя, каждые 8 вертикальных точек собраны в байт");
    out << "\n" << "{";
    uchar sum = 0x00;
    int i,j,k;
    for (i=0; i<4; i++)
    {
        out << "\n" << "{";
        for (j=0; j<122; j++)
        {
            sum = 0x00;
            for (k=7; k>=0; k--)
            {
                sum = (sum << 1);
                if (mas[i*8+k][j]->brush().color() != color0) sum = sum + 1;
            }
            if (j < 122-1) out << sum << ", "; else out << sum;
        }
        if (i < 4-1) out << "},"; else out << "}";
    }
    out << "\n" << "};";
}



void mainScene::onSaveArea(QString path)
{
    QFile file (path);
    //СДЕЛАТЬ - проверку на недопустимое имя файла и выдачу сообщения
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    int masx, masy;
    masx = areaS.first-areaP.first; //размер рамки по x
    masy = (areaS.second-areaP.second)/8; //размер рамки по у (по 8 пикселей)
    out << "unsigned char mas["+QString::number(masy)+"]["+QString::number(masx)+"] = ";
    out << "\n" << "{";
    uchar sum = 0x00;
    int i,j,k;
    for(j=0; j<=masy-1; j++)
    {
        out << "\n" << "{";
        for (i=areaP.first; i<=areaS.first-1; i++)
        {
            sum = 0x00;
            for (k=areaP.second+j*8+7; k>=(areaP.second+j*8); k--)
            {
                sum = (sum << 1);
                if (mas[k][i]->brush().color() != color0) sum = sum + 1;
            }
            if (i < areaS.first-1) out << sum << ", "; else out << sum;
        }
        if (j < masy-1) out << "},"; else out << "}";
    }
    out << "\n" << "};";
}



void mainScene::onSelectArea()
{
    is_area_shown = !is_area_shown;
    if (is_area_shown == true)
    {
        areaRect = new QGraphicsRectItem;
        areaRect->setBrush(QBrush(QColor(255,0,0,10)));
        areaRect->setPen(QPen(QBrush(qRgb(255,0,0)),2));
        areaRect->setRect(areaP.first*step,areaP.second*step,(areaS.first-areaP.first)*step,(areaS.second-areaP.second)*step);
        areaRect->setData(0,"area");
        addItem(areaRect);
    }
    else
    {
        removeItem(areaRect);
    }   
}


