#pragma once

#include <QObject>
#include <VeyonCore.h>
#include <PluginInterface.h>
#include <FeatureProviderInterface.h>

class WebAccessControlPlugin : public QObject,
                               public PluginInterface,
                               public FeatureProviderInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "webaccesscontrol.json")
    Q_INTERFACES(PluginInterface FeatureProviderInterface)

public:
    explicit WebAccessControlPlugin(QObject *parent = nullptr);
    ~WebAccessControlPlugin() override = default;

    // Metadados do Plugin
    PluginInfo pluginInfo() const override
    {
        return {
            QStringLiteral("WebAccessControl"),
            PluginInfo::Type::General,
            QObject::tr("Web Access Control"),
            QStringLiteral("1.0"),
            QObject::tr("Controle de acesso à web e bloqueio total de internet"),
            QStringLiteral("Custom")
        };
    }

    bool initialize(const QVariantMap &configuration) override;

    // Registro das Ações e Botões
    FeatureList features() const override;
    bool startFeature(const Feature &feature, const ComputerControlInterface::Pointer &computer) override;
    bool stopFeature(const Feature &feature, const ComputerControlInterface::Pointer &computer) override;

    // Identificador único do botão na barra do Veyon Master
    static constexpr Feature::Uid InternetKillSwitchUid = 0x57454201;

private:
    void applyInternetBlock(bool block);
};