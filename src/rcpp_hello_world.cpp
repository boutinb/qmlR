
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
#include <QQmlContext>
#include "utilities/appdirs.h"
#include "analysisform.h"
#include "controls/jaspcontrol.h"
#include "controls/checkboxbase.h"
#include "controls/comboboxbase.h"
#include "controls/textinputbase.h"
#include "controls/componentslistbase.h"
#include "controls/factorsformbase.h"
#include "controls/inputlistbase.h"
#include "controls/textareabase.h"
#include "controls/sliderbase.h"
#include "controls/expanderbuttonbase.h"
#include "controls/variableslistbase.h"
#include "controls/variablesformbase.h"
#include "controls/factorlevellistbase.h"
#include "controls/tableviewbase.h"
#include "controls/radiobuttonbase.h"
#include "controls/radiobuttonsgroupbase.h"
#include "controls/jaspdoublevalidator.h"
#include "utilities/messageforwarder.h"
#include "jasptheme.h"
#include "preferencesmodelbase.h"


// [[Rcpp::export]]
StringVector runQml(String qmlFilePath) 
{
	int					dummyArgc = 1;
	char			**	dummyArgv = new char*[1];
	dummyArgv[0] = new char[2];
	//dummyArgv[1] = new char[8];

	memcpy(dummyArgv[0], "?", 2);
	//memcpy(dummyArgv[1], "minimal", 8);

	StringVector output;
	output.push_back(AppDirs::documents().toStdString());


	static QGuiApplication		application(dummyArgc, dummyArgv);

	static QQuickRenderControl renderControl;
	static QQuickWindow		quickWindow(&renderControl);
	static QQmlEngine 			qmlEngine;
	qmlRegisterUncreatableType<JASPControl>						("JASP",		1, 0 ,"JASP",				"Impossible to create JASP Object"	); //This is here to keep JASP.enum short I guess?
	qmlRegisterUncreatableType<MessageForwarder>				("JASP",		1, 0, "MessageForwarder",	"You can't touch this"				);

	qmlRegisterType<JaspTheme>									("JASP",		1, 0, "JaspTheme"						);
	qmlRegisterType<AnalysisForm>								("JASP",		1, 0, "AnalysisForm"					);
	qmlRegisterType<JASPControl>								("JASP",		1, 0, "JASPControl"						);
	qmlRegisterType<ExpanderButtonBase>							("JASP",		1, 0, "ExpanderButtonBase"				);
	qmlRegisterType<CheckBoxBase>								("JASP",		1, 0, "CheckBoxBase"					);
	qmlRegisterType<SliderBase>									("JASP",		1, 0, "SliderBase"						);
	qmlRegisterType<TextInputBase>								("JASP",		1, 0, "TextInputBase"					);
	qmlRegisterType<TextAreaBase>								("JASP",		1, 0, "TextAreaBase"					);
	qmlRegisterType<ComboBoxBase>								("JASP",		1, 0, "ComboBoxBase"					);
	qmlRegisterType<RadioButtonBase>							("JASP",		1, 0, "RadioButtonBase"					);
	qmlRegisterType<RadioButtonsGroupBase>						("JASP",		1, 0, "RadioButtonsGroupBase"			);
	qmlRegisterType<ComponentsListBase>							("JASP",		1, 0, "ComponentsListBase"				);
	qmlRegisterType<FactorsFormBase>							("JASP",		1, 0, "FactorsFormBase"					);
	qmlRegisterType<InputListBase>								("JASP",		1, 0, "InputListBase"					);
	qmlRegisterType<FactorLevelListBase>						("JASP",		1, 0, "FactorLevelListBase"				);
	qmlRegisterType<VariablesListBase>							("JASP",		1, 0, "VariablesListBase"				);
	qmlRegisterType<VariablesFormBase>							("JASP",		1, 0, "VariablesFormBase"				);
	qmlRegisterType<TableViewBase>								("JASP",		1, 0, "TableViewBase"					);
	qmlRegisterType<JASPDoubleValidator>						("JASP",		1, 0, "JASPDoubleValidator"				);

	qmlEngine.addImportPath("qrc:///components");

	output.push_back("IMPORT PATHS:");
	for(const QString & p : qmlEngine.importPathList())
		output.push_back("PATH: " + p.toStdString());

	PreferencesModelBase* preferencesModel = new PreferencesModelBase();
	qmlEngine.rootContext()->setContextProperty("preferencesModel",			preferencesModel);

	output.push_back("BASE URL: " + qmlEngine.baseUrl().path().toStdString());

//	QFileInfo			qmlFileThemeQrc(QString::fromStdString("qrc:///components/JASP/Theme/Theme.qml"));
//	QFileInfo			qmlFileTheme(QString::fromStdString("../../jasp-desktop/QMLComponents/components/JASP/Theme/Theme.qml"));
//	output.push_back("File Theme: " + qmlFileTheme.absoluteFilePath().toStdString() + (qmlFileTheme.exists() ? " EXISTS" : " DOES NOT EXIST"));
//	output.push_back("File Theme Qrc: " + qmlFileThemeQrc.absoluteFilePath().toStdString()  + (qmlFileThemeQrc.exists() ? " EXISTS" : "DOES NOT EXIST"));

	//QQmlComponent		qmlCompTheme(&qmlEngine, QUrl::fromLocalFile(qmlFileTheme.absoluteFilePath()), QQmlComponent::PreferSynchronous);
	QQmlComponent		qmlCompTheme(&qmlEngine, QUrl("qrc:///components/JASP/Theme/Theme.qml"), QQmlComponent::PreferSynchronous);
	JaspTheme*			jaspThemeObj = qobject_cast<JaspTheme*>(qmlCompTheme.create());
	output.push_back(jaspThemeObj ? "JASPTheme created" : "JASPTheme was not created");
	if (!jaspThemeObj)
		for(const auto & error : qmlCompTheme.errors())
			output.push_back("Error at " + std::to_string(error.line()) + "," + std::to_string(error.column()) + ": " + error.description().toStdString());



	bool	debug	= false,
			isMac	= false,
			isLinux = false;

#ifdef JASP_DEBUG
	debug = true;
#endif

#ifdef __APPLE__
	isMac = true;
#endif

#ifdef __linux__
	isLinux = true;
#endif

	bool isWindows = !isMac && !isLinux;

	qmlEngine.rootContext()->setContextProperty("jaspTheme",			jaspThemeObj);
	qmlEngine.rootContext()->setContextProperty("DEBUG_MODE",			debug);
	qmlEngine.rootContext()->setContextProperty("MACOS",				isMac);
	qmlEngine.rootContext()->setContextProperty("LINUX",				isLinux);
	qmlEngine.rootContext()->setContextProperty("WINDOWS",				isWindows);
	qmlEngine.rootContext()->setContextProperty("INTERACTION_SEPARATOR", Term::separator);

	QFileInfo			qmlFile(QString::fromStdString(qmlFilePath));
	QString				qmlBaseName(qmlFile.baseName());

	QQmlComponent		qmlComp(&qmlEngine, QUrl::fromLocalFile(qmlFile.absoluteFilePath()), QQmlComponent::PreferSynchronous);

	QQuickItem* obj = qobject_cast<QQuickItem*>(qmlComp.create());


	for(const auto & error : qmlComp.errors())
		output.push_back("Error in '" + qmlBaseName.toStdString() + "' at " + std::to_string(error.line()) + "," + std::to_string(error.column()) + ": " + error.description().toStdString());
		
	output.push_back(obj ? "Could load QML satisfactorily" : "Loading QML failed miserably");

	if (obj)
	{
		output.push_back(obj->property("myProperty").toString().toStdString());
		obj->setProperty("integerValue", 20);
	}
	return output;
}
