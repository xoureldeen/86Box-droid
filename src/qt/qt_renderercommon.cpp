/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Program settings UI module.
 *
 *
 *
 * Authors: Joakim L. Gilje <jgilje@jgilje.net>
 *
 *      Copyright 2021 Joakim L. Gilje
 */

#include "qt_renderercommon.hpp"
#include "qt_mainwindow.hpp"
#include "qt_machinestatus.hpp"

#include <QPainter>
#include <QWidget>
#include <QEvent>
#include <QApplication>
#include <QFontMetrics>
#include <QStatusBar>
#include <QLayout>

#include <cmath>

extern "C" {
#include <86box/86box.h>
#include <86box/plat.h>
#include <86box/video.h>

int status_icons_fullscreen = 0;
}

RendererCommon::RendererCommon() = default;

extern MainWindow* main_window;

static void integer_scale(double *d, double *g) {
    double ratio;

    if (*d > *g) {
        ratio = std::floor(*d / *g);
        *d = *g * ratio;
    } else {
        ratio = std::ceil(*d / *g);
        *d = *g / ratio;
    }
}

void RendererCommon::onResize(int width, int height) {
    if (video_fullscreen == 0) {
        destination.setRect(0, 0, width, height);
        return;
    }
    double dx, dy, dw, dh, gsr;

    double hw = width;
    double hh = height;
    double gw = source.width();
    double gh = source.height();
    double hsr = hw / hh;

    switch (video_fullscreen_scale) {
    case FULLSCR_SCALE_INT:
        gsr = gw / gh;
        if (gsr <= hsr) {
            dw = hh * gsr;
            dh = hh;
        } else {
            dw = hw;
            dh = hw / gsr;
        }
        integer_scale(&dw, &gw);
        integer_scale(&dh, &gh);
        dx = (hw - dw) / 2.0;
        dy = (hh - dh) / 2.0;
        destination.setRect(dx, dy, dw, dh);
        break;
    case FULLSCR_SCALE_43:
    case FULLSCR_SCALE_KEEPRATIO:
        if (video_fullscreen_scale == FULLSCR_SCALE_43) {
            gsr = 4.0 / 3.0;
        } else {
            gsr = gw / gh;
        }

        if (gsr <= hsr) {
            dw = hh * gsr;
            dh = hh;
        } else {
            dw = hw;
            dh = hw / gsr;
        }
        dx = (hw - dw) / 2.0;
        dy = (hh - dh) / 2.0;
        destination.setRect(dx, dy, dw, dh);
        break;
    case FULLSCR_SCALE_FULL:
    default:
        destination.setRect(0, 0, hw, hh);
        break;
    }
}

void RendererCommon::drawStatusBarIcons(QPainter* painter)
{
    uint32_t x = 0;
    auto prevcompositionMode = painter->compositionMode();
    painter->setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOver);
    for (int i = 0; i < main_window->statusBar()->children().count(); i++)
    {
        QLabel* label = qobject_cast<QLabel*>(main_window->statusBar()->children()[i]);
        if (label)
        {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            const QPixmap pixmap = label->pixmap();
#else
            const QPixmap pixmap = label->pixmap() ? *label->pixmap() : QPixmap();
#endif
            if (!pixmap.isNull())
            {
                painter->setBrush(QColor(0, 0, 0, 255));
                painter->fillRect(x, painter->device()->height() - pixmap.height() - 5, pixmap.width(), pixmap.height() + 5, QColor(0, 0, 0, 127));
                painter->drawPixmap(x + main_window->statusBar()->layout()->spacing() / 2, painter->device()->height() - pixmap.height() - 3, pixmap);
                x += pixmap.width();
                if (i <= main_window->statusBar()->children().count() - 3)
                {
                    painter->fillRect(x, painter->device()->height() - pixmap.height() - 5, main_window->statusBar()->layout()->spacing(), pixmap.height() + 5, QColor(0, 0, 0, 127));
                    x += main_window->statusBar()->layout()->spacing();
                }
                else painter->fillRect(x, painter->device()->height() - pixmap.height() - 4, 4, pixmap.height() + 4, QColor(0, 0, 0, 127));
            }
        }
    }
    if (main_window->status->getMessage().isEmpty() == false)
    {
        auto curStatusMsg = main_window->status->getMessage();
        auto textSize = painter->fontMetrics().size(Qt::TextSingleLine, QChar(' ') + curStatusMsg + QChar(' '));
        painter->setPen(QColor(0, 0, 0, 127));
        painter->fillRect(painter->device()->width() - textSize.width(), painter->device()->height() - textSize.height(), textSize.width(), textSize.height(), QColor(0, 0, 0, 127));
        painter->setPen(QColor(255, 255, 255, 255));
        painter->drawText(QRectF(painter->device()->width() - textSize.width(), painter->device()->height() - textSize.height(), textSize.width(), textSize.height()), Qt::TextSingleLine, QChar(' ') + curStatusMsg + QChar(' '));
    }
    painter->setCompositionMode(prevcompositionMode);
}

bool RendererCommon::eventDelegate(QEvent *event, bool& result)
{
    switch (event->type())
    {
        default:
            return false;
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            result = QApplication::sendEvent(main_window, event);
            return true;
        case QEvent::MouseButtonPress:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:
        case QEvent::Wheel:
        case QEvent::Enter:
        case QEvent::Leave:
            result = QApplication::sendEvent(parentWidget, event);
            return true;
    }
    return false;
}
