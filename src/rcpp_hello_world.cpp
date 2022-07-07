
#include <Rcpp.h>
using namespace Rcpp;

#include <QDir>
#include <QFileInfo>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QQmlComponent>
#include <QGuiApplication>
#include <QQuickRenderControl>
#include <QQuickItem>
#include "appdirs.h"
#include "checkboxbase.h"


// [[Rcpp::export]]
StringVector runQml(String qmlFilePath) 
{
	int					dummyArgc = 1;
	char			**	dummyArgv = new char*[1];
	dummyArgv[0] = new char[2];
	//dummyArgv[1] = new char[8];

	memcpy(dummyArgv[0], "?", 2);
	//memcpy(dummyArgv[1], "minimal", 8);

	QGuiApplication		application(dummyArgc, dummyArgv);

	QQuickRenderControl renderControl;
	QQuickWindow		quickWindow(&renderControl);
	QQmlEngine 			qmlEngine;
	qmlRegisterType<CheckBoxBase>								("JASP",		1, 0, "CheckBoxBase"					);

	QFileInfo			qmlFile(QString::fromStdString(qmlFilePath));
	QString				qmlBaseName(qmlFile.baseName());	

	QQmlComponent		qmlComp(&qmlEngine, QUrl::fromLocalFile(qmlFile.absoluteFilePath()), QQmlComponent::PreferSynchronous);

	QQuickItem* obj = qobject_cast<QQuickItem*>(qmlComp.create());

	StringVector output;
	output.push_back(AppDirs::documents().toStdString());

	for(const auto & error : qmlComp.errors())
		output.push_back("Error in '" + qmlBaseName.toStdString() + "' at " + std::to_string(error.line()) + "," + std::to_string(error.column()) + ": " + error.description().toStdString());
		
	output.push_back(obj ? "Could load QML satisfactorily" : "Loading QML failed miserably");

	if (obj)
		output.push_back(obj->property("myProperty").toString().toStdString());
    
	return output;
}
