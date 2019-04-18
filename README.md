# KClient
[![Build status](https://ci.appveyor.com/api/projects/status/94xj04do4ej4by9y?svg=true)](https://ci.appveyor.com/project/solodyagin/kclient3)
[![Release](https://img.shields.io/github/release/solodyagin/kclient3.svg)](https://github.com/solodyagin/kclient3/releases/latest)

KClient is a web browser based on sources of Falkon. It uses QtWebEngine rendering engine.

По умолчанию применены следующие настройки:

  - Установлена тема по умолчанию - Chrome (DEFAULT_THEME_NAME=chrome)
  - Отключена проверка обновлений (DISABLE_CHECK_UPDATES=true)
  - Включена опция "Внешний вид > Расширенные настройки > Показывать строку состояния при запуске" (showStatusBar=true)
  - Отключена опция "Внешний вид > Расширенные настройка > Показывать панель закладок при запуске" (showBookmarksToolbar=false)
  - Не показывать панель поиска (ShowSearchBar=false)
  - После закрытия текущей вкладки переходить на последнюю посещённую (ActivateLastTabWhenClosingActual=true)
  - Включена опция "Вкладки > Не закрывать браузер при закрытии последней вкладки" (dontCloseWithOneTab=true)
  - Отключена опция "Вкладки > Настройка панели адреса > Включить автоматический поиск из адресной строки" (SearchFromAddressBar=false)
  - Включена опция "Вкладки > Настройка панели адреса > Показывать прогресс загрузки в адресной строке" (ShowLoadingProgress=true)
  - Включена опция "Загрузки > Закрывать менеджер загрузок после завершения всех загрузок" (CloseManagerOnFinish=true)
  - Отключен AdBlock
  - Отключена опция "Меню Вид > Панели инструментов > Вкладки сверху" (TabsOnTop=false)
  - Включена опция "Настройки > Конфиденциальность > Сообщать веб-сайтам, что я не хочу, чтобы за мной следили" (DoNotTrack=true)
  - Включена опция "Настройки > Конфиденциальность > Параметры JavaScript > Открывать всплывающие окна" (allowJavaScriptOpenWindow=true)
  - Включена опция "Вкладки > Настройка вкладок > Открывать всплывающие окна во вкладках" (OpenPopupsInTabs=true)
  - Включена опция "Вкладки > Открывать новую пустую вкладку после текущей" (newEmptyTabAfterActive=true)
  - Включена опция "Вкладки > Переключаться на открываемую вкладку" (OpenNewTabsSelected=true)

## Downloads

KClient downloads are available from [releases](https://github.com/solodyagin/kclient3/releases).
