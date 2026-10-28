#pragma once

#include "FeatureProviderInterface.h"
#include "PluginInterface.h"

class WebAccessControlPlugin : public QObject, PluginInterface, FeatureProviderInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "io.veyon.Veyon.Plugins.WebAccessControl")
	Q_INTERFACES(PluginInterface FeatureProviderInterface)
public:
	explicit WebAccessControlPlugin( QObject* parent = nullptr );
	~WebAccessControlPlugin() override = default;

	Plugin::Uid uid() const override
	{
		return Plugin::Uid{ QStringLiteral("8a9e4c10-5b2d-4e3f-9a1c-7d8e9f0a1b2c") };
	}

	QVersionNumber version() const override
	{
		return QVersionNumber( 1, 0 );
	}

	QString name() const override
	{
		return QStringLiteral( "WebAccessControl" );
	}

	QString description() const override
	{
		return tr( "Block internet access on client computers" );
	}

	QString vendor() const override
	{
		return QStringLiteral( "Veyon Community" );
	}

	QString copyright() const override
	{
		return QStringLiteral( "Marcio" );
	}

	const FeatureList& featureList() const override;

	bool controlFeature( Feature::Uid featureUid, Operation operation, const QVariantMap& arguments,
						const ComputerControlInterfaceList& computerControlInterfaces ) override;

	bool startFeature( VeyonMasterInterface& master, const Feature& feature,
					   const ComputerControlInterfaceList& computerControlInterfaces ) override;

	bool stopFeature( VeyonMasterInterface& master, const Feature& feature,
					  const ComputerControlInterfaceList& computerControlInterfaces ) override;

	bool handleFeatureMessage( ComputerControlInterface::Pointer computerControlInterface,
							   const FeatureMessage& message ) override;

	bool handleFeatureMessage( VeyonServerInterface& server,
							   const MessageContext& messageContext,
							   const FeatureMessage& message ) override;

	bool handleFeatureMessage( VeyonWorkerInterface& worker,
							   const FeatureMessage& message ) override;

private:
	enum Commands
	{
		EnableWebBlock,
		DisableWebBlock
	};

	const Feature m_webBlockFeature;
	const FeatureList m_features;
};