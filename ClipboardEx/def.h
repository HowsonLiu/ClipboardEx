#ifndef DEF_H
#define DEF_H
#include <QString>

// platform
#define UWP

// define control name
static const QString kTipsWindow = "TipsWindow";
static const QString kMainLabel = "MainLabel";
static const QString kSubLabel = "SubLabel";
static const QString kLabelList = "LabelList";
static const QString kExpandBtn = "ExpandBtn";
static const QString kAutoShowBtn = "AutoShowBtn";

static const QString kTrayMenu = "TrayMenu";
static const QString kStartUpAction = "StartUpAction";

static const QString kMenuSubWidget = "MenuSubWidget";
static const QString kMenuSubTitle = "MenuSubTitle";
static const QString kMenuSubValue = "MenuSubValue";
static const QString kMenuSubLBtn = "MenuSubLBtn";
static const QString kMenuSubRBtn = "MenuSubRBtn";

// define style
#define WHEEL_SCROLL_PER_PIXEL

// define history size type
#define HISTORYSIZE_NOT_CONTAINS_CURRENT

static const int kMinFontSize = 15;			// Tips window font min pixel size
static const int kTrayMsgShowTime = 100;	// Tray Message show time ms

static const int kHistorySizeMin = 1;
static const int kHistorySizeDefault = 5;
static const int kHistroySizeMax = 20;
static const int kTipsNumMin = 1;
static const int kTipsNumDefault = 1;
static const int kTipsNumMax = 20;

#endif // DEF_H
