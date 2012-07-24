#include "CursorInspector.h"
#include "ui_CursorInspector.h"
#include <VoxelIntensityQTableModel.h>
#include <QSpinBox>
#include <GenericImageData.h>
#include <IRISApplication.h>
#include <CursorInspectionModel.h>
#include <GlobalUIModel.h>
#include <QtWidgetActivator.h>
#include <QMenu>
#include <IntensityCurveModel.h>
#include <LayerSelectionModel.h>

#include <QtSpinBoxCoupling.h>
#include <QtLineEditCoupling.h>
#include <QtTableWidgetCoupling.h>.h>
#include <QtWidgetArrayCoupling.h>

/**
  This class provide the coupling properties for coupling the table of
  voxel intensities to QTableWidget
  */
class LayerCurrentVoxelInfoRowDescriptionTraits
{
public:
  typedef CurrentVoxelInfoItemSetDomain::ValueType ValueType;

  static QString GetText(ValueType value,
                         const LayerCurrentVoxelInfo &desc, int col)
  {
    if(col == 0)
      return QString::fromStdString(desc.LayerName);
    else
      return QString::fromStdString(desc.IntensityValue);
  }

  static QIcon GetIcon(ValueType value,
                       const LayerCurrentVoxelInfo &desc, int col)
  {
    return QIcon();
  }


  static QVariant GetIconSignature(ValueType value,
                                   const LayerCurrentVoxelInfo &desc, int col)
  {
    return QVariant(0);
  }
};

typedef TextAndIconTableWidgetRowTraits<
  size_t, LayerCurrentVoxelInfo,
  LayerCurrentVoxelInfoRowDescriptionTraits> LayerCurrentVoxelInfoTableWidgetRowTraits;

typedef ItemSetWidgetDomainTraits<
  CurrentVoxelInfoItemSetDomain,
  QTableWidget,
  LayerCurrentVoxelInfoTableWidgetRowTraits> LayerCurrentVoxelInfoDomainTraits;







CursorInspector::CursorInspector(QWidget *parent) :
    SNAPComponent(parent),
    ui(new Ui::CursorInspector)
{
  ui->setupUi(this);

  ui->tableVoxelUnderCursor->setAlternatingRowColors(true);
  ui->tableVoxelUnderCursor->setFixedWidth(160);
  ui->tableVoxelUnderCursor->setFixedHeight(120);
  ui->tableVoxelUnderCursor->setContextMenuPolicy(Qt::CustomContextMenu);

  m_ContextMenu = new QMenu(ui->tableVoxelUnderCursor);
  m_ContextMenu->addAction(ui->actionAutoContrast);

  // Hook up the context menu
  connect(ui->tableVoxelUnderCursor, SIGNAL(customContextMenuRequested(QPoint)),
          SLOT(onContextMenuRequested(QPoint)));

}

CursorInspector::~CursorInspector()
{
  delete ui;
}

void CursorInspector::SetModel(CursorInspectionModel *model)
{
  m_Model = model;

  // Activators
  activateOnFlag(this, m_Model->GetParent(), UIF_BASEIMG_LOADED);

  // Couple to the model
  makeCoupling(ui->outLabelId, m_Model->GetLabelUnderTheCursorIdModel());
  makeCoupling(ui->outLabelText, m_Model->GetLabelUnderTheCursorTitleModel());

  makeArrayCoupling(ui->inCursorX, ui->inCursorY, ui->inCursorZ,
                    m_Model->GetCursorPositionModel());

  makeCoupling(ui->tableVoxelUnderCursor,
               m_Model->GetVoxelAtCursorModel(),
               DefaultWidgetValueTraits<size_t , QTableWidget>(),
               LayerCurrentVoxelInfoDomainTraits());
}

void CursorInspector::onContextMenuRequested(QPoint pos)
{
  m_PopupRow = ui->tableVoxelUnderCursor->rowAt(pos.y());
  if(m_PopupRow >= 0)
    m_ContextMenu->popup(QCursor::pos());
}

void CursorInspector::on_actionAutoContrast_triggered()
{
  LayerIterator it =
      m_Model->GetParent()->GetLoadedLayersSelectionModel()->GetNthLayer(m_PopupRow);

  if(it.GetLayerAsGray())
    {
    // Select the currently highlighted layer
    m_Model->GetParent()->GetIntensityCurveModel()->SetLayer(it.GetLayerAsGray());

    // Auto-adjust intensity in the selected layer
    m_Model->GetParent()->GetIntensityCurveModel()->OnAutoFitWindow();
    }
}
