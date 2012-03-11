#include "ContrastInspector.h"
#include "ui_ContrastInspector.h"
#include "QtStyles.h"
#include "IntensityCurveModel.h"
#include "QtWidgetCoupling.h"
#include "QtReporterDelegates.h"

ContrastInspector::ContrastInspector(QWidget *parent) :
    SNAPComponent(parent),
    ui(new Ui::ContrastInspector)
{
  ui->setupUi(this);
  ApplyCSS(this, ":/root/itksnap.css");

  // Create the viewport reporter
  m_CurveBoxViewportReporter = new QtViewportReporter(ui->box);
}

ContrastInspector::~ContrastInspector()
{
  delete ui;
}

void ContrastInspector::SetModel(IntensityCurveModel *model)
{
  // Set the model
  m_Model = model;
  ui->box->SetModel(model);

  // Connect the viewport reporter to the model
  model->SetViewportReporter(m_CurveBoxViewportReporter);

  // Listen to model update events
  connectITK(m_Model, ModelUpdateEvent());

  // Set up the couplings. This is all we have to do to make the spin box
  // play with the model! There are no callbacks to write, no event handlers
  // to worry about! Yay!!!
  makeArrayCoupling(ui->inControlX, ui->inControlY,
                    m_Model->GetMovingControlXYModel());

  // Set up couplings for window and level
  makeArrayCoupling(ui->inLevel, ui->inWindow,
                    m_Model->GetLevelWindowModel());

  // Make coupling for the control point id
  makeCoupling(ui->inControlId, m_Model->GetMovingControlIdModel());

  // Histogram bin controls
  makeCoupling(ui->inBinSize, m_Model->GetHistogramBinSizeModel());
  makeCoupling(ui->inCutoff, m_Model->GetHistogramCutoffModel());
  makeCoupling(ui->inLogScale, m_Model->GetHistogramScaleModel());
}

void ContrastInspector::on_btnRemoveControl_clicked()
{
  m_Model->OnControlPointNumberDecreaseAction();
}

void ContrastInspector::on_btnAddControl_clicked()
{
  m_Model->OnControlPointNumberIncreaseAction();
}

void ContrastInspector::on_btnReset_clicked()
{
  m_Model->OnResetCurveAction();
}

void ContrastInspector::onModelUpdate(const EventBucket &b)
{
  m_Model->Update();
}


void ContrastInspector::on_btnAuto_clicked()
{
  m_Model->OnAutoFitWindow();
}