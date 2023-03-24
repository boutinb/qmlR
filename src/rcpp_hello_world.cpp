
#include <Rcpp.h>
using namespace Rcpp;

#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <iostream>
#include <QQmlContext>
#include <QThread>
#include <QFileInfo>
#include <QAbstractEventDispatcher>
#include <QQmlComponent>
#include <QQuickItem>
#include <QDir>

#include <filesystem>

static bool initialized = false;
static QGuiApplication* application = nullptr;
static QQmlApplicationEngine* engine = nullptr;
static const std::string SOURCE_FOLDER= "/Users/brunoboutin/JASP/source/qmlR/";

void printFolder(StringVector& output, const QDir& dir, int depth = 0)
{
	for (QFileInfo info : dir.entryInfoList())
	{
		QString text = QString("Niveau %1: ").arg(depth);
		text += info.baseName() + ", " + info.absoluteFilePath() + ", " + info.canonicalFilePath() + ": " + (info.isDir() ? "Folder" : (info.isFile() ? "file" : "???"));
		output.push_back(text.toStdString());
		if (info.isDir() && info.baseName() != "qt-project")
		{
			QDir dir2(info.absoluteFilePath());
			printFolder(output, dir2, depth + 1);
		}
		else if (info.baseName() == "qmldir")
		{
			QFile inputFile(info.absoluteFilePath());
			if (inputFile.open(QIODevice::ReadOnly))
			{
			   QTextStream in(&inputFile);
			   while (!in.atEnd())
				  output.push_back(in.readLine().toStdString());
			   inputFile.close();
			}
		}

	}

}

String getEnv(const std::string& name)
{
	Function f("Sys.getenv('" + name + "')");

	return f();
}

void init(StringVector& output)
{
	if (initialized) return;
	initialized = true;

	QString rHome = qgetenv("R_HOME");
	QString qmlRFolder = rHome + "/library/qmlR";
	output.push_back("R_HOME: " + qgetenv("R_HOME").toStdString());

	QCoreApplication::addLibraryPath(qmlRFolder + "/plugins");

	int					dummyArgc = 1;
	char				dummyArgv[2];
	dummyArgv[0] = '?';
	dummyArgv[1] = '\0';

	const char* qmlR = SOURCE_FOLDER.c_str();
	const char*	platformArg = "-platform";
	const char*	platformOpt = "cocoa";

	std::vector<const char*> arguments = {qmlR, platformArg, platformOpt};

	int		argc = arguments.size();
	char** argvs = new char*[argc];

	for (int i = 0; i < argc; i++)
	{
		argvs[i] = new char[strlen(arguments[i]) + 1];
		memset(argvs[i], '\0',				strlen(arguments[i]) + 1);
		memcpy(argvs[i], arguments[i],		strlen(arguments[i]));
		argvs[i][							strlen(arguments[i])] = '\0';
	}


	qputenv("QT_QPA_PLATFORM", "cocoa");
	//qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", platformFolder.toStdString().c_str());
	char			*	dummyArgvP = dummyArgv;
	application = new QGuiApplication(argc, argvs);
	engine = new QQmlApplicationEngine();


	//new MessageForwarder(this); //We do not need to store this

	//qmlRegisterUncreatableType<JASPControl>						("JASP",		1, 0 ,"JASP",				"Impossible to create JASP Object"	); //This is here to keep JASP.enum short I guess?
	//qmlRegisterUncreatableType<MessageForwarder>				("JASP",		1, 0, "MessageForwarder",	"You can't touch this"				);

	//qmlRegisterType<JaspTheme>									("JASP",		1, 0, "JaspTheme"						);
	//ALTNavigation::registerQMLTypes("JASP");

	//PreferencesModelBase* preferences = new PreferencesModelBase();

	//engine->rootContext()->setContextProperty("preferencesModel",		preferences						);

	engine->addImportPath("/Users/brunoboutin/Qt/6.4.2/macos/qml");
	//engine->addImportPath("/Users/brunoboutin/JASP/source/build-testPlugin-Qt_6_4_2_for_macOS-Debug");
	engine->addImportPath("/Users/brunoboutin/JASP/source/build-jaspQMLComponents-Qt_6_4_2_for_macOS-Debug/components");

	//QQmlComponent	qmlComp2( engine, "qrc:///components/JASP/Theme/Theme.qml", QQmlComponent::PreferSynchronous);
	//qmlComp2.create();

	//engine.load(QUrl("qrc:///components/JASP/Theme/Theme.qml"));

	//engine->rootContext()->setContextProperty("jaspTheme",			JaspTheme::currentTheme()		);
	//application->processEvents();


	output.push_back("Base URL: " + engine->baseUrl().toDisplayString().toStdString());
	output.push_back("Current Path: " + std::filesystem::current_path().string());

	QStringList paths = engine->importPathList();
	for (QString path : paths)
		output.push_back("Import path: " + path.toStdString());
	QStringList pluginPaths = engine->pluginPathList();
	for (QString path : pluginPaths)
		output.push_back("Plugin path: " + path.toStdString());

	QDir dir(":/");

	printFolder(output, dir);


}

// [[Rcpp::export]]


StringVector runQml(String qmlFileName, String options)
{
	StringVector output;

	init(output);
	engine->clearComponentCache();

	std::string qmlFileNameStr = qmlFileName.get_cstring();
	std::string optionsStr = options.get_cstring();

	output.push_back("File: " + qmlFileNameStr);

	QFileInfo			qmlFile(QString::fromStdString(SOURCE_FOLDER + "/src/" + qmlFileNameStr + ".qml"));
	QString				qmlBaseName(qmlFile.baseName());
	if (!qmlFile.exists())
	{
		output.push_back("File NOT found");
		return output;
	}

	output.push_back("Found file");

	QUrl urlFile = QUrl::fromLocalFile(qmlFile.absoluteFilePath());
	QQmlComponent	qmlComp( engine, urlFile, QQmlComponent::PreferSynchronous);

	QQuickItem* item = qobject_cast<QQuickItem*>(qmlComp.create());

	for(const auto & error : qmlComp.errors())
		output.push_back("Error when creating component at " + std::to_string(error.line()) + "," + std::to_string(error.column()) + ": " + error.description().toStdString());

	if (!item)
	{
		output.push_back("Item not created");
		return output;
	}

	output.push_back("Item created!!!!");

	application->processEvents();

	QString returnedValue;
	QMetaObject::invokeMethod(item, "parseOptions",
		Q_RETURN_ARG(QString, returnedValue),
		Q_ARG(QString, QString::fromStdString(optionsStr)));
	output.push_back("OPTIONS: " + returnedValue.toStdString());

	return output;
}
