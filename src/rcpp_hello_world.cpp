
#include <Rcpp.h>
using namespace Rcpp;

#include <QDir>
#include <QFileInfo>
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
	dummyArgv[0] = '?';
	dummyArgv[1] = '\0';

	char			*	dummyArgvP = dummyArgv;
	QGuiApplication		application(dummyArgc, &dummyArgvP);
	QQuickRenderControl renderControl;
	QQuickWindow		quickWindow(&renderControl);
	QQmlEngine 			qmlEngine;
	QFileInfo			qmlFile(QString::fromStdString(qmlFilePath));
	QString				qmlBaseName(qmlFile.baseName());	

	QQmlComponent		qmlComp(&qmlEngine, QUrl::fromLocalFile(qmlFile.absoluteFilePath()), QQmlComponent::PreferSynchronous);

	bool worked = qmlComp.create();

	StringVector output;

	for(const auto & error : qmlComp.errors())
		output.push_back("Error in '" + qmlBaseName.toStdString() + "' at " + std::to_string(error.line()) + "," + std::to_string(error.column()) + ": " + error.description().toStdString());
		
	output.push_back(worked ? "Could load QML satisfactorily" : "Loading QML failed miserably");
    
	return output;
}
