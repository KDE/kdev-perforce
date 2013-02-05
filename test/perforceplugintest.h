/***************************************************************************
 *   This file was inspired by KDevelop's git plugin                       *
 *   Copyright 2008 Evgeniy Ivanov <powerfox@kde.ru>                       *
 *                                                                         *
 *   Adapted for Perforce                                                  *
 *   Copyright 2011  Morten Danielsen Volden <mvolden2@gmail.com>          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef PERFORCEPLUGIN_TEST_H
#define PERFORCEPLUGIN_TEST_H

#include <QtCore/QObject>

class PerforcePlugin;

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
    void testEdit();
    void testStatus();
    void testAnnotate();
    void testHistory();
    void testRevert();
    void testUpdateFile();
    void testUpdateDir();
    void testCommit();
private:
    PerforcePlugin* m_plugin;
    KDevelop::TestCore* m_core;
    void removeTempDirsIfAny();
    void createNewTempDirs();
};

#endif
