
#include <Rcpp.h>
using namespace Rcpp;

#include <QQmlEngine>
#include <QQuickWindow>
#include <QQmlComponent>
#include <QGuiApplication>
#include <QQuickRenderControl>

// [[Rcpp::export]]
StringVector runQml(String qmlFilePath) 
{
	int					dummyArgc = 1;
	char				dummyArgv[2];
	dummyArgv[0] = 'R';
	dummyArgv[1] = '\0';
	char			*	dummyArgvP = dummyArgv;
	QGuiApplication		application(dummyArgc, &dummyArgvP);
	QQuickRenderControl renderControl;
	QQuickWindow		quickWindow(&renderControl);
	QQmlEngine 			qmlEngine;
	QQmlComponent		qmlComp(&qmlEngine, QUrl(QString::fromStdString(qmlFilePath)), QQmlComponent::PreferSynchronous);

	bool worked = qmlComp.create();

	StringVector output;

	for(const auto & error : qmlComp.errors())
		output.push_back(error.toString().toStdString());
		
	output.push_back(worked ? "Could load QML satisfactorily" : "Loading QML failed miserably");
    
	return output;
}
