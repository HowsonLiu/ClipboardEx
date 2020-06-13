#include "mimedatalabel.h"
#include "def.h"
#include "MainControl.h"
#include <QStyle>
#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>

MimeDataLabel::MimeDataLabel(QWidget* parent /*= nullptr*/) : QLabel(parent)
, m_bindMimeData(nullptr)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QLabel::customContextMenuRequested, this, &MimeDataLabel::onContentMenu);
}

QRect MimeDataLabel::textRect()
{
	QRect rect = contentsRect();
	int m = margin(); rect.adjust(m, m, -m, -m);
	layoutDirection();
	const int align = QStyle::visualAlignment(layoutDirection(), QLabel::alignment());
	int i = indent();
	if (i < 0 && frameWidth()) { // no indent, but we do have a frame
		m = fontMetrics().width(QLatin1Char('x')) / 2 - m;
	}
	if (m > 0) {
		if (align & Qt::AlignLeft) rect.setLeft(rect.left() + m);
		if (align & Qt::AlignRight) rect.setRight(rect.right() - m);
		if (align & Qt::AlignTop) rect.setTop(rect.top() + m);
		if (align & Qt::AlignBottom) rect.setBottom(rect.bottom() - m);
	}
	return rect;
}

void MimeDataLabel::showMimeData()
{
	if (!m_bindMimeData) return;

	if (m_bindMimeData->hasImage()) {
		setAlignment(Qt::AlignCenter);
		setPixmap(QPixmap::fromImage(m_bindMimeData->image).scaled(size(), Qt::KeepAspectRatio));
		return;
	}

	QString text;
	if (m_bindMimeData->hasText()) {
		text = m_bindMimeData->text;
	}
	else if (m_bindMimeData->hasUrls()) {
		for (auto url : m_bindMimeData->urls)
			text += url.toString() + "\n";
	}
	else {
		text = "UnKnown";
	}
	showText(text);
}

void MimeDataLabel::showText(const QString& text)
{
	QString showText = text;
	QStringList strList = showText.split('\n');
	if (strList.back().isEmpty()) strList.pop_back();
	bool isSingleLine = strList.size() == 1;
	if (isSingleLine) {
		setWordWrap(true);
		showText = strList[0].trimmed();
	}
	else {
		setWordWrap(false);
		// remote front spaces
		QRegExp spaceReg("^\\s+");
		int len = INT_MAX;
		for (auto& str : strList) {
			spaceReg.indexIn(str);
			len = len > spaceReg.matchedLength() ? spaceReg.matchedLength() : len;
			if (len == -1) break;
		}
		if (len > 0 && len != INT_MAX) {
			for (auto& str : strList)
				str = str.mid(len);
			showText = strList.join('\n');
		}
	}

	QRect textRect = this->textRect();
	QFont font = this->font();
	int fontSize = kMinFontSize;
	font.setPixelSize(fontSize);
	QFontMetrics fontMetrics = QFontMetrics(font);
	QRect rect = fontMetrics.boundingRect(textRect, isSingleLine ? Qt::TextWordWrap : 0, showText);
	while (rect.width() < textRect.width() && rect.height() < textRect.height()) {
		font.setPixelSize(++fontSize);
		fontMetrics = QFontMetrics(font);
		rect = fontMetrics.boundingRect(textRect, isSingleLine ? Qt::TextWordWrap : 0, showText);
	}
	font.setPixelSize(fontSize > kMinFontSize ? fontSize - 1 : kMinFontSize);
	setAlignment(rect.height() < textRect.height() ? Qt::AlignLeft | Qt::AlignVCenter : Qt::AlignLeft);
	setFont(font);
	setText(showText);
}

void MimeDataLabel::onDoubleClicked()
{
	if (!m_bindMimeData || !m_bindMimeData->isValid()) return;
	m_bindMimeData->copyToClipboard();
}

void MimeDataLabel::onContentMenu(const QPoint& p)
{
	QMenu contextMenu(this);
	QAction saveAsAction(tr("Save as...") ,this);

	contextMenu.setObjectName("ContextMenu");
	contextMenu.setStyleSheet(MainControl::getInstance()->getMenuQss());
	
	connect(&saveAsAction, &QAction::triggered, this, &MimeDataLabel::onSaveAs);
	contextMenu.addAction(&saveAsAction);

	emit sigContentMenuShow(true);
	contextMenu.exec(mapToGlobal(p));
	emit sigContentMenuShow(false);
}

void MimeDataLabel::onSaveAs()
{
	static QString path;
	QString typeStr;
	if (m_bindMimeData->hasImage())
		typeStr = "PNG File (*.png);; JPG File (*.jpg);; BMP File(*.bmp)";
	else if (m_bindMimeData->hasText() || m_bindMimeData->hasUrls())
		typeStr = "Text File (*.txt);; XML File (*.xml)";
	if (typeStr.isEmpty()) return;
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), path, typeStr);
	if (fileName.isEmpty()) return;
	path = QFileInfo(fileName).absolutePath();
	if (m_bindMimeData->hasImage()) {
		m_bindMimeData->image.save(fileName);
	}
	else {
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QMessageBox::warning(this, tr("Save error"), tr("Can't open file"));
			return;
		}
		QTextStream textStream(&file);
		if (m_bindMimeData->hasText()) {
			textStream << m_bindMimeData->text;
		}
		else {
			for (auto url : m_bindMimeData->urls)
				textStream << url.toString() << "\n";
		}
		file.close();
	}
}
