#include <QProcess>

#include "WebAccessControlPlugin.h"
#include "FeatureWorkerManager.h"
#include "VeyonServerInterface.h"

WebAccessControlPlugin::WebAccessControlPlugin( QObject* parent ) :
	QObject( parent ),
	m_webBlockFeature( QStringLiteral( "WebAccessBlock" ),
					   Feature::Mode | Feature::AllComponents,
					   Feature::Uid( "b4f1a2c3-6d7e-4f8a-9b0c-1d2e3f4a5b6c" ),
					   Feature::Uid(),
					   tr( "Block internet" ),
					   tr( "Internet blocked" ),
					   tr( "Click this button to block or unblock internet access on selected computers." ),
					   QStringLiteral( ":/core/lock.png" ) ),
	m_features( { m_webBlockFeature } )
{
}

const FeatureList& WebAccessControlPlugin::featureList() const
{
	return m_features;
}

bool WebAccessControlPlugin::controlFeature( Feature::Uid featureUid,
	Operation operation, const QVariantMap& arguments,
	const ComputerControlInterfaceList& computerControlInterfaces )
{
	Q_UNUSED(arguments)

	if( hasFeature( featureUid ) == false )
	{
		return false;
	}

	if( operation == Operation::Initialize )
	{
		return sendFeatureMessage( FeatureMessage{ featureUid, static_cast<FeatureMessage::Command>( EnableWebBlock ) }, computerControlInterfaces );
	}

	if( operation == Operation::Stop )
	{
		return sendFeatureMessage( FeatureMessage{ featureUid, static_cast<FeatureMessage::Command>( DisableWebBlock ) }, computerControlInterfaces );
	}

	return false;
}

bool WebAccessControlPlugin::startFeature( VeyonMasterInterface& master,
	const Feature& feature, const ComputerControlInterfaceList& computerControlInterfaces )
{
	Q_UNUSED(master)
	return controlFeature( feature.uid(), Operation::Initialize, {}, computerControlInterfaces );
}

bool WebAccessControlPlugin::stopFeature( VeyonMasterInterface& master,
	const Feature& feature, const ComputerControlInterfaceList& computerControlInterfaces )
{
	Q_UNUSED(master)
	return controlFeature( feature.uid(), Operation::Stop, {}, computerControlInterfaces );
}

bool WebAccessControlPlugin::handleFeatureMessage( ComputerControlInterface::Pointer computerControlInterface,
												   const FeatureMessage& message )
{
	Q_UNUSED(computerControlInterface)
	Q_UNUSED(message)
	return false;
}

bool WebAccessControlPlugin::handleFeatureMessage( VeyonServerInterface& server,
												   const MessageContext& messageContext,
												   const FeatureMessage& message )
{
	Q_UNUSED(server)
	Q_UNUSED(messageContext)

	if( m_webBlockFeature.uid() == message.featureUid() )
	{
		if( message.command() == static_cast<FeatureMessage::Command>( EnableWebBlock ) )
		{
#ifdef Q_OS_WIN
			QProcess::execute( QStringLiteral("netsh"), { QStringLiteral("advfirewall"), QStringLiteral("firewall"), QStringLiteral("add"), QStringLiteral("rule"), QStringLiteral("name=VeyonWebBlock"), QStringLiteral("dir=out"), QStringLiteral("action=block"), QStringLiteral("protocol=TCP"), QStringLiteral("remoteport=80,443") } );
#else
			QProcess::execute( QStringLiteral("iptables"), { QStringLiteral("-A"), QStringLiteral("OUTPUT"), QStringLiteral("-p"), QStringLiteral("tcp"), QStringLiteral("-m"), QStringLiteral("multiport"), QStringLiteral("--dports"), QStringLiteral("80,443"), QStringLiteral("-j"), QStringLiteral("REJECT") } );
#endif
		}
		else if( message.command() == static_cast<FeatureMessage::Command>( DisableWebBlock ) )
		{
#ifdef Q_OS_WIN
			QProcess::execute( QStringLiteral("netsh"), { QStringLiteral("advfirewall"), QStringLiteral("firewall"), QStringLiteral("delete"), QStringLiteral("rule"), QStringLiteral("name=VeyonWebBlock") } );
#else
			QProcess::execute( QStringLiteral("iptables"), { QStringLiteral("-D"), QStringLiteral("OUTPUT"), QStringLiteral("-p"), QStringLiteral("tcp"), QStringLiteral("-m"), QStringLiteral("multiport"), QStringLiteral("--dports"), QStringLiteral("80,443"), QStringLiteral("-j"), QStringLiteral("REJECT") } );
#endif
		}
		return true;
	}

	return false;
}

bool WebAccessControlPlugin::handleFeatureMessage( VeyonWorkerInterface& worker,
												   const FeatureMessage& message )
{
	Q_UNUSED(worker)
	Q_UNUSED(message)
	return false;
}