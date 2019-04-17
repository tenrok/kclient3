function init() {
    external.extra.helloQmlObject.clicked.connect(function(count){
        alert('You clicked ' + count + ' times')
    })
}

if (window._kclient_external) {
    init();
} else {
    document.addEventListener("_kclient_external_created", init);
}
