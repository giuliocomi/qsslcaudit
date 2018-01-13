#include "ssltest.h"
#include "debug.h"
#include "sslcertgen.h"


SslTest::SslTest()
{

}

void SslCertificatesTest::report(const QList<XSslError> sslErrors,
                                 const QList<QAbstractSocket::SocketError> socketErrors,
                                 bool sslConnectionEstablished,
                                 bool dataReceived) const
{
    if (dataReceived) {
        RED("test failed, client accepted fake certificate, data was intercepted");
        return;
    }

    if (sslConnectionEstablished && !dataReceived
            && !socketErrors.contains(QAbstractSocket::RemoteHostClosedError)) {
        RED("test failed, client accepted fake certificate, but no data transmitted");
        return;
    }

    GREEN("test passed, client refused fake certificate");
}

void SslProtocolsTest::report(const QList<XSslError> sslErrors,
                              const QList<QAbstractSocket::SocketError> socketErrors,
                              bool sslConnectionEstablished,
                              bool dataReceived) const
{
    if (dataReceived) {
        RED("test failed, client accepted fake certificate and weak protocol, data was intercepted");
        return;
    }

    if (sslConnectionEstablished && !dataReceived
            && !socketErrors.contains(QAbstractSocket::RemoteHostClosedError)) {
        RED("test failed, client accepted fake certificate and weak protocol, but no data transmitted");
        return;
    }

    if (sslConnectionEstablished) {
        RED("test failed, client accepted weak protocol");
        return;
    }

    GREEN("test passed, client does not accept weak protocol");
}

bool SslProtocolsTest::prepare(const SslUserSettings &settings)
{
    XSslKey key;
    QList<XSslCertificate> chain = settings.getUserCert();
    if (chain.size() != 0) {
        key = settings.getUserKey();
    }

    if ((chain.size() == 0) || key.isNull()) {
        QString cn;

        if (settings.getUserCN().length() > 0) {
            cn = settings.getUserCN();
        } else {
            cn = "www.example.com";
        }

        QPair<XSslCertificate, XSslKey> generatedCert = SslCertGen::genSignedCert(cn);
        chain.clear();
        chain << generatedCert.first;
        key = generatedCert.second;
    }

    // these parameters should be insignificant, but we tried to make them as much "trustful" as possible
    setLocalCert(chain);
    setPrivateKey(key);

    setProtoAndCiphers();

    return true;
}
