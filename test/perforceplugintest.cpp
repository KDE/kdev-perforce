/***************************************************************************
 *   Copyright 2011  Morten Danielsen Volden                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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
const QString perforceConfigFileName(tempDir + "/p4config.txt" );

const QString gitRepo(perforceTestBaseDir + ".git");
const QString gitSrcDir(perforceTestBaseDir + "src/");
const QString gitTest_FileName("testfile");
const QString gitTest_FileName2("foo");
const QString gitTest_FileName3("bar");

/// TODO make sure a valid p4Config.txt has been added to the test
/// We may need to find some way to add a valid client too?



void PerforcePluginTest::init()
{
    KDevelop::AutoTestShell::init();
    m_core = new KDevelop::TestCore();
    m_core->initialize( KDevelop::Core::NoUi );
    m_plugin = new perforceplugin(m_core);
}

void PerforcePluginTest::cleanup()
{
    m_core->cleanup();
    delete m_core;
}

void PerforcePluginTest::testAdd()
{
    kDebug() << "Adding files to the repo";
    
    //we start it after repoInit, so we still have empty git repo
    QFile f(perforceTestBaseDir + gitTest_FileName);
    
    if (f.open(QIODevice::WriteOnly)) {
	QTextStream input(&f);
	input << "HELLO WORLD";
    }
    
    f.close();
    f.setFileName(perforceTestBaseDir + gitTest_FileName2);
    
    if (f.open(QIODevice::WriteOnly)) {
	QTextStream input(&f);
	input << "No, bar()!";
    }
    
    f.close();
    
    //test git-status exitCode (see DVcsJob::setExitCode).
    KDevelop::VcsJob* j = m_plugin->status(KUrl::List(perforceTestBaseDir));
    VERIFYJOB(j);
    
    // /tmp/kdevGit_testdir/ and testfile
    j = m_plugin->add(KUrl::List(perforceTestBaseDir + gitTest_FileName));
    VERIFYJOB(j);
    
    f.setFileName(gitSrcDir + gitTest_FileName3);
    
    if (f.open(QIODevice::WriteOnly)) {
	QTextStream input(&f);
	input << "No, foo()! It's bar()!";
    }
    
    f.close();
    
    //test git-status exitCode again
    j = m_plugin->status(KUrl::List(perforceTestBaseDir));
    VERIFYJOB(j);
    
    //repository path without trailing slash and a file in a parent directory
    // /tmp/repo  and /tmp/repo/src/bar
    j = m_plugin->add(KUrl::List(QStringList(gitSrcDir + gitTest_FileName3)));
    VERIFYJOB(j);
    
    //let's use absolute path, because it's used in ContextMenus
    j = m_plugin->add(KUrl::List(QStringList(perforceTestBaseDir + gitTest_FileName2)));
    VERIFYJOB(j);
    
    //Now let's create several files and try "git add file1 file2 file3"
    f.setFileName(perforceTestBaseDir + "file1");
    
    if (f.open(QIODevice::WriteOnly)) {
	QTextStream input(&f);
	input << "file1";
    }
    
    f.close();
    f.setFileName(perforceTestBaseDir + "file2");
    
    if (f.open(QIODevice::WriteOnly)) {
	QTextStream input(&f);
	input << "file2";
    }
    f.close();
    
    KUrl::List multipleFiles;
    multipleFiles << (perforceTestBaseDir + "file1");
    multipleFiles << (perforceTestBaseDir + "file2");
    j = m_plugin->add(multipleFiles);
    VERIFYJOB(j);
}

QTEST_KDEMAIN(PerforcePluginTest, GUI)
