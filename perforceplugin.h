/***************************************************************************
 *   Copyright 2010  Morten Danielsen Volden                               *
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

#ifndef KDEVPERFORCEPLUGIN_H
#define KDEVPERFORCEPLUGIN_H

#include <vcs/interfaces/icentralizedversioncontrol.h>
#include <vcs/vcsstatusinfo.h>
#include <interfaces/iplugin.h>
#include <outputview/outputjob.h>


#include <QVariantList>
#include <QString>

#include <kaction.h>
#include <KMimeType>
#include <auto_ptr.h>

class QMenu;
class QFileInfo;
class QDir;


namespace KDevelop
{
  class ContextMenuExtension;
  class VcsPluginHelper;
  class DVcsJob;
}


class perforceplugin : public KDevelop::IPlugin, public KDevelop::ICentralizedVersionControl
{
  Q_OBJECT
  Q_INTERFACES(KDevelop::IBasicVersionControl KDevelop::ICentralizedVersionControl)

public:
  perforceplugin(QObject* parent, const QVariantList & = QVariantList()); 
  virtual ~perforceplugin();
  
  //@{
  /** Methods inherited from KDevelop::IBasicVersionControl */
  QString name() const;
  
  KDevelop::VcsImportMetadataWidget* createImportMetadataWidget( QWidget* parent );
  
  bool isVersionControlled(const KUrl& localLocation);
  
  KDevelop::VcsJob* repositoryLocation(const KUrl& localLocation);
  
  KDevelop::VcsJob* add( const KUrl::List& localLocations,
                         RecursionMode recursion = IBasicVersionControl::Recursive );
  KDevelop::VcsJob* remove( const KUrl::List& localLocations );

  KDevelop::VcsJob* copy( const KUrl& localLocationSrc,
                          const KUrl& localLocationDstn );
  KDevelop::VcsJob* move( const KUrl& localLocationSrc,
                          const KUrl& localLocationDst );
  KDevelop::VcsJob* status( const KUrl::List& localLocations,
                            RecursionMode recursion = IBasicVersionControl::Recursive );

  KDevelop::VcsJob* revert( const KUrl::List& localLocations,
                            RecursionMode recursion = IBasicVersionControl::Recursive );

  KDevelop::VcsJob* update( const KUrl::List& localLocations,
                            const KDevelop::VcsRevision& rev = KDevelop::VcsRevision::createSpecialRevision( KDevelop::VcsRevision::Head ),
                            KDevelop::IBasicVersionControl::RecursionMode recursion = KDevelop::IBasicVersionControl::Recursive );

  KDevelop::VcsJob* commit( const QString& message,
                            const KUrl::List& localLocations,
                            KDevelop::IBasicVersionControl::RecursionMode recursion = KDevelop::IBasicVersionControl::Recursive );

  KDevelop::VcsJob* diff( const KUrl& fileOrDirectory,
                          const KDevelop::VcsRevision& srcRevision,
                          const KDevelop::VcsRevision& dstRevision,
                          KDevelop::VcsDiff::Type = KDevelop::VcsDiff::DiffUnified,
                          KDevelop::IBasicVersionControl::RecursionMode recursion = KDevelop::IBasicVersionControl::Recursive );

  KDevelop::VcsJob* log( const KUrl& localLocation,
                         const KDevelop::VcsRevision& rev,
                         unsigned long limit = 0 );

  KDevelop::VcsJob* log( const KUrl& localLocation,
                         const KDevelop::VcsRevision& rev = KDevelop::VcsRevision::createSpecialRevision( KDevelop::VcsRevision::Base ),
                         const KDevelop::VcsRevision& limit= KDevelop::VcsRevision::createSpecialRevision( KDevelop::VcsRevision::Start ) );

  KDevelop::VcsJob* annotate( const KUrl& localLocation,
                              const KDevelop::VcsRevision& rev = KDevelop::VcsRevision::createSpecialRevision( KDevelop::VcsRevision::Head ) );

  KDevelop::VcsJob* resolve( const KUrl::List& localLocations,
                             KDevelop::IBasicVersionControl::RecursionMode recursion );

  KDevelop::VcsJob* createWorkingCopy(const  KDevelop::VcsLocation & sourceRepository, 
                                      const KUrl & destinationDirectory, 
                                      KDevelop::IBasicVersionControl::RecursionMode recursion = IBasicVersionControl::Recursive);

    
  KDevelop::VcsLocationWidget* vcsLocation(QWidget* parent) const;  
  //@}
  
  //@{
  /** Methods inherited from KDevelop::ICentralizedVersionControl  */
    KDevelop::VcsJob* edit( const KUrl& localLocation );

    KDevelop::VcsJob* unedit( const KUrl& localLocation );

    KDevelop::VcsJob* localRevision( const KUrl& localLocation,
                                    KDevelop::VcsRevision::RevisionType );

    KDevelop::VcsJob* import(const QString & commitMessage, 
                             const KUrl & sourceDirectory, 
                             const  KDevelop::VcsLocation & destinationRepository);
  //@}  
  
    KDevelop::ContextMenuExtension contextMenuExtension(KDevelop::Context* context);
  
  
public Q_SLOTS:

  /// invoked by context-menu
  void ctxEdit();
//   void ctxUnedit();
//   void ctxLocalRevision();
//   void ctxImport();

private slots:
   void parseP4StatusOutput(KDevelop::DVcsJob* job);
   void parseP4DiffOutput(KDevelop::DVcsJob* job);
   void parseP4LogOutput(KDevelop::DVcsJob* job);
   void parseP4AnnotateOutput(KDevelop::DVcsJob* job);
   

  
private:
    bool isValidDirectory(const KUrl & dirPath);
    KDevelop::DVcsJob* p4fstatJob(const QFileInfo& curFile,
                            KDevelop::OutputJob::OutputJobVerbosity verbosity = KDevelop::OutputJob::Verbose);

    bool parseP4fstat(const QFileInfo& curFile, 
		      KDevelop::OutputJob::OutputJobVerbosity verbosity = KDevelop::OutputJob::Verbose);

    KDevelop::VcsJob* errorsFound(const QString& error, 
				  KDevelop::OutputJob::OutputJobVerbosity verbosity = KDevelop::OutputJob::Verbose);

    QString getRepositoryName(const QFileInfo& curFile);


    void setEnvironmentForJob(KDevelop::DVcsJob* job, QFileInfo const& fsObject);
    QList<QVariant> getQvariantFromLogOutput(QStringList const& outputLines );
    
  
    std::auto_ptr<KDevelop::VcsPluginHelper> m_common;
    QMenu* m_perforcemenu;
    QString m_perforceConfigName;
    KAction* m_edit_action;



};

#endif // PERFORCEPLUGIN_H
