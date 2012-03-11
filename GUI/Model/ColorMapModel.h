#ifndef COLORMAPMODEL_H
#define COLORMAPMODEL_H

#include "AbstractLayerAssociatedModel.h"
#include "EditableNumericValueModel.h"
#include "GreyImageWrapper.h"
#include <UIReporterDelegates.h>


class ColorMap;
class SystemInterface;

class ColorMapLayerProperties
{
public:

  irisGetSetMacro(ObserverTag, unsigned long)

  ColorMapLayerProperties()
  {
    m_SelectedControlIndex = -1;
    m_SelectedControlSide = NA;
    m_ObserverTag = 0;
  }

  enum Side {LEFT = 0, RIGHT, NA};

  irisGetSetMacro(SelectedControlIndex, int)
  irisGetSetMacro(SelectedControlSide, Side)
  irisGetSetMacro(SelectedPreset, std::string)

protected:

  // Control point being edited
  int m_SelectedControlIndex;

  // Side of the control point, if discontinuous
  Side m_SelectedControlSide;

  // The index of the current preset
  std::string m_SelectedPreset;

  // Whether or not we are already listening to events from this layer
  unsigned long m_ObserverTag;
};

typedef AbstractLayerAssociatedModel<
    ColorMapLayerProperties,
    GreyImageWrapperBase> ColorMapModelBase;


/**
  The UI model for color map editing
  */
class ColorMapModel : public ColorMapModelBase
{
public:
  irisITKObjectMacro(ColorMapModel, ColorMapModelBase)

  typedef ColorMapLayerProperties::Side Side;
  typedef ColorMap::CMPointType Continuity;
  typedef std::vector<std::string> PresetList;

  // This event only affects this model
  itkEventMacro(PresetUpdateEvent, IRISEvent)

  // This event is fired when the presets are changed
  FIRES(PresetUpdateEvent)

  /**
    States in which the model can be, which allow the activation and
    deactivation of various widgets in the interface
    */
  enum UIState {
    UIF_LAYER_ACTIVE,
    UIF_CONTROL_SELECTED,
    UIF_CONTROL_SELECTED_IS_NOT_ENDPOINT,
    UIF_CONTROL_SELECTED_IS_DISCONTINUOUS,
    UIF_USER_PRESET_SELECTED
    };

  void SetParentModel(GlobalUIModel *parent);

  /**
    Check the state flags above
    */
  bool CheckState(UIState state);

  // Implementation of virtual functions from parent class
  void RegisterWithLayer(GreyImageWrapperBase *layer);
  void UnRegisterFromLayer(GreyImageWrapperBase *layer);

  /** Before using the model, it must be coupled with a size reporter */
  irisGetSetMacro(ViewportReporter, ViewportSizeReporter *)

  /**
    Process colormap box interaction events
    */
  bool ProcessMousePressEvent(const Vector3d &x);
  bool ProcessMouseDragEvent(const Vector3d &x);
  bool ProcessMouseReleaseEvent(const Vector3d &x);

  /** Update the model in response to upstream events */
  virtual void OnUpdate();

  typedef AbstractRangedPropertyModel<double>::Type RealValueModel;
  typedef AbstractRangedPropertyModel<int>::Type IntegerValueModel;
  typedef AbstractRangedPropertyModel<bool>::Type BooleanValueModel;
  typedef AbstractPropertyModel<Continuity> ContinuityValueModel;
  typedef AbstractPropertyModel<Side> SideValueModel;

  /** The model for setting moving control point position */
  irisGetMacro(MovingControlPositionModel, RealValueModel *)

  /** The model for setting moving control point opacity */
  irisGetMacro(MovingControlOpacityModel, RealValueModel *)

  /** The model for setting moving control point continuity */
  irisGetMacro(MovingControlContinuityModel, ContinuityValueModel *)

  /** The model for setting moving control point side */
  irisGetMacro(MovingControlSideModel, SideValueModel *)

  /** The index of the moving control point */
  irisGetMacro(MovingControlIndexModel, IntegerValueModel *)

  /** Get the color map in associated layer */
  ColorMap *GetColorMap();

  /** Check whether a particular control point is selected */
  bool IsControlSelected(int cp, Side side);

  /** Set the selected control point, return true if selection
      changed as the result */
  bool SetSelection(int cp, Side side = ColorMapLayerProperties::NA);

  /** Delete the selected control */
  void DeleteSelectedControl();

  /** Get the color of the selected point */
  Vector3d GetSelectedColor();

  /** Set the color of the selected point */
  void SetSelectedColor(Vector3d rgb);

  /** Get the list of color map presets */
  void GetPresets(PresetList &system, PresetList &user);

  /** Select one of the presets. The index is into the combined list
    of system and user presets */
  void SelectPreset(const char *preset);

  /** Save the current state as a preset */
  void SaveAsPreset(std::string name);

  /** Delete a selected preset */
  void DeletePreset(std::string name);

protected:

  ColorMapModel();

  SmartPtr<RealValueModel> m_MovingControlPositionModel;
  SmartPtr<RealValueModel> m_MovingControlOpacityModel;
  SmartPtr<SideValueModel> m_MovingControlSideModel;
  SmartPtr<ContinuityValueModel> m_MovingControlContinuityModel;
  SmartPtr<IntegerValueModel> m_MovingControlIndexModel;

  // A pointer to the system interface object
  SystemInterface *m_System;

  // A size reporter delegate (notifies the model when the size of the
  // corresponding widget changes).
  ViewportSizeReporter *m_ViewportReporter;

  // Colormap presets
  PresetList m_PresetSystem, m_PresetUser;

  // Get the RGBA for selected point
  bool GetSelectedRGBA(ColorMap::RGBAType &rgba);
  void SetSelectedRGBA(ColorMap::RGBAType rgba);

  // Control point position
  bool GetMovingControlPositionValueAndRange(
      double &value, NumericValueRange<double> *range);
  void SetMovingControlPosition(double value);

  // Control point opacity
  bool GetMovingControlOpacityValueAndRange(
      double &value, NumericValueRange<double> *range);
  void SetMovingControlOpacity(double value);

  // Control point style
  bool GetMovingControlType(Continuity &value);
  void SetMovingControlType(Continuity value);

  // Control point style
  bool GetMovingControlSide(Side &value);
  void SetMovingControlSide(Side value);

  // Selected control point index
  bool GetMovingControlIndexValueAndRange(
      int &value, NumericValueRange<int> *range);
  void SetMovingControlIndex(int value);
};

#endif // COLORMAPMODEL_H