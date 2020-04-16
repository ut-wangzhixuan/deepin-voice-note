#include "dbvisitor.h"
#include "globaldef.h"
#include "db/vnotefolderoper.h"
#include "db/vnoteitemoper.h"
#include "db/vnotesaferoper.h"
#include "common/datatypedef.h"
#include "common/metadataparser.h"
#include "common/vnotedatamanager.h"
#include "common/vnoteforlder.h"
#include "common/vnoteitem.h"

#include <QVariant>

#include <DLog>

DbVisitor::DbVisitor(QSqlDatabase& db, void* result)
    :m_sqlQuery(new QSqlQuery(db))
{
    results.ptr = result;
}

DbVisitor::~DbVisitor()
{

}

QSqlQuery *DbVisitor::sqlQuery()
{
    return m_sqlQuery.get();
}

FolderQryDbVisitor::FolderQryDbVisitor(QSqlDatabase& db, void *result)
    :DbVisitor (db, result)
{

}

bool FolderQryDbVisitor::visitorData()
{
    bool isOK = false;

    if (nullptr != results.folders) {
        isOK = true;

        while(m_sqlQuery->next()) {
            VNoteFolder* folder = new VNoteFolder();

            folder->id          = m_sqlQuery->value(VNoteFolderOper::folder_id).toInt();
            folder->category    = m_sqlQuery->value(VNoteFolderOper::category_id).toInt();
            folder->name        = m_sqlQuery->value(VNoteFolderOper::folder_name).toString();
            folder->defaultIcon = m_sqlQuery->value(VNoteFolderOper::default_icon).toInt();
            folder->iconPath    = m_sqlQuery->value(VNoteFolderOper::icon_path).toString();
            folder->folder_state= m_sqlQuery->value(VNoteFolderOper::folder_state).toInt();

            folder->maxNoteIdRef()= m_sqlQuery->value(VNoteFolderOper::max_noteid).toInt();

            folder->createTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteFolderOper::create_time).toString(),VNOTE_TIME_FMT);
            folder->modifyTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteFolderOper::modify_time).toString(),VNOTE_TIME_FMT);
            folder->deleteTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteFolderOper::delete_time).toString(),VNOTE_TIME_FMT);

            //************Expand fileds begin**********
            //TODO:
            //    Add the expand fileds parse code here

            //************Expand fileds end************

            //Get default icon image
            folder->UI.icon = VNoteDataManager::instance()->getDefaultIcon(folder->defaultIcon, IconsType::DefaultIcon);
            folder->UI.grayIcon = VNoteDataManager::instance()->getDefaultIcon(folder->defaultIcon, IconsType::DefaultGrayIcon);


            results.folders->folders.insert(folder->id, folder);
        }
    }

    return isOK;
}

NoteQryDbVisitor::NoteQryDbVisitor(QSqlDatabase& db, void *result)
    :DbVisitor (db, result)
{

}

bool NoteQryDbVisitor::visitorData()
{
    bool isOK = false;

    if (nullptr != results.notes) {
        isOK = true;

        MetaDataParser metaParser;

        while(m_sqlQuery->next()) {
            VNoteItem* note = new VNoteItem();

            note->noteId      = m_sqlQuery->value(VNoteItemOper::note_id).toInt();
            note->folderId    = m_sqlQuery->value(VNoteItemOper::folder_id).toInt();
            note->noteType    = m_sqlQuery->value(VNoteItemOper::note_type).toInt();
            note->noteTitle   = m_sqlQuery->value(VNoteItemOper::note_title).toString();

            //Parse meta data
            QVariant metaData  = m_sqlQuery->value(VNoteItemOper::meta_data);
            note->setMetadata(metaData);
            metaParser.parse(metaData, note);

            note->noteState   = m_sqlQuery->value(VNoteItemOper::note_state).toInt();

            note->createTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteItemOper::create_time).toString(),VNOTE_TIME_FMT);
            note->modifyTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteItemOper::modify_time).toString(),VNOTE_TIME_FMT);
            note->deleteTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteItemOper::modify_time).toString(),VNOTE_TIME_FMT);

            //************Expand fileds begin**********
            //TODO:
            //    Add the expand fileds parse code here

            //************Expand fileds end************

            VNOTE_ALL_NOTES_DATA_MAP::iterator it =
                    results.notes->notes.find(note->folderId);
#ifdef QT_QML_DEBUG
            qInfo() << "" << (*note);
#endif
            //TODO
            //    If find the folder add note to it, or need create
            //folder items map first;
            if (it != results.notes->notes.end()) {
                (*it)->folderNotes.insert(note->noteId, note);
            } else {
                VNOTE_ITEMS_MAP* folderNotes = new VNOTE_ITEMS_MAP();

                //DataManager data should set autoRelease flag
                folderNotes->autoRelease = true;

                folderNotes->folderNotes.insert(note->noteId, note);
                results.notes->notes.insert(note->folderId, folderNotes);
            }
        }
    }

    return isOK;
}

CountDbVisitor::CountDbVisitor(QSqlDatabase& db, void *result)
    :DbVisitor (db, result)
{

}

