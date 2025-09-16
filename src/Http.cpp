/* Copyright (C) 2013 Webyog Inc

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

#include "Http.h"

#include "UrlEncode.h"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslError>
#include <QtNetwork/QSslSocket>

#include <QAuthenticator>
#include <QEventLoop>
#include <QObject>
#include <QTimer>
#include <QVariant>

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <limits>
#include <string>

namespace
{
    constexpr char USER_AGENT[] = "Mozilla/5.0 (compatible, MSIE 11, Windows NT 6.3; Trident/7.0;  rv:11.0) like Gecko";
}

CHttp::CHttp()
    : m_urlSet(false),
      m_useProxy(false),
      m_proxyPort(0),
      m_hasProxyCredentials(false),
      m_hasChallenge(false),
      m_timeout(GetTimeOut()),
      m_lastStatusCode(0),
      m_allowProxyAuth(false),
      m_allowChallengeAuth(false)
{
    QObject::connect(&m_manager,
                     &QNetworkAccessManager::authenticationRequired,
                     [this](QNetworkReply *, QAuthenticator *authenticator) {
                         if (m_allowChallengeAuth && m_hasChallenge && authenticator) {
                             authenticator->setUser(m_challengeUser);
                             authenticator->setPassword(m_challengePassword);
                         }
                     });

    QObject::connect(&m_manager,
                     &QNetworkAccessManager::proxyAuthenticationRequired,
                     [this](const QNetworkProxy &, QAuthenticator *authenticator) {
                         if (m_allowProxyAuth && m_hasProxyCredentials && authenticator) {
                             authenticator->setUser(m_proxyUser);
                             authenticator->setPassword(m_proxyPassword);
                         }
                     });
}

CHttp::~CHttp() = default;

DWORD
CHttp::GetTimeOut()
{
    return 15000;
}

void
CHttp::ResetRequestState()
{
    m_lastResponse.clear();
    m_lastHeaders.clear();
    m_lastStatusCode = 0;
    m_lastReasonPhrase.clear();
}

bool
CHttp::SetUrl(wyWChar * url)
{
    if (!url || m_urlSet)
        return false;

    QString urlString = QString::fromWCharArray(url);
    QUrl parsed = QUrl::fromUserInput(urlString);

    if (!parsed.isValid())
        return false;

    m_url = parsed;
    m_urlSet = true;
    return true;
}

bool
CHttp::SetProxyInfo(bool isproxy,
                    const wyWChar * proxy,
                    const wyWChar * proxyuser,
                    const wyWChar * proxypwd,
                    int proxyport)
{
    m_useProxy = false;
    m_proxyHost.clear();
    m_proxyPort = 0;
    m_hasProxyCredentials = false;
    m_proxyUser.clear();
    m_proxyPassword.clear();

    if (!isproxy || !proxy)
        return true;

    QString host = QString::fromWCharArray(proxy);
    if (host.isEmpty())
        return true;

    m_useProxy = true;
    m_proxyHost = host;
    m_proxyPort = proxyport;

    if (proxyuser && proxypwd) {
        m_proxyUser = QString::fromWCharArray(proxyuser);
        m_proxyPassword = QString::fromWCharArray(proxypwd);
        m_hasProxyCredentials = !m_proxyUser.isEmpty();
    }

    return true;
}

bool
CHttp::SetChallengeInfo(bool challenge, const wyWChar * username, const wyWChar * pwd)
{
    if (!challenge) {
        m_hasChallenge = false;
        m_challengeUser.clear();
        m_challengePassword.clear();
        return true;
    }

    if (!username || !pwd)
        return false;

    m_hasChallenge = true;
    m_challengeUser = QString::fromWCharArray(username);
    m_challengePassword = QString::fromWCharArray(pwd);

    return true;
}

bool
CHttp::SetContentType(const wyWChar * contenttype)
{
    if (!contenttype || !m_contenttype.isEmpty())
        return false;

    m_contenttype = QString::fromWCharArray(contenttype);
    return true;
}

bool
CHttp::GetAllHeaders(wyWChar * buffer, DWORD bufsize)
{
    if (!buffer || bufsize == 0)
        return false;

    const std::size_t maxChars = bufsize / sizeof(wyWChar);

    QString headerString;
    if (m_lastStatusCode > 0) {
        if (!m_lastReasonPhrase.isEmpty()) {
            headerString += QStringLiteral("HTTP/1.1 %1 %2\r\n").arg(m_lastStatusCode).arg(m_lastReasonPhrase);
        } else {
            headerString += QStringLiteral("HTTP/1.1 %1\r\n").arg(m_lastStatusCode);
        }
    }

    for (const QPair<QByteArray, QByteArray> &pair : m_lastHeaders) {
        headerString += QString::fromLatin1(pair.first);
        headerString += QStringLiteral(": ");
        headerString += QString::fromLatin1(pair.second);
        headerString += QStringLiteral("\r\n");
    }

    if (headerString.isEmpty())
        return false;

    std::wstring headerWide = headerString.toStdWString();
    if (headerWide.size() + 1 > maxChars)
        return false;

    std::wmemcpy(buffer, headerWide.c_str(), headerWide.size());
    buffer[headerWide.size()] = L'\0';
    return true;
}

bool
CHttp::SendData(char * data, unsigned long datalen, bool isbase64encoded, int * status, bool checkauth)
{
    if ((!data && datalen) || !m_urlSet || !m_url.isValid())
        return false;

    if (datalen > static_cast<unsigned long>(std::numeric_limits<int>::max()))
        return false;

    ResetRequestState();

    m_allowChallengeAuth = checkauth && m_hasChallenge;
    m_allowProxyAuth = checkauth && m_hasProxyCredentials;

    QNetworkRequest request(m_url);
    if (!m_contenttype.isEmpty())
        request.setHeader(QNetworkRequest::ContentTypeHeader, m_contenttype);

    request.setHeader(QNetworkRequest::UserAgentHeader, QString::fromLatin1(USER_AGENT));

    if (isbase64encoded)
        request.setRawHeader("Base64", "yes");

    if (m_url.scheme().compare(QStringLiteral("https"), Qt::CaseInsensitive) == 0) {
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(sslConfig);
    }

    if (m_useProxy) {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, m_proxyHost, static_cast<quint16>(m_proxyPort));
        if (checkauth && m_hasProxyCredentials) {
            proxy.setUser(m_proxyUser);
            proxy.setPassword(m_proxyPassword);
        }
        m_manager.setProxy(proxy);
    } else {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::NoProxy);
        m_manager.setProxy(proxy);
    }

    QByteArray payload;
    if (datalen > 0 && data)
        payload.append(data, static_cast<int>(datalen));

    QNetworkReply *reply = m_manager.post(request, payload);

    QObject::connect(reply, &QNetworkReply::readyRead, [this, reply]() {
        m_lastResponse.append(reply->readAll());
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, reply, &QNetworkReply::ignoreSslErrors);

    bool timedOut = false;
    QTimer timer;
    timer.setSingleShot(true);
    if (m_timeout > 0) {
        QObject::connect(&timer, &QTimer::timeout, [&]() {
            timedOut = true;
            reply->abort();
        });
        timer.start(m_timeout);
    }

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (timer.isActive())
        timer.stop();

    if (reply->bytesAvailable() > 0)
        m_lastResponse.append(reply->readAll());

    m_lastHeaders = reply->rawHeaderPairs();
    m_lastStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_lastReasonPhrase = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    QNetworkReply::NetworkError networkError = reply->error();
    reply->deleteLater();

    if (status)
        *status = m_lastStatusCode;

    if (timedOut)
        return false;

    return networkError == QNetworkReply::NoError;
}

char *
CHttp::GetResponse(bool * stop)
{
    if (stop && *stop)
        return nullptr;

    const std::size_t length = static_cast<std::size_t>(m_lastResponse.size());
    char *result = static_cast<char *>(std::calloc(length + 1, sizeof(char)));

    if (!result)
        return nullptr;

    if (length > 0)
        std::memcpy(result, m_lastResponse.constData(), length);

    return result;
}

char *
CHttp::GetEncodedData(char * data, DWORD * encodeddatalen)
{
    if (!data || !encodeddatalen)
        return nullptr;

    *encodeddatalen = 0;
    std::size_t datalen = std::strlen(data);

    DWORD bufferlen = static_cast<DWORD>(datalen * 3 + 1);
    char *encodereq = static_cast<char *>(std::malloc(sizeof(char) * bufferlen));
    if (!encodereq)
        return nullptr;

    CURLEncode encode;

    if (!encode.URLEncode(data, encodereq, encodeddatalen)) {
        std::free(encodereq);
        return nullptr;
    }

    return encodereq;
}

char *
CHttp::GetEncodedData(const char * data, char * outdata, DWORD * encodeddatalen)
{
    if (!data || !outdata)
        return nullptr;

    CURLEncode encode;
    DWORD encodedlen = 0;

    if (!encode.URLEncode(data, outdata, &encodedlen))
        return nullptr;

    if (encodeddatalen)
        *encodeddatalen = encodedlen;

    return outdata;
}
