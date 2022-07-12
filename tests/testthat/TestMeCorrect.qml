import QtQuick 2.15
//import "../testthat/extra" as TT
import "../../../jasp-desktop/QMLComponents/components/JASP/Controls" as JC
//import JASP.Controls 1.0 as JC

Item
{
	id: someID

	property string myProperty: "Hello QML World"
	property alias integerValue: myField.value

	JC.IntegerField
	{
		id: myField
		name: "test"
		isBound: false
		max: 10
	}

}
