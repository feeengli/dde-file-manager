/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yanghao<yanghao@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
 *             hujianzhong<hujianzhong@uniontech.com>
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
#include "event.h"

#include "private/event_p.h"

#include <QDebug>
#include <QMutex>

DPF_BEGIN_NAMESPACE

namespace PrivateGlobal
{
    static QString EVENT_TOPIC_KEY{"EVENT_TOPIC_KEY"};
    static QString EVENT_DATA_KEY{"EVENT_DATA_KEY"};
}

Event::Event()
    : d_ptr(new EventPrivate(this))
{

}

Event::Event(const Event &event):
    d_ptr(new EventPrivate(this))
{
    d_ptr->sourceHash = event.d_ptr->sourceHash;
}

Event::~Event()
{
    delete d_ptr;
}

/**
 * @brief Event::setTopic
 *  设置事件的主题
 * @param topic 主题字符串
 */
void Event::setTopic(const QString &topic)
{
    static QMutex mutex;
    mutex.lock();
    d_ptr->sourceHash[PrivateGlobal::EVENT_TOPIC_KEY] = topic;
    mutex.unlock();
}

/**
 * @brief Event::setTopic
 *  获取设置事件的主题
 * @return 主题字符串
 */
QString Event::topic() const
{
    return d_ptr->sourceHash[PrivateGlobal::EVENT_TOPIC_KEY].toString();
}

/**
 * @brief Event::setData 设置关联数据
 * @param data 任意泛型，需要Qt基本类型的QVariant构造要求,
 *  可以通过QVariant::fromValue接口进行复合类型的构造。
 */
void Event::setData(const QVariant &data)
{
    d_ptr->sourceHash[PrivateGlobal::EVENT_DATA_KEY] = data;
}

/**
 * @brief Event::data 获取设置的关联数据
 * @return QVariant任意泛型，
 *  可使用canConvert进行类型匹配后进行类型匹配后转换
 *  未知类型下直接调用qvariant_cast将导致未定义的行为，引发程序崩溃
 */
QVariant Event::data() const
{
    return d_ptr->sourceHash[PrivateGlobal::EVENT_DATA_KEY];
}

/**
 * @brief Event::setProperty 设置其他映射关联数据属性
 * @param key 索引键值
 * @param value 关联数据值
 */
void Event::setProperty(const QString &key, const QVariant value)
{
    d_ptr->sourceHash[key] = value;
}

/**
 * @brief Event::value 获取元素列表中标定的value
 * @param key 标识元素的键值
 * @return 返回QVariant 复合类型需要自己转换
 */
QVariant Event::property(const QString &key) const
{
    return d_ptr->sourceHash[key];
}

QT_BEGIN_NAMESPACE
Q_CORE_EXPORT QDebug operator <<(QDebug out, const DPF_NAMESPACE::Event &event)
{
    out << event.d_ptr->sourceHash;
    return out;
}
QT_END_NAMESPACE

DPF_END_NAMESPACE




