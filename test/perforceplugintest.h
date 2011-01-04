/***************************************************************************
 *   This file was inspired by KDevelop's git plugin                       *
 *   Copyright 2008 Evgeniy Ivanov <powerfox@kde.ru>                       *
 *                                                                         *
 *   Adapted for Perforce                                                  *
 *   Copyright 2011  Morten Danielsen Volden <mvolden2@gmail.com>          *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PERFORCEPLUGIN_TEST_H
#define PERFORCEPLUGIN_TEST_H

#include <QtCore/QObject>

class perforceplugin;

namespace KDevelop
{
    class TestCore;
}

class PerforcePluginTest : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void testAdd();
private:
    perforceplugin* m_plugin;
    KDevelop::TestCore* m_core;
};

#endif
