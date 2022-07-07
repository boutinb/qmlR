import QtQuick 2.15
import JASP.Controls 1.0

Item
{
	id: someID

	property string myProperty: "Hello QML World"

	CheckBox
	{
		name: "test"
		isBound: false
	}

}