bool CountDbVisitor::visitorData()
{
    bool isOK = false;

    if (nullptr != results.notes) {
        isOK = true;

        while(m_sqlQuery->next()) {
            *results.count =  m_sqlQuery->value(0).toInt();
            break;
        }
    }

    return isOK;
}

AddFolderDbVisitor::AddFolderDbVisitor(QSqlDatabase& db, void *result)
    :DbVisitor (db, result)
{

}

bool AddFolderDbVisitor::visitorData()
{
    bool isOK = false;

    if (nullptr != results.newFolder) {
        isOK = true;

        while(m_sqlQuery->next()) {
            results.newFolder->id          = m_sqlQuery->value(VNoteFolderOper::folder_id).toInt();
            results.newFolder->category    = m_sqlQuery->value(VNoteFolderOper::category_id).toInt();
            results.newFolder->name        = m_sqlQuery->value(VNoteFolderOper::folder_name).toString();
            results.newFolder->defaultIcon = m_sqlQuery->value(VNoteFolderOper::default_icon).toInt();
            results.newFolder->iconPath    = m_sqlQuery->value(VNoteFolderOper::icon_path).toString();
            results.newFolder->folder_state= m_sqlQuery->value(VNoteFolderOper::folder_state).toInt();

            results.newFolder->createTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteFolderOper::create_time).toString(),VNOTE_TIME_FMT);
            results.newFolder->modifyTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteFolderOper::modify_time).toString(),VNOTE_TIME_FMT);
            results.newFolder->deleteTime  = QDateTime::fromString(
                                    m_sqlQuery->value(VNoteFolderOper::delete_time).toString(),VNOTE_TIME_FMT);

            //************Expand fileds begin**********
            //TODO:
            //    Add the expand fileds parse code here

            //************Expand fileds end************

            break;
        }
    }

    return isOK;
}

AddNoteDbVisitor::AddNoteDbVisitor(QSqlDatabase &db, void *result)
    :DbVisitor (db, result)
{

}

bool AddNoteDbVisitor::visitorData()
{
    bool isOK = false;

    if (nullptr != results.newNote) {
        isOK = true;

        MetaDataParser metaParser;

        while(m_sqlQuery->next()) {
            VNoteItem* note = results.newNote;

            note->noteId      = m_sqlQuery->value(VNoteItemOper::note_id).toInt();
            note->folderId    = m_sqlQuery->value(VNoteItemOper::folder_id).toInt();
            note->noteType    = m_sqlQuery->value(VNoteItemOper::note_type).toInt();
            note->noteTitle   = m_sqlQuery->value(VNoteItemOper::note_title).toString();

            //Parse meta data
            QVariant metaData  = m_sqlQuery->value(VNoteItemOper::meta_data);
            note->setMetadata(metaData);
            metaParser.parse(metaData, note);

            note->noteState  = m_sqlQuery->value(VNoteItemOper::note_state).toInt();

            note->createTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteItemOper::create_time).toString(),VNOTE_TIME_FMT);
            note->modifyTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteItemOper::modify_time).toString(),VNOTE_TIME_FMT);
            note->deleteTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteItemOper::modify_time).toString(),VNOTE_TIME_FMT);

            //************Expand fileds begin**********
            //TODO:
            //    Add the expand fileds parse code here

            //************Expand fileds end************

            break;
        }
    }

    return isOK;
}

SaferQryDbVisitor::SaferQryDbVisitor(QSqlDatabase &db, void *result)
    :DbVisitor (db, result)
{

}

bool SaferQryDbVisitor::visitorData()
{
    bool isOK = false;

    if (nullptr != results.safetyDatas) {
        isOK = true;

        while(m_sqlQuery->next()) {
            VDataSafer dataSafer;
            //All data at the start moment are all exception safers.
            //Default safer type is ExceptionSafer.
            //dataSafer.setSaferType(VDataSafer::ExceptionSafer);

            dataSafer.id      = m_sqlQuery->value(VNoteSaferOper::id).toInt();
            dataSafer.folder_id = m_sqlQuery->value(VNoteSaferOper::folder_id).toInt();
            dataSafer.note_id = m_sqlQuery->value(VNoteSaferOper::note_id).toInt();
            dataSafer.path    = m_sqlQuery->value(VNoteSaferOper::path).toString();
            dataSafer.state   = m_sqlQuery->value(VNoteSaferOper::state).toInt();

            //Parse meta data
            QVariant metaData = m_sqlQuery->value(VNoteSaferOper::meta_data);
            dataSafer.meta_data = metaData.toString();

            dataSafer.createTime  = QDateTime::fromString(
                        m_sqlQuery->value(VNoteSaferOper::create_time).toString(),VNOTE_TIME_FMT);

            //************Expand fileds begin**********
            //TODO:
            //    Add the expand fileds parse code here

            //************Expand fileds end************

            results.safetyDatas->push_back(dataSafer);
        }
    }

    return isOK;
}

AddSaferDbVisitor::AddSaferDbVisitor(QSqlDatabase &db, void *result)
    :DbVisitor (db, result)
{

}

bool AddSaferDbVisitor::visitorData()
{
    //TODO:
    //    Do nothing for safer visitor

    return true;
}