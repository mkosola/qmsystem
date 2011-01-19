/*!
 * @file qmthermal.cpp
 * @brief QmThermal

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

   This file is part of SystemSW QtAPI.

   SystemSW QtAPI is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   SystemSW QtAPI is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with SystemSW QtAPI.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */
#include "qmthermal.h"
#include "qmthermal_p.h"

namespace MeeGo {

QmThermal::QmThermal(QObject *parent)
             : QObject(parent) {
    MEEGO_INITIALIZE(QmThermal)

    connect(priv, SIGNAL(thermalChanged(MeeGo::QmThermal::ThermalState)),
            this, SIGNAL(thermalChanged(MeeGo::QmThermal::ThermalState)));
}

QmThermal::~QmThermal(){
    MEEGO_PRIVATE(QmThermal)

    disconnect(priv, SIGNAL(thermalChanged(MeeGo::QmThermal::ThermalState)),
               this, SIGNAL(thermalChanged(MeeGo::QmThermal::ThermalState)));

    MEEGO_UNINITIALIZE(QmThermal);
}

void QmThermal::connectNotify(const char *signal) {
    MEEGO_PRIVATE(QmThermal)

    /* QObject::connect() needs to be thread-safe */
    QMutexLocker locker(&priv->connectMutex);

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(thermalChanged(MeeGo::QmThermal::ThermalState))))) {
        if (0 == priv->connectCount[SIGNAL_THERMAL_STATE]) {
            QDBusConnection::systemBus().connect(SYS_THERMALMANAGER_SERVICE,
                                                 SYS_THERMALMANAGER_PATH,
                                                 SYS_THERMALMANAGER_INTERFACE,
                                                 SYS_THERMALMANAGER_STATE_SIG,
                                                 priv,
                                                 SLOT(thermalStateChanged(const QString&)));
        }
        priv->connectCount[SIGNAL_THERMAL_STATE]++;
    }
}
void QmThermal::disconnectNotify(const char *signal) {
    MEEGO_PRIVATE(QmThermal)

    /* QObject::disconnect() needs to be thread-safe */
    QMutexLocker locker(&priv->connectMutex);

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(thermalChanged(MeeGo::QmThermal::ThermalState))))) {
        priv->connectCount[SIGNAL_THERMAL_STATE]--;

        if (0 == priv->connectCount[SIGNAL_THERMAL_STATE]) {
            QDBusConnection::systemBus().disconnect(SYS_THERMALMANAGER_SERVICE,
                                                    SYS_THERMALMANAGER_PATH,
                                                    SYS_THERMALMANAGER_INTERFACE,
                                                    SYS_THERMALMANAGER_STATE_SIG,
                                                    priv,
                                                    SLOT(thermalStateChanged(const QString&)));
        }
    }
}

QmThermal::ThermalState QmThermal::get() const {
    MEEGO_PRIVATE_CONST(QmThermal)
    QString state;
    QList<QVariant> resp;

    resp = priv->If->get(SYS_THERMALMANAGER_STATE_GET);

    if (resp.isEmpty()){
        qWarning("Unable to get the thermal state");
        return Error;
    }

    state = resp[0].toString();
    return QmThermalPrivate::stringToState(state);
}

} // MeeGo namespace
