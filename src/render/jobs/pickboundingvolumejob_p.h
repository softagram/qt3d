/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H
#define QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "abstractpickingjob_p.h"
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/qray3d_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/qboundingvolumeprovider_p.h>
#include <Qt3DRender/private/qcollisionqueryresult_p.h>
#include <Qt3DRender/private/pickboundingvolumeutils_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/qpickevent.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace PickingUtils {
typedef QVector<RayCasting::QCollisionQueryResult::Hit> HitList;
}

class QT3DRENDERSHARED_PRIVATE_EXPORT PickBoundingVolumeJob : public AbstractPickingJob
{
public:
    PickBoundingVolumeJob();

    void setRoot(Entity *root);
    void setMouseEvents(const QList<QPair<QObject*, QMouseEvent>> &pendingEvents);
    void setKeyEvents(const QList<QKeyEvent> &pendingEvents);
    void markPickersDirty();
    bool pickersDirty() const { return m_pickersDirty; }

    // For unit tests
    inline HObjectPicker currentPicker() const { return m_currentPicker; }
    inline QVector<HObjectPicker> hoveredPickers() const { return m_hoveredPickers; }
    bool runHelper() override;

protected:
    void dispatchPickEvents(const QMouseEvent &event,
                            const PickingUtils::HitList &sphereHits,
                            QPickEvent::Buttons eventButton,
                            int eventButtons,
                            int eventModifiers,
                            bool allHitsRequested);

private:
    void clearPreviouslyHoveredPickers();

    QList<QPair<QObject*, QMouseEvent>> m_pendingMouseEvents;
    QList<QKeyEvent> m_pendingKeyEvents;
    bool m_pickersDirty;
    bool m_oneHoverAtLeast;
    HObjectPicker m_currentPicker;
    QVector<HObjectPicker> m_hoveredPickers;
    QVector<HObjectPicker> m_hoveredPickersToClear;
};

typedef QSharedPointer<PickBoundingVolumeJob> PickBoundingVolumeJobPtr;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H
