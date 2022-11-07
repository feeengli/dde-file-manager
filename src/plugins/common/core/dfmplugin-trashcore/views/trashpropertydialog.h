/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     lixiang<lixianga@uniontech.com>
 *
 * Maintainer: lixiang<lixianga@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRASHPROPERTYDIALOG_H
#define TRASHPROPERTYDIALOG_H

#include "dfmplugin_trashcore_global.h"
#include "dfm-base/widgets/dfmkeyvaluelabel/keyvaluelabel.h"
#include "dfm-base/utils/filestatisticsjob.h"

#include <DDialog>

namespace dfmplugin_trashcore {
class TrashPropertyDialog : public DTK_WIDGET_NAMESPACE::DDialog
{
    Q_OBJECT
public:
    explicit TrashPropertyDialog(QWidget *parent = nullptr);

    virtual ~TrashPropertyDialog() override;

private:
    void initUI();
    void updateLeftInfo(const int &count);
    void calculateSize();
    void updateUI(qint64 size, int count);

protected:
    virtual void showEvent(QShowEvent *event) override;

private:
    DTK_WIDGET_NAMESPACE::DLabel *trashNameLabel { nullptr };
    DTK_WIDGET_NAMESPACE::DLabel *trashIconLabel { nullptr };
    DFMBASE_NAMESPACE::KeyValueLabel *fileCountAndFileSize { nullptr };
};
}
#endif   // TRASHPROPERTYDIALOG_H