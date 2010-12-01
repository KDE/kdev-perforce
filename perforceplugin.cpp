/***************************************************************************
 *   Copyright 2010  Morten Danielsen Volden                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "perforceplugin.h"

#include <iostream>

#include <KPluginFactory>
#include <KPluginLoader>
#include <KLocalizedString>
#include <KAboutData>
#include <KActionCollection>
#include <QFileInfo>
#include <QDir>
#include <QProcessEnvironment>
#include <QMenu>
#include <kmessagebox.h>
#include <vcs/vcsjob.h>
#include <vcs/vcsrevision.h>
#include <vcs/vcsevent.h>
#include <vcs/dvcs/dvcsjob.h>
#include <vcs/vcsannotation.h>
#include <vcs/widgets/standardvcslocationwidget.h>

#include <interfaces/context.h>
#include <interfaces/contextmenuextension.h>
#include <interfaces/icore.h>
#include <interfaces/iruncontroller.h>



#include <vcs/vcspluginhelper.h>

using namespace KDevelop;


/* Todo: 
 * 			
 * 			Implement diff to make commit work */

K_PLUGIN_FACTORY (KdevPerforceFactory, registerPlugin<perforceplugin>();)
K_EXPORT_PLUGIN (KdevPerforceFactory (KAboutData ("kdevperforce","kdevperforce", ki18n ("Support for Perforce Version Control System"), "0.1", ki18n ("Support for Perforce Version Control System"), KAboutData::License_GPL)))

perforceplugin::perforceplugin(QObject* parent, const QVariantList& ): 
  KDevelop::IPlugin(KdevPerforceFactory::componentData(), parent )
  , m_common(new KDevelop::VcsPluginHelper(this, this))
  , m_perforcemenu( 0 )
  , m_perforceConfigName("p4config.txt")
  , m_edit_action( 0 )
{
    KDEV_USE_EXTENSION_INTERFACE( KDevelop::IBasicVersionControl )
    KDEV_USE_EXTENSION_INTERFACE( KDevelop::ICentralizedVersionControl )
    
    QProcessEnvironment currentEviron(QProcessEnvironment::systemEnvironment());
    // We will default search for p4config.txt - However if something else is used, search for that 
    QString tmp(currentEviron.value("P4CONFIG"));
    if(!tmp.isEmpty())
    {
	m_perforceConfigName = tmp;
    }
}

perforceplugin::~perforceplugin()
{
}

QString perforceplugin::name() const
{
  return i18n("Perforce");
}

KDevelop::VcsImportMetadataWidget* perforceplugin::createImportMetadataWidget(QWidget* /*parent*/)
{
  return 0;
}

bool perforceplugin::isValidDirectory(const KUrl & dirPath)
{
    const QFileInfo finfo(dirPath.toLocalFile());
    QDir dir = finfo.isDir() ? QDir(dirPath.toLocalFile()) : finfo.absoluteDir();

    do 
    {
      if(dir.exists(m_perforceConfigName))
      {
          return true;
      }
    } 
    while (dir.cdUp()); 
    return false;
}

bool perforceplugin::isVersionControlled(const KUrl& localLocation)
{
    QFileInfo fsObject(localLocation.toLocalFile());
    if (fsObject.isDir()) 
    {
        return isValidDirectory(localLocation);
    }
    return parseP4fstat(fsObject, KDevelop::OutputJob::Silent);
}

DVcsJob* perforceplugin::p4fstatJob(const QFileInfo& curFile, OutputJob::OutputJobVerbosity verbosity)
{
    DVcsJob* job = new DVcsJob(curFile.absolutePath(), this, verbosity);
    setEnvironmentForJob(job, curFile);
    *job << "p4" << "fstat" << curFile.fileName();
    return job;
}

bool perforceplugin::parseP4fstat(const QFileInfo& curFile, OutputJob::OutputJobVerbosity verbosity)
{
    QScopedPointer<DVcsJob> job(p4fstatJob(curFile, verbosity));
    if (job->exec() && job->status() == KDevelop::VcsJob::JobSucceeded)
    {
      kDebug() << "Perforce returned: " << job->output();
      if(!job->output().isEmpty())
	return true;
    }
    return false;
}


