/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     wangchunlin<wangchunlin@uniontech.com>
 *
 * Maintainer: wangchunlin<wangchunlin@uniontech.com>
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

#include "stubext.h"
#include "view/collectionview_p.h"
#include "view/collectionview.h"
#include "delegate/collectionitemdelegate.h"
#include "interface/canvasmanagershell.h"

#include <gtest/gtest.h>
#include <QMenu>

using namespace testing;
using namespace ddplugin_organizer;

class CollectionViewPrivateTest : public Test
{
public:
    virtual void SetUp() override {}
    virtual void TearDown() override { stub.clear(); }

    stub_ext::StubExt stub;
};

class CollectionViewTest : public Test
{
public:
    virtual void SetUp() override {}
    virtual void TearDown() override { stub.clear(); }

    stub_ext::StubExt stub;
};

TEST_F(CollectionViewPrivateTest, onIconSizeChanged) {

    QString testUuid("testUuid");
    CollectionView view(testUuid, nullptr);

    int setLevel = -1;
    stub.set_lamda(ADDR(CollectionItemDelegate, setIconLevel), [&] (CollectionItemDelegate *obj, int lv) {
        Q_UNUSED(obj)

        setLevel = lv;
        return lv;
    });

    view.d->onIconSizeChanged(1);

    EXPECT_EQ(setLevel, 1);
}

TEST_F(CollectionViewTest, setCanvasManagerShell) {

    QString testUuid("testUuid");
    CollectionView view(testUuid, nullptr);

    CanvasManagerShell *sh = nullptr;

    EXPECT_EQ(view.d->canvasManagerShell, nullptr);

    view.setCanvasManagerShell(sh);
    EXPECT_EQ(view.d->canvasManagerShell, nullptr);

    int setLevel = -1;
    bool isCallSetIconLevel = false;
    stub.set_lamda(ADDR(CollectionItemDelegate, setIconLevel), [&] (CollectionItemDelegate *obj, int lv) {
        Q_UNUSED(obj)

        isCallSetIconLevel = true;
        setLevel = lv;
        return lv;
    });

    bool isCallIconLevel = false;
    stub.set_lamda(ADDR(CanvasManagerShell, iconLevel), [&] () {
        isCallIconLevel = true;
        return 3;
    });

    sh = new CanvasManagerShell;
    view.setCanvasManagerShell(sh);
    EXPECT_TRUE(isCallIconLevel);
    EXPECT_TRUE(isCallSetIconLevel);

    isCallIconLevel = false;
    isCallSetIconLevel = false;
    view.setCanvasManagerShell(nullptr);
    EXPECT_FALSE(isCallIconLevel);
    EXPECT_FALSE(isCallSetIconLevel);

    delete sh;
    sh = nullptr;
}