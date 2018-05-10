/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2017 - 2018 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "JsonSettings.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QFile>
#include <QtCore/QSaveFile>
#include <QtCore/QTextStream>

namespace Otter
{

JsonSettings::JsonSettings() : QJsonDocument(),
	m_hasError(false)
{
}

JsonSettings::JsonSettings(const QString &path) : QJsonDocument(),
	m_path(path),
	m_hasError(false)
{
	QFile file(path);

	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}

	if (file.read(2) == QByteArray(2, '/'))
	{
		file.reset();

		QStringList comment;
		QTextStream stream(&file);
		stream.setCodec("UTF-8");

		while (!stream.atEnd())
		{
			const QString line(stream.readLine());

			if (line.startsWith(QLatin1String("//")))
			{
				comment.append(line.mid(3));
			}
			else
			{
				break;
			}
		}

		m_comment = comment.join(QLatin1Char('\n'));

		file.seek(stream.pos());
	}
	else
	{
		file.reset();
	}

	const QJsonDocument document(QJsonDocument::fromJson(file.readAll()));

	if (document.isArray())
	{
		setArray(document.array());
	}
	else
	{
		setObject(document.object());
	}

	file.close();
}

void JsonSettings::setComment(const QString &comment)
{
	m_comment = comment;
}

QRect JsonSettings::readRectangle(const QJsonValue &value)
{
	QRect rectangle;

	if (value.isString())
	{
		const QStringList geometry(value.toString().split(QLatin1Char(',')));

		if (geometry.count() == 4)
		{
			rectangle = {geometry.at(0).simplified().toInt(), geometry.at(1).simplified().toInt(), geometry.at(2).simplified().toInt(), geometry.at(3).simplified().toInt()};
		}
	}
	else if (value.isObject())
	{
		const QJsonObject object(value.toObject());

		rectangle = {object.value(QLatin1String("x")).toInt(), object.value(QLatin1String("y")).toInt(), object.value(QLatin1String("width")).toInt(), object.value(QLatin1String("height")).toInt()};
	}

	return rectangle;
}

QString JsonSettings::getComment() const
{
	return m_comment;
}

bool JsonSettings::save(const QString &path, bool isAtomic)
{
	if (path.isEmpty() && m_path.isEmpty())
	{
		m_hasError = true;

		return false;
	}

	QFileDevice *file(nullptr);

	if (isAtomic)
	{
		file = new QSaveFile(path.isEmpty() ? m_path : path);
	}
	else
	{
		file = new QFile(path.isEmpty() ? m_path : path);
	}

	if (!file->open(QIODevice::WriteOnly))
	{
		m_hasError = true;

		file->deleteLater();

		return false;
	}

	m_hasError = false;

	if (!m_comment.isEmpty())
	{
		QTextStream stream(file);
		stream.setCodec("UTF-8");

		const QStringList comment(m_comment.split(QLatin1Char('\n')));

		for (int i = 0; i < comment.count(); ++i)
		{
			stream << QLatin1String("// ") << comment.at(i) << QLatin1Char('\n');
		}

		stream << QLatin1Char('\n');
	}

	QByteArray data(toJson());
	int spacesAmount(0);
	int tabsAmount(0);
	int lineStartPosition(0);
	bool isReplacingSpaces(true);

	for (int i = 0; i < data.count(); ++i)
	{
		const char character(data.at(i));

		if (character == '\n')
		{
			spacesAmount = 0;
			tabsAmount = 0;
			lineStartPosition = (i + 1);
			isReplacingSpaces = true;
		}
		else if (isReplacingSpaces)
		{
			if (character == ' ')
			{
				++spacesAmount;

				if (spacesAmount == 4)
				{
					spacesAmount = 0;

					++tabsAmount;
				}
			}
			else
			{
				if (tabsAmount > 0)
				{
					data.replace(lineStartPosition, (tabsAmount * 4), QByteArray(tabsAmount, '\t'));

					i -= (tabsAmount * 3);
				}

				isReplacingSpaces = false;
			}
		}
	}

	file->write(data);

	bool result(true);

	if (isAtomic)
	{
		result = qobject_cast<QSaveFile*>(file)->commit();
	}
	else
	{
		file->close();
	}

	file->deleteLater();

	return result;
}

bool JsonSettings::hasError() const
{
	return m_hasError;
}

}
