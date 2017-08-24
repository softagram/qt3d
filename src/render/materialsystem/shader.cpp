/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "shader_p.h"
#include "renderlogging_p.h"

#include <QFile>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QMutexLocker>
#include <qshaderprogram.h>
#include <Qt3DRender/private/attachmentpack_p.h>
#include <Qt3DRender/private/graphicscontext_p.h>
#include <Qt3DRender/private/qshaderprogram_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DRender/private/managers_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Shader::Shader()
    : BackendNode(ReadWrite)
    , m_status(QShaderProgram::NotReady)
    , m_format(QShaderProgram::GLSL)
    , m_dirty(false)
{
    m_shaderCode.resize(static_cast<int>(QShaderProgram::Compute) + 1);
}

Shader::~Shader()
{
}

void Shader::cleanup()
{
    QBackendNode::setEnabled(false);
    m_status = QShaderProgram::NotReady;
    m_format = QShaderProgram::GLSL;
    m_log.clear();
    m_dirty = false;
}

void Shader::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QShaderProgramData>>(change);
    const auto &data = typedChange->data;

    for (int i = QShaderProgram::Vertex; i <= QShaderProgram::Compute; ++i)
        m_shaderCode[i].clear();

    m_shaderCode[QShaderProgram::Vertex] = data.vertexShaderCode;
    m_shaderCode[QShaderProgram::TessellationControl] = data.tessellationControlShaderCode;
    m_shaderCode[QShaderProgram::TessellationEvaluation] = data.tessellationEvaluationShaderCode;
    m_shaderCode[QShaderProgram::Geometry] = data.geometryShaderCode;
    m_shaderCode[QShaderProgram::Fragment] = data.fragmentShaderCode;
    m_shaderCode[QShaderProgram::Compute] = data.computeShaderCode;
    m_format = data.format;
    m_dirty = true;
    markDirty(AbstractRenderer::ShadersDirty);
}

void Shader::setShaderCode(QShaderProgram::ShaderType type, const QByteArray &code)
{
    if (code == m_shaderCode[type])
        return;

    m_shaderCode[type] = code;
    m_dirty = true;
    setStatus(QShaderProgram::NotReady);
    markDirty(AbstractRenderer::ShadersDirty);
}

void Shader::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = e.staticCast<QPropertyUpdatedChange>();
        QVariant propertyValue = propertyChange->value();

        if (propertyChange->propertyName() == QByteArrayLiteral("vertexShaderCode"))
            setShaderCode(QShaderProgram::Vertex, propertyValue.toByteArray());
        else if (propertyChange->propertyName() == QByteArrayLiteral("fragmentShaderCode"))
            setShaderCode(QShaderProgram::Fragment, propertyValue.toByteArray());
        else if (propertyChange->propertyName() == QByteArrayLiteral("tessellationControlShaderCode"))
            setShaderCode(QShaderProgram::TessellationControl, propertyValue.toByteArray());
        else if (propertyChange->propertyName() == QByteArrayLiteral("tessellationEvaluationShaderCode"))
            setShaderCode(QShaderProgram::TessellationEvaluation, propertyValue.toByteArray());
        else if (propertyChange->propertyName() == QByteArrayLiteral("geometryShaderCode"))
            setShaderCode(QShaderProgram::Geometry, propertyValue.toByteArray());
        else if (propertyChange->propertyName() == QByteArrayLiteral("computeShaderCode"))
            setShaderCode(QShaderProgram::Compute, propertyValue.toByteArray());
        else if (propertyChange->propertyName() == QByteArrayLiteral("format"))
            setFormat(propertyValue.value<QShaderProgram::Format>());
    }

    BackendNode::sceneChangeEvent(e);
}

void Shader::setFormat(QShaderProgram::Format format)
{
    if (format == m_format)
        return;
    m_format = format;
    m_dirty = true;
    setStatus(QShaderProgram::NotReady);
    markDirty(AbstractRenderer::ShadersDirty);
}

QVector<QByteArray> Shader::shaderCode() const
{
    return m_shaderCode;
}


// To be called from a worker thread
void Shader::submitPendingNotifications()
{
    const  QVector<Qt3DCore::QPropertyUpdatedChangePtr> notifications = std::move(m_pendingNotifications);
    for (const Qt3DCore::QPropertyUpdatedChangePtr &notification : notifications)
        notifyObservers(notification);
}


/*!
   \internal
   Initializes this Shader's state relating to attributes, global block uniforms and
   and named uniform blocks by copying these details from \a other.
*/
void Shader::initializeFromReference(const Shader &other)
{
    setStatus(other.status());
    setLog(other.log());
}

void Shader::setLog(const QString &log)
{
    if (log != m_log) {
        m_log = log;
        Qt3DCore::QPropertyUpdatedChangePtr e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName("log");
        e->setValue(QVariant::fromValue(m_log));
        m_pendingNotifications.push_back(e);
    }
}

void Shader::setStatus(QShaderProgram::Status status)
{
    if (status != m_status) {
        m_status = status;
        Qt3DCore::QPropertyUpdatedChangePtr e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName("status");
        e->setValue(QVariant::fromValue(m_status));
        m_pendingNotifications.push_back(e);
    }
}

ShaderFunctor::ShaderFunctor(AbstractRenderer *renderer, ShaderManager *manager)
    : m_renderer(renderer)
    , m_shaderManager(manager)
{
}

QBackendNode *ShaderFunctor::create(const QNodeCreatedChangeBasePtr &change) const
{
    Shader *backend = m_shaderManager->getOrCreateResource(change->subjectId());
    backend->setRenderer(m_renderer);
    return backend;
}

QBackendNode *ShaderFunctor::get(QNodeId id) const
{
    return m_shaderManager->lookupResource(id);
}

void ShaderFunctor::destroy(QNodeId id) const
{
    m_shaderManager->addShaderIdToCleanup(id);
    // We only add ourselves to the dirty list
    // The actual removal needs to be performed after we have
    // destroyed the associated APIShader in the RenderThread
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
