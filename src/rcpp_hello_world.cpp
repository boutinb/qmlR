
#include <Rcpp.h>
using namespace Rcpp;

#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "utilities/qutils.h"
#include "utilities/qmlutils.h"
#include <iostream>
#include "jasptheme.h"
#include <QQmlContext>
#include <QThread>
#include <QFileInfo>
#include <QAbstractEventDispatcher>
#include "controls/checkboxbase.h"
#include "analysisform.h"
#include "controls/sliderbase.h"
#include "controls/textareabase.h"
#include "controls/textinputbase.h"
#include "controls/radiobuttonbase.h"
#include "controls/radiobuttonsgroupbase.h"
#include "controls/componentslistbase.h"
#include "controls/factorsformbase.h"
#include "controls/inputlistbase.h"
#include "controls/factorlevellistbase.h"
#include "controls/comboboxbase.h"
#include "controls/variablesformbase.h"
#include "controls/tableviewbase.h"
#include "controls/jaspdoublevalidator.h"
#include "controls/expanderbuttonbase.h"
#include "controls/variableslistbase.h"
#include "rsyntax/formulabase.h"

static bool initialized = false;
static QGuiApplication* application = nullptr;
static QQmlApplicationEngine* engine = nullptr;

void init()
{
	if (initialized) return;
	initialized = true;
	Q_INIT_RESOURCE(qmlComponents);

	int		argc = 2;
	const char*	platformArg = "-platform";
	const char*	platformOpt = "offscreen";
	char * argv[] = {const_cast<char*>(platformArg), const_cast<char*>(platformOpt)};

	application = new QGuiApplication(argc, argv);
	engine = new QQmlApplicationEngine();

	qmlRegisterUncreatableType<JASPControl>						("JASP",		1, 0 ,"JASP",				"Impossible to create JASP Object"	); //This is here to keep JASP.enum short I guess?
	qmlRegisterType<JaspTheme>									("JASP",		1, 0, "JaspTheme"										);
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
	qmlRegisterType<FormulaBase>								("JASP",		1, 0, "Formula"							);

	PreferencesModelBase* preferences = new PreferencesModelBase();

	engine->rootContext()->setContextProperty("preferencesModel",		preferences						);
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

	engine->rootContext()->setContextProperty("DEBUG_MODE",			debug);
	engine->rootContext()->setContextProperty("MACOS",				isMac);
	engine->rootContext()->setContextProperty("LINUX",				isLinux);
	engine->rootContext()->setContextProperty("WINDOWS",				isWindows);
	engine->rootContext()->setContextProperty("INTERACTION_SEPARATOR", Term::separator);

	engine->addImportPath("qrc:///components");

	QQmlComponent	qmlComp2( engine, "qrc:///components/JASP/Theme/Theme.qml", QQmlComponent::PreferSynchronous);
	qmlComp2.create();

	//engine.load(QUrl("qrc:///components/JASP/Theme/Theme.qml"));

	engine->rootContext()->setContextProperty("jaspTheme",			JaspTheme::currentTheme()		);
	application->processEvents();

}

// [[Rcpp::export]]
StringVector runQml(String qmlFilePath)
{
	init();

	StringVector output;
	output.push_back("START");


	QFileInfo			qmlFile(QString::fromStdString("/Users/brunoboutin/JASP/source/qmlR/src/Descriptives.qml"));
	QString				qmlBaseName(qmlFile.baseName());
	if (!qmlFile.exists())
	{
		output.push_back("File NOT found");
		return output;
	}

	output.push_back("Found file");

	QUrl urlFile = QUrl::fromLocalFile(qmlFile.absoluteFilePath());
	QQmlComponent	qmlComp( engine, urlFile, QQmlComponent::PreferSynchronous);

	AnalysisForm* form = qobject_cast<AnalysisForm*>(qmlComp.create());

	for(const auto & error : qmlComp.errors())
		output.push_back("Error when cerating component at " + std::to_string(error.line()) + "," + std::to_string(error.column()) + ": " + error.description().toStdString());

	if (!form)
	{
		output.push_back("Form not created");
		return output;
	}

	output.push_back("Form created");
	Json::Value options(Json::objectValue);

	options["formula"] = "~ one + two";

	output.push_back("options: " + options.toStyledString() );

	application->processEvents();
	AnalysisBase* analysis = new AnalysisBase();
	form->setAnalysis(analysis);
	output.push_back("Start parsing");
	if (form->parseOptions(options))
		output.push_back("Option parsed: " + options.toStyledString() );
	else
	{
		output.push_back("Error!" );
		QString error;
		if (!form->hasError()) error = form->getError();
		output.push_back("Problem when parsing options: " + error.toStdString() );
	}

	form->setAnalysis(nullptr);
	delete form;

	return output;
}
