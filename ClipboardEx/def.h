#ifndef DEF_H
#define DEF_H
#include <QString>
#include <QColor>
#include <QDebug>

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

#define EPSINON 0.000001
static const float kShowTimeMin = 0.0f;
static const float kShowTimeDefault = 2.0f;
static const float kShowTimeMax = 5.0f;
static const float kShowTimeStep = 0.1f;
static const int kHistorySizeMin = 1;
static const int kHistorySizeDefault = 5;
static const int kHistroySizeMax = 20;
static const int kHistorySizeStep = 1;
static const int kTipsNumMin = 1;
static const int kTipsNumDefault = 1;
static const int kTipsNumMax = 20;
static const int kTipsNumStep = 1;

// snip module
static const QColor kMaskColor = QColor(0, 0, 0, 100);
static const QColor kRectColor = QColor(0, 255, 0);
// magnifier
static const QSize kWidgetSize = QSize(121, 121);
static const QSize kMinMagnifySize = QSize(31, 31);
static const QSize kMaxMagnifySize = QSize(61, 61);
static const int kMagnifyResizeStep = 2;
static const int kDistanceToCursor = 20;
static const QColor kMagnifierPenColor = QColor(0, 255, 0);
static const int kMagnifierPenWidth = 2;
static const QString kMagnifierTips ="Pos:(%1,%2)\nRGB:(%3,%4,%5)\nRGB Hex: #%6";

// dock setting
static const int kUnDockTriggerDistance = 3;
static const int kDockAnimationDuration = 200;
static const int kDockObjectFirstShowTime = 1000;

#endif // DEF_H
