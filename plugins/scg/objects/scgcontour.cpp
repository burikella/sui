/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "scgprecompiled.h"
#include "scgcontour.h"

#include <QVector2D>

SCgContour::SCgContour(QObject *parent) :
    SCgPointContainer(parent)
{
}

SCgContour::~SCgContour()
{

}

QPointF SCgContour::calculateDotCoordinates(qreal dotPosition, const QPointF &point) const
{
    QPointF res;

    if (dotPosition != 0)
    {
        qint32 seg_num = (qint32)dotPosition;
        qreal seg_pos = dotPosition - (qreal)seg_num;

        QPointF p2, p1;
        QPointF wpos = worldPosition();
        if (seg_num >= mPoints.size() - 1)
        {
            seg_num = (qint32)mPoints.size() - 2;
            p2 = mPoints.at(seg_num + 1) + wpos;
            p1 = mPoints.at(seg_num) + wpos;
        }else
        {
            p2 = mPoints.front() + wpos;
            p1 = mPoints.back() + wpos;
        }

        QPointF dir = p2 - p1;

        res = p1 + dir * seg_pos;
    }
    else
    {
        //! TODO: old style calculation
    }

    return res - worldPosition();
}

qreal SCgContour::calculateDotPosition(const QPointF &point) const
{
    // get sector with minimal distance to point
    // and calculates relative dot position on it
    qreal minDist = -1.f;
    qreal result = 0.f;
    qint32 sz = mPoints.size();
    QPointF wpos = worldPosition();

    for (int i = 0; i < mPoints.size(); i++)
    {
        QPointF p1 = mPoints.at((i + sz - 1) % sz) + wpos;
        QPointF p2 = mPoints.at((i + sz) % sz) + wpos;

        QVector2D v(p2 - p1);
        QVector2D vp(point - p1);

        if(v.length() == 0)
            return result;

        qreal dotPos = QVector2D::dotProduct(vp, v.normalized()) / v.length();
        QPointF p = p1 + v.toPointF() * dotPos;

        if (dotPos < 0.f || dotPos > 1.f)
            continue;

        // we doesn't need to get real length, because we need minimum
        // so we get squared length to make that procedure faster
        qreal d = QVector2D(point - p).lengthSquared();

        // compare with minimum distance
        if (minDist < 0.f || (minDist > d && d < 15.f))
        {
            minDist = d;
            result = i + dotPos - 1;
        }
    }

    return result;
}