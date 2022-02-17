#ifndef _UILIB_HBOX_H
#define _UILIB_HBOX_H

#include "container.h"

namespace Ui {

class HBox : public Container {
protected:
    int _padding=0;
    int _spacing=2;
public:
    HBox(int x, int y, int w, int h)
        : Container(x,y,w,h) {}
    virtual void addChild(Widget* w) override {
        if (!_children.empty()) {
            auto& last = _children.back();
            w->setPosition({last->getLeft() + last->getWidth() + _spacing, _padding});
            Container::addChild(w);
            setWidth(w->getLeft() + w->getWidth() + _padding);
            if (w->getTop() + w->getHeight() + _padding > _size.height) setHeight(w->getTop() + w->getHeight() + _padding);
        } else {
            w->setPosition({_padding,_padding});
            setSize({w->getWidth()+2*2*_padding, w->getHeight()+2*_padding});
            Container::addChild(w);
        }
        // keep track of max and min size
        _maxSize = {2*_padding,-1};
        _minSize = {2*_padding,0};
        if (!_children.empty()) {
            // handle spacing and handle case where first child has different margin
            auto child = _children.front();
            _minSize.width = child->getLeft() + _padding - _spacing;
            _maxSize.width = child->getLeft() + _padding - _spacing;
        }
        for (auto& child : _children) {
            if (_maxSize.height<0 || _maxSize.height>(child->getTop()+child->getMaxHeight()+_padding))
                _maxSize.height=child->getTop()+child->getMaxHeight()+_padding;
            if (child->getTop()+child->getMinHeight()+_padding>_minSize.height)
                _minSize.height=child->getTop()+child->getMinHeight()+_padding;
            _minSize.width += child->getMinWidth()+_spacing;
            if (_maxSize.width != -1) {
                if (child->getMaxWidth() == -1)
                    _maxSize.width = -1; // undefined = no max
                else
                    _maxSize.width += child->getMaxWidth()+_spacing;
            }
        }
        if (_maxSize.width >= 0 && _minSize.width>_maxSize.width)
            _maxSize.width = _minSize.width;
        if (_maxSize.height >= 0 && _minSize.height>_maxSize.height)
            _maxSize.height = _minSize.height;
    }
    // TODO: removeChild: update maxSize and minSize
    virtual void setSize(Size size) override {
        if (size.height < _minSize.height) {
            // this may happen if container<this widget
            size.height = _minSize.height;
        }
        if (size.width < _minSize.width) {
            // this may happen if container<this widget
            size.width = _minSize.width;
        }
        Container::setSize(size);
        // TODO: move this to relayout and run relayout instead
        for (auto child : _children) {
            child->setTop(_padding);
            child->setHeight(size.height-child->getTop()-_padding);
        }
        // FIXME: make this depend on vgrow of *all* chrildren
        if (!_children.empty() && _children.back()->getHGrow()) {
            // FIXME: this code actually gets run for every addChild()
            _children.back()->setWidth(size.width - _children.back()->getLeft()-_padding);
        }
    }
    void relayout() { // TODO: virtual?
        _minSize.width = 0; // TODO: subscribe to children's onMinWidthChanged instead
        int x=_padding;
        for (auto& child : _children) {
            child->setLeft(x);
            x += child->getWidth() + _spacing;
            _minSize.width += child->getMinWidth() + _spacing;
        }
        if (_minSize.width>0) _minSize.width += 2*_padding-_spacing;
    }
    virtual void setPadding(int padding) { _padding = padding; } // TODO: relayout?
    virtual void setSpacing(int spacing) { _spacing = spacing; } // TODO: relayout?
};

} // namespace Ui

#endif // _UILIB_HBOX_H
