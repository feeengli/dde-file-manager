/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
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

#include "keyvaluelabel.h"

#include <QFontMetrics>

DWIDGET_USE_NAMESPACE
DFMBASE_USE_NAMESPACE

KeyValueLabel::KeyValueLabel(QWidget *parent)
    : QFrame(parent)
{
    initUI();
    initPropertyMap();
}

KeyValueLabel::~KeyValueLabel()
{
}

void KeyValueLabel::initUI()
{
    leftValueLabel = new DLabel(this);
    leftValueLabel->setMinimumWidth(100);
    rightValueLabel = new DLabel(this);
    rightValueLabel->setMinimumWidth(130);
    glayout = new QGridLayout;
    glayout->setMargin(0);
    glayout->addWidget(leftValueLabel, 0, 0, 1, 2);
    glayout->addWidget(rightValueLabel, 0, 2, 1, 5);
    glayout->setColumnStretch(1, 1);
    glayout->setColumnStretch(2, 2);
    setLayout(glayout);
}

void KeyValueLabel::initPropertyMap()
{
    propertyMap.insert(kLeftValue, -1);
    propertyMap.insert(kLeftElideMode, -1);
    propertyMap.insert(kLeftTip, -1);
    propertyMap.insert(kLeftAlignment, -1);
    propertyMap.insert(kLeftWordWrap, -1);
    propertyMap.insert(kRightValue, -1);
    propertyMap.insert(kRightElideMode, -1);
    propertyMap.insert(kRightTip, -1);
    propertyMap.insert(kRightAlignment, -1);
    propertyMap.insert(kRightWordWrap, -1);
    propertyMap.insert(kRowHeight, -1);
}

/*!
 * \brief                       设置左值以及相应的显示方式
 * \param value                 需要显示的信息
 * \param elideMode             省略显示的方式
 * \param aligment              信息显示对齐方式
 * \param toolTipVisibility     是否设置toolTip
 */
void KeyValueLabel::setLeftValue(QString value, Qt::TextElideMode elideMode, Qt::Alignment aligment, bool toolTipVisibility)
{
    QString elideNote = value;
    leftValueLabel->setAlignment(aligment);
    QFontMetrics fontWidth(leftValueLabel->font());
    elideNote = fontWidth.elidedText(value, elideMode, leftValueLabel->width());
    leftValueLabel->setText(elideNote);
    if (toolTipVisibility)
        leftValueLabel->setToolTip(value);
    propertyMap[kLeftValue] = QVariant::fromValue(value);
    propertyMap[kLeftElideMode] = QVariant::fromValue(elideMode);
    propertyMap[kLeftTip] = QVariant::fromValue(toolTipVisibility);
    propertyMap[kLeftAlignment] = QVariant::fromValue(aligment);
}

/*!
 * \brief                       设置右值以及相应的显示方式
 * \param value                 需要显示的信息
 * \param elideMode             省略显示的方式
 * \param aligment              信息显示对齐方式
 * \param toolTipVisibility     是否设置toolTip
 */
void KeyValueLabel::setRightValue(QString value, Qt::TextElideMode elideMode, Qt::Alignment aligment, bool toolTipVisibility, int fontMinWidth)
{
    QString elideNote = value;
    rightValueLabel->setAlignment(aligment);
    QFontMetrics fontWidth(rightValueLabel->font());
    int fontW = rightValueLabel->width();
    if (fontW < fontMinWidth)
        fontW = fontMinWidth;
    elideNote = fontWidth.elidedText(value, elideMode, fontW);
    rightValueLabel->setText(elideNote);
    if (toolTipVisibility)
        rightValueLabel->setToolTip(value);

    propertyMap[kRightValue] = QVariant::fromValue(value);
    propertyMap[kRightElideMode] = QVariant::fromValue(elideMode);
    propertyMap[kRightTip] = QVariant::fromValue(toolTipVisibility);
    propertyMap[kRightAlignment] = QVariant::fromValue(aligment);
}

/*!
 * \brief                       设置左右右值以及相应的显示方式
 * \param leftValue             需要显示的左值信息
 * \param rightValue            需要显示的右值信息
 * \param elideMode             省略显示的方式
 * \param aligment              信息显示对齐方式
 * \param toolTipVisibility     是否设置toolTip
 */
