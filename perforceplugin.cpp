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
#include <QDateTime>
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

namespace
{
  const QString ACTION_STR("... action ");
  const QString CLIENT_FILE_STR("... clientFile ");
  const QString DEPOT_FILE_STR("... depotFile ");
  const QString LOGENTRY_START("... #");
}

/* Todo:
 *
 * 			Implement History  */

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
    if (!tmp.isEmpty())
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
        if (dir.exists(m_perforceConfigName))
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
        if (!job->output().isEmpty())
            return true;
    }
    return false;
}

QString perforceplugin::getRepositoryName(const QFileInfo& curFile)
{
    QString ret;
    QScopedPointer<DVcsJob> job(p4fstatJob(curFile, KDevelop::OutputJob::Silent));
    if (job->exec() && job->status() == KDevelop::VcsJob::JobSucceeded)
    {
        if (!job->output().isEmpty())
	{
	    QStringList outputLines = job->output().split('\n', QString::SkipEmptyParts);
	    foreach(const QString& line, outputLines)
	    {
		int idx(line.indexOf(DEPOT_FILE_STR));
		if (idx != -1)
		{
		    ret = line.right(line.size() - DEPOT_FILE_STR.size());
		    return ret;
		}
	    }
	}
    }
    
    return ret; 
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

KDevelop::VcsJob* perforceplugin::status(const KUrl::List& localLocations, KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
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

KDevelop::VcsJob* perforceplugin::commit(const QString& message, const KUrl::List& localLocations, KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
{
    if (localLocations.empty() || message.isEmpty())
        return errorsFound(i18n("No files or message specified"));


    QFileInfo curFile(localLocations.front().toLocalFile());

    DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
    setEnvironmentForJob(job, curFile);
    *job << "p4" << "submit" << "-d" << message << localLocations;

    return job;
}

KDevelop::VcsJob* perforceplugin::diff(const KUrl& fileOrDirectory, const KDevelop::VcsRevision& srcRevision, const KDevelop::VcsRevision& dstRevision, KDevelop::VcsDiff::Type , KDevelop::IBasicVersionControl::RecursionMode /*recursion*/)
{
    QFileInfo curFile(fileOrDirectory.toLocalFile());
    QString depotSrcFileName = getRepositoryName(curFile);
    QString depotDstFileName = depotSrcFileName;
    switch(srcRevision.revisionType())
    {
	case VcsRevision::Special:
	    switch(srcRevision.revisionValue().value<VcsRevision::RevisionSpecialType>()) 
	    {
		case VcsRevision::Head:
		    depotSrcFileName.append("#head");
		    break;
		case VcsRevision::Base:
		    depotSrcFileName.append("#have");
		    break;
		case VcsRevision::Previous:
		    {
			kDebug() << "########### Called with a Previous";
			bool *ok(new bool());
			int previous = dstRevision.prettyValue().toInt(ok);
			previous--;
			QString tmp;
			tmp.setNum(previous);
			depotSrcFileName.append("#");
			depotSrcFileName.append(tmp);
		    }
		    break;
		case VcsRevision::Working:
		case VcsRevision::Start:
		case VcsRevision::UserSpecialType:
		    break;
	    }
	    break;
	case VcsRevision::FileNumber:
	case VcsRevision::GlobalNumber:
	    depotSrcFileName.append("#");
	    depotSrcFileName.append(srcRevision.prettyValue());
	    kDebug() << "########### Called with a global number ";
	    break;
	case VcsRevision::Date:
	    kDebug() << "########### Called with a global date";
	    break;
	case VcsRevision::Invalid:
	    kDebug() << "########### Called with a invalid";
	    break;
	case VcsRevision::UserSpecialType:
	    kDebug() << "########### Called with a UserSpecialType";
	    break;
	    
    }

    DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
    setEnvironmentForJob(job, curFile);
    switch(dstRevision.revisionType())
    {
	case VcsRevision::FileNumber:
	case VcsRevision::GlobalNumber:
	    depotDstFileName.append("#");
	    depotDstFileName.append(dstRevision.prettyValue());
	    *job << "p4" << "diff2" << "-u" << depotSrcFileName << depotDstFileName;
	    break;
	case VcsRevision::Special:
	    switch(dstRevision.revisionValue().value<VcsRevision::RevisionSpecialType>()) 
	    {
		case VcsRevision::Working:
		    *job << "p4" << "diff" << "-du" << depotSrcFileName;
		    break;
		case VcsRevision::Start:
		    kDebug() << "########### dstRevision special type is start";
		    break;
		case VcsRevision::UserSpecialType:
		    kDebug() << "########### dstRevision special type is user special type";
		    break;
		    
		default:
		    break;
	    }
	default:
	    break;
    }
    kDebug() << "########### srcRevision Is: " << srcRevision.prettyValue();
    kDebug() << "########### dstRevision Is: " << dstRevision.prettyValue();


    connect(job, SIGNAL(readyForParsing(KDevelop::DVcsJob*)), SLOT(parseP4DiffOutput(KDevelop::DVcsJob*)));
    return job;
}

KDevelop::VcsJob* perforceplugin::log(const KUrl& localLocation, const KDevelop::VcsRevision& /*rev*/, long unsigned int /*limit*/)
{
    QFileInfo curFile(localLocation.toLocalFile());
    DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
    setEnvironmentForJob(job, curFile);
    *job << "p4" << "filelog" << "-l" << localLocation;

    connect(job, SIGNAL(readyForParsing(KDevelop::DVcsJob*)), SLOT(parseP4LogOutput(KDevelop::DVcsJob*)));
    return job;
}

KDevelop::VcsJob* perforceplugin::log(const KUrl& localLocation, const KDevelop::VcsRevision& /*rev*/, const KDevelop::VcsRevision& /*limit*/)
{
    QFileInfo curFile(localLocation.toLocalFile());
    DVcsJob* job = new DVcsJob(curFile.dir(), this, KDevelop::OutputJob::Verbose);
    setEnvironmentForJob(job, curFile);
    *job << "p4" << "filelog" << "-lt" << localLocation;
    
    connect(job, SIGNAL(readyForParsing(KDevelop::DVcsJob*)), SLOT(parseP4LogOutput(KDevelop::DVcsJob*)));
    return job;
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

    if ( !m_edit_action )
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
    //KUrl fileUrl = job->directory().absolutePath();
    QVariantList statuses;
    QList<KUrl> processedFiles;


    VcsStatusInfo status;
    status.setState(VcsStatusInfo::ItemUserState);
    foreach(const QString& line, outputLines)
    {
        int idx(line.indexOf(ACTION_STR));
        if (idx != -1)
        {
            QString curr = line.right(line.size() - ACTION_STR.size());
            kDebug() << "PARSED FROM P4 FSTAT JOB " << curr;

            if (curr == "edit")
            {
                status.setState(VcsStatusInfo::ItemModified);
            }
            else if (curr == "add")
            {
                status.setState(VcsStatusInfo::ItemAdded);
            }
            else
            {
                status.setState(VcsStatusInfo::ItemUserState);
            }
            continue;
        }
        idx = line.indexOf(CLIENT_FILE_STR);
        if (idx != -1)
        {
            KUrl fileUrl = line.right(line.size() - CLIENT_FILE_STR.size());
            kDebug() << "PARSED URL FROM P4 FSTAT JOB " << fileUrl.url();
	    status.setUrl(fileUrl);
	}	
    }
    statuses.append(qVariantFromValue<VcsStatusInfo>(status));
    job->setResults(statuses);
}

void perforceplugin::parseP4LogOutput(KDevelop::DVcsJob* job)
{
    QList<QVariant> commits;
    VcsEvent item;
    QString commitMessage;
    QStringList outputLines = job->output().split('\n', QString::SkipEmptyParts);
    bool foundAChangelist(false);
    /// I'm pretty sure this could be done more elegant.
    foreach(const QString& line, outputLines)
    {
        int idx(line.indexOf(LOGENTRY_START));
        if (idx != -1)
        {
	    if(!foundAChangelist)
	    {
		foundAChangelist = true;
	    }
	    else
	    {
		item.setMessage(commitMessage.trimmed()); 
		commits.append(QVariant::fromValue(item));
		commitMessage.clear();
	    }
	    // expecting the Logentry line to be of the form:
	    //... #5 change 10 edit on 2010/12/06 12:07:31 by mvo@testbed (text)
	    //QString changeNumber(line.section(' ', 3, 3 ));
	    QString localChangeNumber(line.section(' ', 1, 1 ));
	    localChangeNumber.remove(0, 1); // Remove the # from the local revision number
	    
	    QString author(line.section(' ', 9, 9 ));
	    VcsRevision rev;
	    rev.setRevisionValue(localChangeNumber, KDevelop::VcsRevision::FileNumber);
	    item.setRevision(rev);
	    item.setAuthor(author);
	    item.setDate(QDateTime::fromString(line.section(' ', 6, 7 ), "yyyy/MM/dd hh:mm:ss") );
	} 
	else
	{
	    if(foundAChangelist)
		commitMessage += line +'\n';
	}
	    
    }
    item.setMessage(commitMessage); 
    commits.append(QVariant::fromValue(item));
    
    job->setResults(commits);
}


void perforceplugin::parseP4DiffOutput(DVcsJob* job)
{
    VcsDiff diff;
    diff.setDiff(job->output());

    QDir dir(job->directory());

    do
    {
        if (dir.exists(m_perforceConfigName))
        {
            break;
        }
    }
    while (dir.cdUp());

    diff.setBaseDiff(KUrl(dir.absolutePath()));

    job->setResults(qVariantFromValue(diff));
}

KDevelop::VcsJob* perforceplugin::errorsFound(const QString& error, KDevelop::OutputJob::OutputJobVerbosity verbosity)
{
    DVcsJob* j = new DVcsJob(QDir::temp(), this, verbosity);
    *j << "echo" << i18n("error: %1", error) << "-n";
    return j;
}



