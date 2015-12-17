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

//#include <qtest_kde.h>

#include <tests/autotestshell.h>
#include <tests/testcore.h>

#include <vcs/dvcs/dvcsjob.h>
#include <vcs/vcsjob.h>
#include <vcs/vcsannotation.h>

#include <perforceplugin.h>

#define VERIFYJOB(j) \
    QVERIFY(j); QVERIFY(j->exec()); QVERIFY((j)->status() == KDevelop::VcsJob::JobSucceeded)

const QString tempDir = QDir::tempPath();
const QString perforceTestBaseDirNoSlash(tempDir + "/kdevPerforce_testdir");
const QString perforceTestBaseDir(tempDir + "/kdevPerforce_testdir/");
const QString perforceTestBaseDir2(tempDir + "/kdevPerforce_testdir2/");
const QString perforceConfigFileName("p4config.txt");

const QString perforceSrcDir(perforceTestBaseDir + "src/");
const QString perforceTest_FileName("testfile");
const QString perforceTest_FileName2("foo");
const QString perforceTest_FileName3("bar");

void PerforcePluginTest::init()
{
    KDevelop::AutoTestShell::init();
    m_core = new KDevelop::TestCore();
    m_core->initialize(KDevelop::Core::NoUi);
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

    //Put a file here because the annotate and update function will check for that
    QFile g(perforceTestBaseDir + perforceTest_FileName);
    if (g.open(QIODevice::WriteOnly)) {
        QTextStream input(&g);
        input << "HELLO WORLD";
    }
    g.close();


    tmpdir.mkdir(perforceSrcDir);
    tmpdir.mkdir(perforceTestBaseDir2);
}


void PerforcePluginTest::removeTempDirsIfAny()
{
    QDir dir(perforceTestBaseDir);
    if (dir.exists() && !dir.removeRecursively())
        qDebug() << "QDir::removeRecursively(" << perforceTestBaseDir << ") returned false";

    QDir dir2(perforceTestBaseDir);
    if (dir2.exists() && !dir2.removeRecursively())
        qDebug() << "QDir::removeRecursively(" << perforceTestBaseDir2 << ") returned false";
}


void PerforcePluginTest::cleanup()
{
    m_core->cleanup();
    delete m_core;
    removeTempDirsIfAny();
}

void PerforcePluginTest::testAdd()
{
    KDevelop::VcsJob* j = m_plugin->add(QList<QUrl>({ QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName) } ));
    VERIFYJOB(j);
}

void PerforcePluginTest::testEdit()
{
    KDevelop::VcsJob* j = m_plugin->edit(QList<QUrl>( { QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName) } ));
    VERIFYJOB(j);
}

void PerforcePluginTest::testEditMultipleFiles()
{
    QList<QUrl> filesForEdit;
    filesForEdit.push_back(QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName));
    filesForEdit.push_back(QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName2));
    filesForEdit.push_back(QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName3));
    KDevelop::VcsJob* j = m_plugin->edit(filesForEdit);
    VERIFYJOB(j);
}


void PerforcePluginTest::testStatus()
{
    KDevelop::VcsJob* j = m_plugin->status(QList<QUrl>( { QUrl::fromLocalFile(perforceTestBaseDirNoSlash) } ));
    VERIFYJOB(j);
}

void PerforcePluginTest::testAnnotate()
{
    KDevelop::VcsJob* j = m_plugin->annotate(QUrl( QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName) ));
    VERIFYJOB(j);
}

void PerforcePluginTest::testHistory()
{
    KDevelop::VcsJob* j = m_plugin->log(QUrl( QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName) ));
    VERIFYJOB(j);
}

void PerforcePluginTest::testRevert()
{
    KDevelop::VcsJob* j = m_plugin->revert(QList<QUrl>( { QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName) } ));
    VERIFYJOB(j);
}

void PerforcePluginTest::testUpdateFile()
{
    KDevelop::VcsJob* j = m_plugin->update(QList<QUrl>( { QUrl::fromLocalFile(perforceTestBaseDir + perforceTest_FileName) } ));
    VERIFYJOB(j);
}

void PerforcePluginTest::testUpdateDir()
{
    KDevelop::VcsJob* j = m_plugin->update(QList<QUrl>( { QUrl::fromLocalFile(perforceTestBaseDirNoSlash) } ));
    VERIFYJOB(j);
}

void PerforcePluginTest::testCommit()
{
    QString commitMsg("this is the commit message");
    KDevelop::VcsJob* j = m_plugin->commit(commitMsg, QList<QUrl>( { QUrl::fromLocalFile(perforceTestBaseDirNoSlash) }  ));
    VERIFYJOB(j);
}


QTEST_MAIN(PerforcePluginTest)
