/*!
 * @file qmlocks_p.h
 * @brief Contains QmLocksPrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

   @scope Private

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

#ifndef QMLOCKS_P_H
#define QMLOCKS_P_H

#include "qmlocks.h"

#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusReply>
#include <QMutex>

#if HAVE_MCE
    #include "mce/dbus-names.h"
    #include "mce/mode-names.h"
#endif

#include "qmipcinterface_p.h"

// The DBus system service provided by devicelock
#define DEVLOCK_SERVICE "com.nokia.devicelock"
// The interface that the devicelock uses
#define DEVLOCK_INTERFACE "com.nokia.devicelock"
// The DBus path of the service
#define DEVLOCK_PATH "/request"
// Method used to determine the state of the devicelock
#define DEVLOCK_GET "getState"
// Method used to set the state of the devicelock
#define DEVLOCK_SET "setState"
// A DBus signal used to notify that the state of hte lock has changed
#define DEVLOCK_SIGNAL "stateChanged"

#define DEVLOCK_LOCK_TYPE_DEVICE 1
#define DEVLOCK_LOCK_STATE_UNLOCKED 0
#define DEVLOCK_LOCK_STATE_LOCKED 1
#define DEVLOCK_LOCK_STATE_UNDEFINED 8

#define SIGNAL_LOCK_STATE 0

namespace MeeGo
{
    class QmLocksPrivate : public QObject
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmLocks)

    public:
        QmLocksPrivate() :
            mceRequestIf(0),
            devlockIf(0) {
            #if HAVE_MCE
                mceRequestIf = new QmIPCInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF);
            #endif

            devlockIf = new QmIPCInterface(DEVLOCK_SERVICE, DEVLOCK_PATH, DEVLOCK_SERVICE);

            connectCount[SIGNAL_LOCK_STATE] = 0;
        }

        ~QmLocksPrivate() {
            if (mceRequestIf) {
                delete mceRequestIf, mceRequestIf = 0;
            }
            if (devlockIf) {
                delete devlockIf, devlockIf = 0;
            }
        }

        static QmLocks::State stringToState(const QString &state) {
            #if HAVE_MCE
                if (state == MCE_TK_LOCKED) {
                    return QmLocks::Locked;
                } else if (state == MCE_TK_UNLOCKED) {
                    return QmLocks::Unlocked;
                }
            #else
                Q_UNUSED(state);
            #endif
            return QmLocks::Unknown;
        }

        static QString stateToString(QmLocks::Lock what, QmLocks::State state) {
            #if HAVE_MCE
                if (what == QmLocks::TouchAndKeyboard) {
                    if (state == QmLocks::Locked) {
                        return MCE_TK_LOCKED;
                    } else if (state == QmLocks::Unlocked) {
                        return MCE_TK_UNLOCKED;
                    }
                }
            #else
                Q_UNUSED(what);
                Q_UNUSED(state);
            #endif
            return "";
        }

        static QmLocks::State stateToState(int state) {
            switch (state) {
            case DEVLOCK_LOCK_STATE_UNLOCKED:
                return QmLocks::Unlocked;
            case DEVLOCK_LOCK_STATE_LOCKED:
                return QmLocks::Locked;
            default:
                return QmLocks::Unknown;
            }
        }

        static int stateToState(QmLocks::State state) {
            switch (state) {
            case QmLocks::Unlocked:
                return DEVLOCK_LOCK_STATE_UNLOCKED;
            case QmLocks::Locked:
                return DEVLOCK_LOCK_STATE_LOCKED;
            default:
                return DEVLOCK_LOCK_STATE_UNDEFINED;
            }
        }

        QmLocks::State getState(QmLocks::Lock what, bool async) {
            QmLocks::State state = QmLocks::Unknown;

            if (what == QmLocks::Device) {
                if (async) {
                    QDBusPendingCall pcall = devlockIf->asyncCall(DEVLOCK_GET, DEVLOCK_LOCK_TYPE_DEVICE);
                    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
                    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                                     this, SLOT(didReceiveDeviceLockState(QDBusPendingCallWatcher*)));
                } else {
                    QDBusReply<int> reply = devlockIf->call(DEVLOCK_GET, DEVLOCK_LOCK_TYPE_DEVICE);
                    if (reply.isValid()) {
                        state = QmLocksPrivate::stateToState(reply.value());
                    }
                }
            } else if (what == QmLocks::TouchAndKeyboard) {
                #if HAVE_MCE
                    if (async) {
                        QDBusPendingCall pcall = mceRequestIf->asyncCall(MCE_TKLOCK_MODE_GET);
                        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
                        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                                         this, SLOT(didReceiveTkLockState(QDBusPendingCallWatcher*)));
                    } else {
                        QDBusReply<QString> reply = mceRequestIf->call(MCE_TKLOCK_MODE_GET);
                        if (reply.isValid()) {
                            state = QmLocksPrivate::stringToState(reply.value());
                        }
                    }
                #endif /* HAVE_MCE */
            }
            return state;
        }

        bool setState(QmLocks::Lock what, QmLocks::State how) {
            bool success = false;
            if (what == QmLocks::Device) {
                devlockIf->callAsynchronously(DEVLOCK_SET, DEVLOCK_LOCK_TYPE_DEVICE, stateToState(how));
                success = true;
            } else if (what == QmLocks::TouchAndKeyboard) {
                #if HAVE_MCE
                    mceRequestIf->callAsynchronously(MCE_TKLOCK_MODE_CHANGE_REQ, QmLocksPrivate::stateToString(what, how));
                    success = true;
                #else
                    Q_UNUSED(what);
                #endif /* HAVE_MCE */
            }
            return success;
        }

        QMutex connectMutex;
        size_t connectCount[1];
        QmIPCInterface *mceRequestIf;
        QmIPCInterface *devlockIf;

    Q_SIGNALS:
        void stateChanged(MeeGo::QmLocks::Lock what, MeeGo::QmLocks::State how);

    private Q_SLOTS:

        void didReceiveDeviceLockState(QDBusPendingCallWatcher *call) {
            QDBusPendingReply<int> reply = *call;
            if (reply.isError()) {
                return;
            }
            int state = reply.argumentAt<0>();
            emit stateChanged(QmLocks::Device, stateToState(state));
            call->deleteLater();
        }

        void didReceiveTkLockState(QDBusPendingCallWatcher *call) {
            #if HAVE_MCE
                QDBusPendingReply<QString> reply = *call;
                if (reply.isError()) {
                    return;
                }
                QString state = reply.argumentAt<0>();
                emit stateChanged(QmLocks::TouchAndKeyboard, stringToState(state));
            #endif
            call->deleteLater();
        }

        void deviceStateChanged(int device, int state) {
            if (device == DEVLOCK_LOCK_TYPE_DEVICE) {
                emit stateChanged(QmLocks::Device, stateToState(state));
            }
        }

        void touchAndKeyboardStateChanged(const QString& state) {
            emit stateChanged(QmLocks::TouchAndKeyboard, stringToState(state));
        }
    };
}
#endif // QMLOCKS_P_H
