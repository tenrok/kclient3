import org.kde.kclient 1.0 as KClient
import QtQuick.Controls 2.3
import QtQuick 2.3

KClient.PluginInterface {

    QtObject {
        id: helloQmlObject
        property int clickCount: 0
        signal clicked(int count)
    }

    KClient.UserScript {
        id: testingHelloQmlUserScript
        name: 'testing-hello-qml'
        runsOnSubFrames: false
        sourceCode: KClient.FileUtils.readAllFileContents('script.js')
        injectionPoint: KClient.UserScript.DocumentReady
        worldId: KClient.UserScript.ApplicationWorld
    }

    init: function(state, settingsPath){
        console.log(i18n('"Hello QML" plugin loaded'))
        KClient.ExternalJsObject.registerExtraObject({
            id: 'helloQmlObject',
            object: helloQmlObject
        })
        KClient.UserScripts.insert(testingHelloQmlUserScript)
    }

    testPlugin: function() {
        return true
    }

    unload: function() {
        console.log(i18n('Bye!'))
    }

    // Point kclient to extension://helloqml to see Hello World
    KClient.ExtensionScheme {
        name: 'helloqml'
        onRequestStarted: {
            request.reply({
                contentType: 'text/html',
                content: '<h1>Hello World</h1>'
            })
        }
    }

    KClient.BrowserAction {
        name: 'helloqml-button'
        identity: 'helloqml-id'
        title: i18n('Testing QML Title')
        toolTip: i18n('Testing QML Tooltip')
        icon: 'extensions.svg'
        location: KClient.BrowserAction.NavigationToolBar | KClient.BrowserAction.StatusBar
        popup: Rectangle {
            property var borderMargin: 1
            property var imageWidth: 256
            property var imageHeight: 200
            property var buttonHeight: 40

            width: imageWidth + 2 * borderMargin
            height: imageHeight + buttonHeight + 2 * borderMargin
            color: 'black'
            Rectangle {
                anchors.fill: parent
                anchors.leftMargin: borderMargin
                anchors.rightMargin: borderMargin
                anchors.topMargin: borderMargin
                anchors.bottomMargin: borderMargin
                color: 'white'
                Image {
                    id: image
                    source: 'qrc:/icons/other/startpage.svg'
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                }

                Button {
                    text: i18n('Click Me!')
                    height: buttonHeight
                    anchors.top: image.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    onClicked: {
                        helloQmlObject.clickCount += 1
                        helloQmlObject.clicked(helloQmlObject.clickCount)
                    }
                }
            }
        }
    }

    KClient.SideBar {
        name: 'helloqml-sidebar'
        title: i18n('Testing QML SideBar')
        icon: 'extensions.svg'
        checkable: true
        Rectangle {
            Image {
                source: 'qrc:/icons/other/startpage.svg'
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
            }

            Button {
                text: i18n('Hello Qml Plugin')
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }
    }

    populateWebViewMenu: function(menu, webHitTestResult) {
        var text = 'My first qml plugin action'
        var action = menu.addAction({
            text: text,
            icon: 'extensions.svg'
        })

        action.triggered.connect(function() {
            KClient.Notifications.create({
                heading: i18n('Hello QML'),
                message: i18n('First qml plugin action works :-)'),
                icon: 'extensions.svg'
            })
        })
    }

    KClient.Settings {
        id: settings
        name: 'HelloQML'
    }

    settingsWindow: Rectangle {
        id: window
        width: 256
        height: 200
        Image {
            id: image
            source: 'qrc:/icons/other/about.svg'
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
        }
        TextField {
            id: textField
            text: settings.value({key: 'text'})
            placeholderText: i18n('Enter text to save')
            width: 256
            height: 50
            anchors.top: image.bottom
            onTextChanged: function() {
                button.text = i18n('Save')
            }
        }
        Button {
            id: button
            text: i18n('Save')
            width: 256
            height: 50
            anchors.top: textField.bottom
            onClicked: function() {
                var res = settings.setValue({
                    key: 'text',
                    value: textField.text
                })
                if (res) {
                    button.text = i18n('Saved!')
                } else {
                    button.text = i18n('Error occurred, try again!')
                }
            }
        }
    }
}
