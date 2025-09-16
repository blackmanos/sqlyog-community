/* Copyright (C) 2013 Webyog Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA

*/

#ifndef __CHttp_H_
#define __CHttp_H_

#include "Datatype.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkProxy>

#include <QByteArray>
#include <QList>
#include <QPair>
#include <QString>
#include <QUrl>

#ifndef HTTP_STATUS_OK
#define HTTP_STATUS_OK 200
#endif

#ifndef HTTP_STATUS_DENIED
#define HTTP_STATUS_DENIED 401
#endif

#ifndef HTTP_STATUS_PROXY_AUTH_REQ
#define HTTP_STATUS_PROXY_AUTH_REQ 407
#endif

class CHttp
{
public:
    CHttp();
    virtual ~CHttp();

    bool SetUrl(wyWChar * url);
    void SetTimeOut(DWORD timeout) { m_timeout = timeout; }
    bool GetAllHeaders(wyWChar * buffer, DWORD bufsize);
    bool SendData(char * data, unsigned long datalen, bool isbase64encoded, int * status, bool checkauth = true);
    char *GetResponse(bool * stop = 0);

    char *GetEncodedData(char * data, DWORD * encodeddatalen);
    char *GetEncodedData(const char * data, char * outdata, DWORD * encodeddatalen);

    bool SetProxyInfo(bool isproxy, const wyWChar * proxy, const wyWChar * proxyuser, const wyWChar * proxypwd, int proxyport);
    bool SetChallengeInfo(bool ischallenge, const wyWChar * username, const wyWChar * pwd);
    bool SetContentType(const wyWChar * contenttype);

private:
    DWORD GetTimeOut();
    void ResetRequestState();

    bool m_urlSet;
    QUrl m_url;
    QString m_contenttype;

    bool m_useProxy;
    QString m_proxyHost;
    int m_proxyPort;
    bool m_hasProxyCredentials;
    QString m_proxyUser;
    QString m_proxyPassword;

    bool m_hasChallenge;
    QString m_challengeUser;
    QString m_challengePassword;

    DWORD m_timeout;

    QNetworkAccessManager m_manager;

    QByteArray m_lastResponse;
    QList<QPair<QByteArray, QByteArray>> m_lastHeaders;
    int m_lastStatusCode;
    QString m_lastReasonPhrase;

    bool m_allowProxyAuth;
    bool m_allowChallengeAuth;
};

#endif
