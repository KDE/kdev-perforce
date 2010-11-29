/***************************************************************************
 *   Copyright 2010  Morten Danielsen Volden                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KDEVPERFORCEPLUGIN_H
#define KDEVPERFORCEPLUGIN_H

#include <vcs/interfaces/icentralizedversioncontrol.h>
#include <vcs/vcsstatusinfo.h>
#include <interfaces/iplugin.h>

#include <QVariantList>

#include <kaction.h>
#include <KMimeType>
#include <auto_ptr.h>

class QMenu;
class QFileInfo;


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

  
private:
    bool pathHasConfigFile(const KUrl & dirPath);
    void setEnvironmentForJob(KDevelop::DVcsJob* job, QFileInfo const& fsObject);
  
    std::auto_ptr<KDevelop::VcsPluginHelper> m_common;
    QMenu* m_perforcemenu;
    QString m_perforceConfigName;
    KAction* m_edit_action;



};

#endif // PERFORCEPLUGIN_H
