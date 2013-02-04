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

#include "perforceplugintest.h"

#include <QtTest/QtTest>

#include <qtest_kde.h>

#include <tests/autotestshell.h>
#include <tests/testcore.h>

#include <KUrl>
#include <KDebug>

#include <kio/netaccess.h>

#include <vcs/dvcs/dvcsjob.h>
#include <vcs/vcsjob.h>
#include <vcs/vcsannotation.h>

#include <perforceplugin.h>

#define VERIFYJOB(j) \
QVERIFY(j); QVERIFY(j->exec()); QVERIFY((j)->status() == KDevelop::VcsJob::JobSucceeded)

const QString tempDir = QDir::tempPath();
const QString perforceTestBaseDir(tempDir + "/kdevPerforce_testdir/");
const QString perforceTestBaseDir2(tempDir + "/kdevPerforce_testdir2/");
const QString perforceConfigFileName( "p4config.txt" );

const QString perforceSrcDir(perforceTestBaseDir + "src/");
const QString perforceTest_FileName("testfile");
const QString perforceTest_FileName2("foo");
const QString perforceTest_FileName3("bar");

void PerforcePluginTest::init()
{
    KDevelop::AutoTestShell::init();
    m_core = new KDevelop::TestCore();
    m_core->initialize( KDevelop::Core::NoUi );
    m_plugin = new PerforcePlugin(m_core);
	/// During test we are setting the executable the plugin uses to our own stub
	m_plugin->m_perforceExecutable = P4_CLIENT_STUB_EXE;
    removeTempDirsIfAny();
    createNewTempDirs();
}

void PerforcePluginTest::createNewTempDirs()
{
     // Now create the basic directory structure
    QDir tmpdir(tempDir);
    tmpdir.mkdir(perforceTestBaseDir);
    //we start it after repoInit, so we still have empty repo
    QFile f(perforceTestBaseDir + perforceConfigFileName);
    
    if (f.open(QIODevice::WriteOnly)) {
	QTextStream input(&f);
	input << "P4PORT=127.0.0.1:1666\n";
	input << "P4USER=mvo\n";
	input << "P4CLIENT=testbed\n";
    }
    
    f.close();

    tmpdir.mkdir(perforceSrcDir);
    tmpdir.mkdir(perforceTestBaseDir2);
}


void PerforcePluginTest::removeTempDirsIfAny()
{
    if (QFileInfo(perforceTestBaseDir).exists())
        if (!KIO::NetAccess::del(KUrl(perforceTestBaseDir), 0))
            qDebug() << "KIO::NetAccess::del(" << perforceTestBaseDir << ") returned false";

    if (QFileInfo(perforceTestBaseDir2).exists())
        if (!KIO::NetAccess::del(KUrl(perforceTestBaseDir2), 0))
            qDebug() << "KIO::NetAccess::del(" << perforceTestBaseDir2 << ") returned false";
}


void PerforcePluginTest::cleanup()
{
    m_core->cleanup();
    delete m_core;
    removeTempDirsIfAny();
}

void PerforcePluginTest::testAdd()
{
    KDevelop::VcsJob* j = m_plugin->add(KUrl::List(perforceTestBaseDir));
    VERIFYJOB(j);
}

void PerforcePluginTest::testEdit()
{
    KDevelop::VcsJob* j = m_plugin->edit(KUrl(perforceTestBaseDir));
    VERIFYJOB(j);
}

void PerforcePluginTest::testStatus()
{
    KDevelop::VcsJob* j = m_plugin->status(KUrl::List(perforceTestBaseDir));
    VERIFYJOB(j);
}


QTEST_KDEMAIN(PerforcePluginTest, GUI)