KDevelop::VcsJob* perforceplugin::repositoryLocation(const KUrl& /*localLocation*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::add(const KUrl::List& localLocations, KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
{
  QFileInfo curFile(localLocations.front().toLocalFile());
  DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
  setEnvironmentForJob(job, curFile);
  *job << "p4" << "add" << localLocations;
  
  return job;
}

KDevelop::VcsJob* perforceplugin::remove(const KUrl::List& /*localLocations*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::copy(const KUrl& /*localLocationSrc*/, const KUrl& /*localLocationDstn*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::move(const KUrl& /*localLocationSrc*/, const KUrl& /*localLocationDst*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::status(const KUrl::List& localLocations, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
    if (localLocations.count() != 1) 
    {
        KMessageBox::error(0, i18n("Please select only one item for this operation"));
        return 0; 
    }

    QFileInfo curFile(localLocations.front().toLocalFile());
   
    DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
    setEnvironmentForJob(job, curFile);
    *job << "p4" << "fstat" << curFile.fileName();
    connect(job, SIGNAL(readyForParsing(KDevelop::DVcsJob*)), SLOT(parseP4StatusOutput(KDevelop::DVcsJob*)));

    return job;
}

KDevelop::VcsJob* perforceplugin::revert(const KUrl::List& localLocations, KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
{
    if (localLocations.count() != 1) 
    {
        KMessageBox::error(0, i18n("Please select only one item for this operation"));
        return 0; 
    }

    QFileInfo curFile(localLocations.front().toLocalFile());
   
    DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
    setEnvironmentForJob(job, curFile);
    *job << "p4" << "revert" << curFile.fileName();

    return job;
 
}

KDevelop::VcsJob* perforceplugin::update(const KUrl::List& localLocations, const KDevelop::VcsRevision& /*rev*/, KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
{
    QFileInfo curFile(localLocations.front().toLocalFile());
   
    DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
    setEnvironmentForJob(job, curFile);
    *job << "p4" << "-p" << "127.0.0.1:1666" << "info";

    return job;
}

KDevelop::VcsJob* perforceplugin::commit(const QString& message, const KUrl::List& localLocations, KDevelop::IBasicVersionControl::RecursionMode recursion)
{
  QFileInfo curFile(localLocations.front().toLocalFile());
   
  DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
  setEnvironmentForJob(job, curFile);
  *job << "p4" << "submit" << "-d" << message << localLocations;
  
  return job;
}

KDevelop::VcsJob* perforceplugin::diff(const KUrl& /*fileOrDirectory*/, const KDevelop::VcsRevision& /*srcRevision*/, const KDevelop::VcsRevision& /*dstRevision*/, KDevelop::VcsDiff::Type , KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
{
    return 0;
}

KDevelop::VcsJob* perforceplugin::log(const KUrl& /*localLocation*/, const KDevelop::VcsRevision& /*rev*/, long unsigned int /*limit*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::log(const KUrl& /*localLocation*/, const KDevelop::VcsRevision& /*rev*/, const KDevelop::VcsRevision& /*limit*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::annotate(const KUrl& /*localLocation*/, const KDevelop::VcsRevision& /*rev*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::resolve(const KUrl::List& /*localLocations*/, KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::createWorkingCopy(const KDevelop::VcsLocation& /*sourceRepository*/, const KUrl& /*destinationDirectory*/, KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
{
  return 0;
}

KDevelop::VcsLocationWidget* perforceplugin::vcsLocation(QWidget* parent) const
{
    return new StandardVcsLocationWidget(parent);
}


KDevelop::VcsJob* perforceplugin::edit(const KUrl& localLocation)
{
    QFileInfo curFile(localLocation.toLocalFile());
   
    DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
    setEnvironmentForJob(job, curFile);
    *job << "p4" << "edit" << curFile.fileName();

    return job;
}

KDevelop::VcsJob* perforceplugin::unedit(const KUrl& /*localLocation*/)
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::localRevision(const KUrl& /*localLocation*/, KDevelop::VcsRevision::RevisionType )
{
  return 0;
}

KDevelop::VcsJob* perforceplugin::import(const QString& /*commitMessage*/, const KUrl& /*sourceDirectory*/, const KDevelop::VcsLocation& /*destinationRepository*/)
{
  return 0;
}

KDevelop::ContextMenuExtension perforceplugin::contextMenuExtension(KDevelop::Context* context)
{
    m_common->setupFromContext(context);

    const KUrl::List & ctxUrlList  = m_common->contextUrlList();

    bool hasVersionControlledEntries = false;
    foreach(KUrl const& url, ctxUrlList) 
    {
        if (isValidDirectory(url)) 
        {
            hasVersionControlledEntries = true;
            break;
        }
    }

    //kDebug() << "version controlled?" << hasVersionControlledEntries;

    if (!hasVersionControlledEntries)
        return IPlugin::contextMenuExtension(context);

    QMenu * perforceMenu = m_common->commonActions();
    perforceMenu->addSeparator();    
    
    perforceMenu->addSeparator();
    
    if( !m_edit_action )
    {
        m_edit_action = new KAction(i18n("Edit"), this);
        connect(m_edit_action, SIGNAL(triggered()), this, SLOT(ctxEdit()));
    }
    perforceMenu->addAction(m_edit_action);

    ContextMenuExtension menuExt;
    menuExt.addAction(ContextMenuExtension::VcsGroup, perforceMenu->menuAction());

    return menuExt;
}

void perforceplugin::ctxEdit()
{
    KUrl::List const & ctxUrlList = m_common->contextUrlList();
    if (ctxUrlList.count() != 1) 
    {
        KMessageBox::error(0, i18n("Please select only one item for this operation"));
        return;
    }
    KUrl source = ctxUrlList.first();
    KDevelop::ICore::self()->runController()->registerJob(edit(source));
}

void perforceplugin::setEnvironmentForJob(DVcsJob* job, const QFileInfo& curFile)
{
    KProcess* jobproc = job->process();
    QStringList beforeEnv = jobproc->environment();
    //kDebug() << "Before setting the environment : " << beforeEnv;
    jobproc->setEnv("P4CONFIG", m_perforceConfigName);
    jobproc->setEnv("PWD", curFile.absolutePath());
    QStringList afterEnv = jobproc->environment();  
    //kDebug() << "After setting the environment : " << afterEnv;   
}

void perforceplugin::parseP4StatusOutput(DVcsJob* job)
{
    QStringList outputLines = job->output().split('\n', QString::SkipEmptyParts);
    KUrl fileUrl = job->directory().absolutePath();
    QVariantList statuses;
    QList<KUrl> processedFiles;
    
    
    foreach(const QString& line, outputLines) 
    {
	int idx(line.indexOf("no such file(s)"));
	if(idx != -1)
	{
	    QString curr = line.left(line.size()-idx-15);
	    fileUrl.addPath(curr);
            VcsStatusInfo status;
            status.setUrl(fileUrl);
            status.setState(VcsStatusInfo::ItemUnknown);
            
            statuses.append(qVariantFromValue<VcsStatusInfo>(status));
	}
        //kDebug() << "Checking perforce status for " << line << curr << messageToState(state);
    }
    job->setResults(statuses);
}




