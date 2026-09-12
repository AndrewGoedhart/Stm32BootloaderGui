#include <logView.h>
#include <ui_logView.h>
#include <QFileDialog>

/**
 * @brief LogView::LogView Std output windoe for trace, debug, testing
 * etc.
 * @param parent
 */

LogView::LogView(QWidget *parent)
    : QWidget(parent), ui(new Ui::LogView), _model(new QStringListModel()),
      _paused(false), _traceFile(), _maxLineLength(200), _maxLines(500), _timestampChanged(true) {
  ui->setupUi(this);
  ui->logView->setModel(_model);
  _start = StopwatchTime{std::chrono::steady_clock::now()};
}

LogView::~LogView() { delete ui; }

void LogView::logLine(QString line) {
  _traceFile.log(formatLine(line));
  if (!_paused) {
    int rows = _model->rowCount();
    if (rows > _maxLines) {
      rows -= 50;
      _model->removeRows(0, REMOVE_LINES);
    }
    _model->insertRows(rows, 1);
    _model->setData(_model->index(rows), line);
    ui->logView->setCurrentIndex(_model->index(rows));
  }
}

void LogView::log(QString text) {
  _traceFile.log(formatLine(text));
  if (!_paused) {
    auto lines = splitOnNewLine(text);
    updateFirstRowWithExistingData(lines);
    updateWithTimestamp(lines);
    insertCombinedTraceRows(lines);
  }
}

void LogView::updateWithTimestamp(QStringList &lines){
  bool firstLine = true;
  for (auto &line : lines) {
    if( !firstLine){
      line = formatLine(line);
    } else {
      firstLine = false;
    }
  }
}

QStringList LogView::splitOnNewLine(QString text) {
  QStringList newLines;
  if (text.contains('\n')) {
    newLines = text.split("\n", Qt::SkipEmptyParts,
                          Qt::CaseSensitivity::CaseSensitive);
    if (text.right(1) == "\n") {
      newLines.append("");
    }
    if (text.left(1) == "\n") {
      newLines.insert(0,"");
    }
  } else {
    newLines.append(text);
  }
  return newLines;
}

QString LogView::formatLine(QString line){
  return QString("%1 %2").arg(_offset.count() /10, 7).arg(line);
}



void LogView::updateFirstRowWithExistingData(QStringList &newLines) {
  int rowCount = _model->rowCount();
  if (rowCount > 0) {
    QString currentRow = _model->data(_model->index(rowCount - 1)).toString();
    if (currentRow.length() > _maxLineLength) {
      newLines.insert(0, currentRow);
    } else {
      newLines[0] = currentRow + newLines[0];
    }
  } else {
    newLines[0] = formatLine(newLines[0]);
  }
}

void LogView::insertCombinedTraceRows(QStringList newLines) {
  if (newLines.count() > 0) {
    if (_model->rowCount() > _maxLines) {
      _model->removeRows(0, REMOVE_LINES);
    }
    int rowCount = _model->rowCount();
    if (rowCount > 0) {
      if (newLines.count() > 1) {
        _model->insertRows(rowCount, newLines.count() - 1);
      }
    } else {
      _model->insertRows(rowCount, newLines.count());
      rowCount = 1;
    }
    for (auto line : newLines) {
      QModelIndex index = _model->index(rowCount - 1);
      _model->setData(index, line);
      ++rowCount;
    }
    ui->logView->setCurrentIndex(_model->index(rowCount - 1));
  }
}

void LogView::updateTime()
{
  _timestampChanged = true;
  _offset = std::chrono::duration_cast<Milliseconds>(StopwatchTime{std::chrono::steady_clock::now()} - _start);
}

void LogView::on_logActiveCb_toggled(bool checked) {
  if (checked) {
    _traceFile.createLogFile(ui->logEdt->text());
    if (!_traceFile.isLogging()) {
      ui->logActiveCb->setCheckState(Qt::CheckState::Unchecked);
    }
  }
}

void LogView::on_clearBtn_clicked() {
  _model->setStringList(QStringList());
}

void LogView::on_logFileBtn_clicked() {
  QString traceFile = QFileDialog::getSaveFileName(
      this, tr("Save Trace File"), "./", tr("Log Files(*.log)"));
  ui->logEdt->setText(traceFile);
}

void LogView::on_pauseButton_clicked() { _paused = !_paused; }
