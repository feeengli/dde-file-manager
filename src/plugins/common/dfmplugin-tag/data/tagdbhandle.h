// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TAGDBHANDLE_H
#define TAGDBHANDLE_H

#include "dfm-base/base/db/sqlitehandle.h"

#include <QObject>
#include <QMap>

DFMBASE_USE_NAMESPACE
namespace dfmplugin_tag {

class TagDbHandle : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TagDbHandle)
public:
    enum SqlType {
        kNone = 0,
        kGetAllTags,
        kGetTagsThroughFile,
        kGetFilesThroughTag,
        kGetTagColor,
        kGetTagCount,
        kInsertTag
    };

    enum DbFileExistState {
        kExist = 0,
        kNoExist,
        kNoThisDir,
        kPlaceHolder,
        kFailedExecSql
    };

public:
    static TagDbHandle *instance();
    QVariantMap getAllTags();
    QVariantMap getTagsColor(const QStringList &tags);
    QVariantMap getTagsByUrls(const QStringList &urlList);
    QVariant getSameTagsOfDiffUrls(const QStringList &urlList);
    QVariantMap getFilesByTag(const QStringList &tags);

    bool addTagProperty(const QVariantMap &data);
    bool addTagsForFiles(const QVariantMap &data);
    bool removeTagsOfFiles(const QVariantMap &data);
    bool deleteTags(const QStringList &tags);
    bool deleteFiles(const QStringList &urls);
    bool changeTagColors(const QVariantMap &data);
    bool changeTagNames(const QVariantMap &data);
    bool changeFilePaths(const QVariantMap &data);

    QString lastError();
    bool checkDatabase();

private:
    bool checkTag(const QString &tag);
    bool checkFile(const QString &file);
    bool insertTagProperty(const QString &name, const QVariant &value);
    bool tagFile(const QString &file, const QVariant &tags);
    bool removeSpecifiedTagOfFile(const QString &url, const QVariant &val);
    bool changeTagColor(const QString &tagName, const QString &newTagColor);
    bool changeTagName(const QString &tagName, const QString &newName);
    bool changeFilePath(const QString &oldPath, const QString &newPath);

    bool chechTable(const QString &tableName);
    bool createTable(const QString &tableName);

    explicit TagDbHandle(QObject *parent = nullptr);
    virtual ~TagDbHandle() = default;

Q_SIGNALS:
    void addedNewTags(const QVariant &newTags);
    void deletedTags(const QVariant &beDeletedTags);
    void changedTagColor(const QVariantMap &oldAndNewColor);
    void changedTagName(const QVariantMap &oldAndNewName);
    void filesWereTagged(const QVariantMap &filesWereTagged);
    void untagFiles(const QVariantMap &delTagsOfFile);

private:
    SqliteHandle *handle { nullptr };
    QString lastErr;
};

}

#endif   // TAGDBHANDLE_H