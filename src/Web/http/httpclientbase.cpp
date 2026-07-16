#include "httpclientbase.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QNetworkCookieJar>

#include <QFile>
#include <QFileInfo>

#include "httpcommon.hpp"

namespace QtCustom::Web {

HTTPClientBase::HTTPClientBase(QObject *parent)
    : QObject{parent}
{

}

bool HTTPClientBase::setServer(const QString &serverAddress)
{
    m_error.reset();
    auto splittedAddr = serverAddress.split(":", Qt::SkipEmptyParts);
    if (splittedAddr.size() < 2) {
        m_error.setCode(EC_InvalidAddress);
        m_error.setDetailText("No port in address");
        emit sig_errorOccurs(m_error);
        return false;
    }
    auto port = splittedAddr.back().toInt();
    if (port < 0 || port > 65535) {
        m_error.setCode(EC_InvalidAddress);
        m_error.setDetailText("Invalid port");
        emit sig_errorOccurs(m_error);
        return false;
    }
    if (!isServerListening(splittedAddr.front(), port)) {
        m_error.setCode(EC_ConnectionError);
        m_error.setDetailText(std::string("Failed to ping server. Is address correct? Address: ") + serverAddress.toStdString());
        emit sig_errorOccurs(m_error);
        return false;
    }
    m_serverAddress = serverAddress;
    return true;
}

QString HTTPClientBase::getServer() const
{
    return m_serverAddress;
}

QNetworkReply *HTTPClientBase::startFileUpload(const QString &localFilePath, const QString &fileTarget)
{
    std::shared_ptr<QFile> file = std::make_shared<QFile>(localFilePath);
    file->open(QIODevice::ReadOnly);
    if (!file->isOpen()) {
        m_error.setCode(EC_InvalidArgument);
        m_error.setDetailText("Download failed: Invalid send file path");
        emit sig_errorOccurs(m_error);
        return {};
    }

    auto request = createRequest(fileTarget);
    auto reply = m_requester.post(request, file.get());
    connect(reply, &QNetworkReply::finished, [reply, file]() { // For file correct existance during upload
        file->close();
    });
    return reply;
}

QNetworkReply *HTTPClientBase::startFileDownload(const QString &localSavefile, const QString &fileTarget)
{
    std::shared_ptr<QFile> file = std::make_shared<QFile>(localSavefile);
    if (!file->open(QIODevice::WriteOnly)) {
        m_error.setCode(EC_InvalidArgument);
        m_error.setDetailText("Download failed: Invalid local save file path");
        emit sig_errorOccurs(m_error);
        return {};
    }

    auto request = createRequest(fileTarget);
    auto reply = m_requester.get(request);
    connect(reply, &QNetworkReply::readyRead, [reply, file]() {
        file->write(reply->readAll());
    });
    connect(reply, &QNetworkReply::finished, [reply, file]() { // For file correct existance during download
        file->close();
    });
    return reply;
}

bool HTTPClientBase::isServerListening(const QString &host, quint16 port) const {
    QTcpSocket socket;
    socket.connectToHost(host, port);
    if (socket.waitForConnected(1000)) {
        socket.disconnectFromHost();
        return true;
    }
    return false;
}

void HTTPClientBase::setCommonHeader(const QString &headerName, const QString &headerData)
{
    m_commonHeaders[headerName.toUtf8()] = headerData.toUtf8();
}

QNetworkRequest HTTPClientBase::createRequest(const QString &target, const QStringList &args) const
{
    QNetworkRequest res;
    res.setUrl(HTTPCommon::createUrl(m_serverAddress, target, args));

    for (auto& [headerName, headerData] : m_commonHeaders) {
        res.setRawHeader(headerName, headerData);
    }

    return res;
}

QNetworkAccessManager &HTTPClientBase::getRequester()
{
    return m_requester;
}

}