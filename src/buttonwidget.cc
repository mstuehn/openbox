// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-

#ifdef HAVE_CONFIG_H
# include "../config.h"
#endif

#include "buttonwidget.hh"
#include "client.hh"

namespace ob {

ButtonWidget::ButtonWidget(otk::Widget *parent,
                           WidgetBase::WidgetType type,
                           Client *client)
  : otk::Widget(parent),
    WidgetBase(type),
    _client(client),
    _pressed(false),
    _button(0),
    _state(false)
{
}


ButtonWidget::~ButtonWidget()
{
}


void ButtonWidget::setTextures()
{
  bool p = _pressed;

  switch (type()) {
  case Type_AllDesktopsButton:
    if (_client->desktop() == (signed)0xffffffff)
      p = true;
    break;
  case Type_MaximizeButton:
    if (_client->maxHorz() || _client->maxVert())
      p = true;
    break;
  default:
    break;
  }
  
  switch (type()) {
  case Type_LeftGrip:
  case Type_RightGrip:
    if (_focused)
      setTexture(_style->gripFocusBackground());
    else
      setTexture(_style->gripUnfocusBackground());
    break;
  case Type_AllDesktopsButton:
  case Type_MaximizeButton:
  case Type_CloseButton:
  case Type_IconifyButton:
    if (p) {
      if (_focused)
        setTexture(_style->buttonPressFocusBackground());
      else
        setTexture(_style->buttonPressUnfocusBackground());
    } else {
      if (_focused)
        setTexture(_style->buttonUnpressFocusBackground());
      else
        setTexture(_style->buttonUnpressUnfocusBackground());
    }
    break;
  default:
    assert(false); // there's no other button widgets!
  }
}


void ButtonWidget::setStyle(otk::RenderStyle *style)
{
  otk::Widget::setStyle(style);
  setTextures();

  switch (type()) {
  case Type_LeftGrip:
  case Type_RightGrip:
    setBorderColor(_style->frameBorderColor());
    break;
  case Type_AllDesktopsButton:
  case Type_CloseButton:
  case Type_MaximizeButton:
  case Type_IconifyButton:
    break;
  default:
    assert(false); // there's no other button widgets!
  }
}


void ButtonWidget::update()
{
  switch (type()) {
  case Type_AllDesktopsButton:
    if ((_client->desktop() == (signed)0xffffffff) != _state) {
      _state = !_state;
      setTextures();
    }
    break;
  case Type_MaximizeButton:
    if ((_client->maxHorz() || _client->maxVert()) != _state) {
      _state = !_state;
      setTextures();
    }
    break;
  default:
    break;
  }
  
  otk::Widget::update();
}


void ButtonWidget::renderForeground()
{
  otk::PixmapMask *pm;
  int width;
  bool draw = _dirty;

  otk::Widget::renderForeground();

  if (draw) {
    switch (type()) {
    case Type_AllDesktopsButton:
      pm = _style->alldesktopsMask();
      break;
    case Type_CloseButton:
      pm = _style->closeMask();
      break;
    case Type_MaximizeButton:
      pm = _style->maximizeMask();
      break;
    case Type_IconifyButton:
      pm = _style->iconifyMask();
      break;
    case Type_LeftGrip:
    case Type_RightGrip:
      return; // no drawing
    default:
      assert(false); // there's no other button widgets!
    }

    assert(pm->mask);
    if (pm->mask == None) return; // no mask for the button, leave it empty

    width = _rect.width();

    otk::RenderColor *color = (_focused ? _style->buttonFocusColor() :
                               _style->buttonUnfocusColor());

    // set the clip region
    int x = (width - pm->w) / 2, y = (width - pm->h) / 2;
    XSetClipMask(**otk::display, color->gc(), pm->mask);
    XSetClipOrigin(**otk::display, color->gc(), x, y);

    // fill in the clipped region
    XFillRectangle(**otk::display, _surface->pixmap(), color->gc(), x, y,
                   x + pm->w, y + pm->h);

    // unset the clip region
    XSetClipMask(**otk::display, color->gc(), None);
    XSetClipOrigin(**otk::display, color->gc(), 0, 0);
  }
}


void ButtonWidget::adjust()
{
  // nothing to adjust. no children.
}


void ButtonWidget::focus()
{
  otk::Widget::focus();
  setTextures();
}


void ButtonWidget::unfocus()
{
  otk::Widget::unfocus();
  setTextures();
}


void ButtonWidget::buttonPressHandler(const XButtonEvent &e)
{
  otk::Widget::buttonPressHandler(e);
  if (_button) return;
  _button = e.button;
  _pressed = true;
  setTextures();
  update();
}


void ButtonWidget::buttonReleaseHandler(const XButtonEvent &e)
{
  otk::Widget::buttonPressHandler(e);
  if (e.button != _button) return;
  _button = 0;
  _pressed = false;
  setTextures();
  update();
}

}
