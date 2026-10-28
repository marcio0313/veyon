#include "WebAccessControlPlugin.h"
#include <QProcess>
#include <QDebug>

WebAccessControlPlugin::WebAccessControlPlugin(QObject *parent)
    : QObject(parent)
{
}

bool WebAccessControlPlugin::initialize(const QVariantMap &)
{
    return true;
}

FeatureList WebAccessControlPlugin::features() const
{
    Feature killSwitch(InternetKillSwitchUid,
                       Feature::Type::Action,
                       tr("Bloquear Internet"),
                       tr("Corta ou restabelece todo o acesso web nos alunos selecionados"));

    // Utiliza ícones de rede padrão do tema do sistema
    killSwitch.setIcon(QIcon::fromTheme(QStringLiteral("network-disconnect")));

    return { killSwitch };
}

bool WebAccessControlPlugin::startFeature(const Feature &feature, const ComputerControlInterface::Pointer &computer)
{
    Q_UNUSED(computer);

    if (feature.uid() == InternetKillSwitchUid)
    {
        qInfo() << "[WebAccessControl] Ativando bloqueio total de internet...";
        applyInternetBlock(true);
        return true;
    }

    return false;
}

bool WebAccessControlPlugin::stopFeature(const Feature &feature, const ComputerControlInterface::Pointer &computer)
{
    Q_UNUSED(computer);

    if (feature.uid() == InternetKillSwitchUid)
    {
        qInfo() << "[WebAccessControl] Desativando bloqueio de internet...";
        applyInternetBlock(false);
        return true;
    }

    return false;
}

void WebAccessControlPlugin::applyInternetBlock(bool block)
{
#ifdef Q_OS_WIN
    if (block)
    {
        // Cria regras de saída bloqueando portas 80 (HTTP), 443 (HTTPS) e 53 (DNS)
        QProcess::execute(QStringLiteral("netsh"), {
            QStringLiteral("advfirewall"), QStringLiteral("firewall"), QStringLiteral("add"), QStringLiteral("rule"),
            QStringLiteral("name=Veyon_Block_Web"), QStringLiteral("dir=out"), QStringLiteral("action=block"),
            QStringLiteral("protocol=TCP"), QStringLiteral("remoteport=80,443")
        });

        QProcess::execute(QStringLiteral("netsh"), {
            QStringLiteral("advfirewall"), QStringLiteral("firewall"), QStringLiteral("add"), QStringLiteral("rule"),
            QStringLiteral("name=Veyon_Block_DNS"), QStringLiteral("dir=out"), QStringLiteral("action=block"),
            QStringLiteral("protocol=UDP"), QStringLiteral("remoteport=53")
        });
    }
    else
    {
        // Remove as regras de bloqueio do Firewall
        QProcess::execute(QStringLiteral("netsh"), {
            QStringLiteral("advfirewall"), QStringLiteral("firewall"), QStringLiteral("delete"), QStringLiteral("rule"),
            QStringLiteral("name=Veyon_Block_Web")
        });

        QProcess::execute(QStringLiteral("netsh"), {
            QStringLiteral("advfirewall"), QStringLiteral("firewall"), QStringLiteral("delete"), QStringLiteral("rule"),
            QStringLiteral("name=Veyon_Block_DNS")
        });
    }
#elif defined(Q_OS_LINUX)
    if (block)
    {
        QProcess::execute(QStringLiteral("iptables"), {QStringLiteral("-A"), QStringLiteral("OUTPUT"), QStringLiteral("-p"), QStringLiteral("tcp"), QStringLiteral("-m"), QStringLiteral("multiport"), QStringLiteral("--dports"), QStringLiteral("80,443"), QStringLiteral("-j"), QStringLiteral("DROP")});
    }
    else
    {
        QProcess::execute(QStringLiteral("iptables"), {QStringLiteral("-D"), QStringLiteral("OUTPUT"), QStringLiteral("-p"), QStringLiteral("tcp"), QStringLiteral("-m"), QStringLiteral("multiport"), QStringLiteral("--dports"), QStringLiteral("80,443"), QStringLiteral("-j"), QStringLiteral("DROP")});
    }
#endif
}