void KeyValueLabel::setLeftRightValue(QString leftValue, QString rightValue, Qt::TextElideMode elideMode, Qt::Alignment aligment, bool toolTipVisibility)
{
    setLeftValue(leftValue, elideMode, aligment, toolTipVisibility);
    setRightValue(rightValue, elideMode, aligment, toolTipVisibility);
}

/*!
 * \brief           设置行显示的最小高度
 * \param height    高度
 */
void KeyValueLabel::setRowMinimumHeight(int height)
{
    glayout->setRowMinimumHeight(0, height);
    propertyMap[kRowHeight] = QVariant::fromValue(height);
}

/*!
 * \brief           设置左值是否可换行显示
 * \param wordWrap  true为可换行，反之亦然
 */
void KeyValueLabel::setLeftWordWrap(bool wordWrap)
{
    leftValueLabel->setWordWrap(wordWrap);
    propertyMap[kLeftWordWrap] = QVariant::fromValue(wordWrap);
}

/*!
 * \brief           设置右值是否可换行显示
 * \param wordWrap  true为可换行，反之亦然
 */
void KeyValueLabel::setRightWordWrap(bool wordWrap)
{
    rightValueLabel->setWordWrap(wordWrap);
    propertyMap[kRightWordWrap] = QVariant::fromValue(wordWrap);
}

/*!
 * \brief                   设置左值字体大小、风格以及颜色类型
 * \param sizeType          使用DTK DFontSizeManager类中SizeType枚举值
 * \param fontWeight        字体的风格（如粗体等）
 * \param foregroundRole    颜色类型(ColorType 参考DTK DPalette)
 */
void KeyValueLabel::setLeftFontSizeWeight(DFontSizeManager::SizeType sizeType, QFont::Weight fontWeight, DPalette::ColorType foregroundRole)
{
    DFontSizeManager::instance()->bind(leftValueLabel, sizeType, fontWeight);
    leftValueLabel->setForegroundRole(foregroundRole);
}

/*!
 * \brief                   设置右值字体大小、风格以及颜色类型
 * \param sizeType          使用DTK DFontSizeManager类中SizeType枚举值
 * \param fontWeight        字体的风格（如粗体等）
 * \param foregroundRole    颜色类型(ColorType 参考DTK DPalette)
 */
void KeyValueLabel::setRightFontSizeWeight(DFontSizeManager::SizeType sizeType, QFont::Weight fontWeight, DPalette::ColorType foregroundRole)
{
    DFontSizeManager::instance()->bind(rightValueLabel, sizeType, fontWeight);
    rightValueLabel->setForegroundRole(foregroundRole);
}

QString KeyValueLabel::LeftValue()
{
    return leftValueLabel->text();
}

QString KeyValueLabel::RightValue()
{
    return rightValueLabel->text();
}

void KeyValueLabel::paintEvent(QPaintEvent *evt)
{
    Qt::TextElideMode fontWeight = static_cast<Qt::TextElideMode>(propertyMap.value(kLeftElideMode).toInt());
    Qt::Alignment alignment = static_cast<Qt::Alignment>(propertyMap.value(kLeftAlignment).toInt());
    setLeftValue(propertyMap.value(kLeftValue).toString(), fontWeight, alignment, propertyMap.value(kLeftTip).toBool());
    fontWeight = static_cast<Qt::TextElideMode>(propertyMap.value(kRightElideMode).toInt());
    alignment = static_cast<Qt::Alignment>(propertyMap.value(kRightAlignment).toInt());
    setRightValue(propertyMap.value(kRightValue).toString(), fontWeight, alignment, propertyMap.value(kRightTip).toBool());
    if (propertyMap.value(kRowHeight).toInt() > -1) {
        QFontMetrics fontMetrics(font());
        QRect boundingRect = fontMetrics.boundingRect(propertyMap.value(kLeftValue).toString());
        setRowMinimumHeight(boundingRect.height() - boundingRect.y());
    }
    QFrame::paintEvent(evt);
}