/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2013 - 2016 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#ifndef OTTER_PREFERENCESPRIVACYPAGEWIDGET_H
#define OTTER_PREFERENCESPRIVACYPAGEWIDGET_H

#include <QtWidgets/QWidget>

namespace Otter
{

namespace Ui
{
	class PreferencesPrivacyPageWidget;
}

class PreferencesPrivacyPageWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PreferencesPrivacyPageWidget(QWidget *parent = NULL);
	~PreferencesPrivacyPageWidget();

protected:
	void changeEvent(QEvent *event);

protected slots:
	void setupThirdPartyCookiesExceptions();
	void setupClearHistory();
	void save();

private:
	QStringList m_thirdPartyCookiesAcceptedHosts;
	QStringList m_thirdPartyCookiesRejectedHosts;
	QStringList m_clearHisorySettings;
	Ui::PreferencesPrivacyPageWidget *m_ui;

signals:
	void settingsModified();
};

}

#endif
